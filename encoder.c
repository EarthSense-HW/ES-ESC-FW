/*
	Copyright 2016 Benjamin Vedder	benjamin@vedder.se

	This file is part of the VESC firmware.

	The VESC firmware is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The VESC firmware is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    */

#include "encoder.h"
#include "ch.h"
#include "hal.h"
#include "stm32f4xx_conf.h"
#include "hw.h"
#include "mc_interface.h"
#include "utils.h"
#include <math.h>
#include "commands.h"
#include "i2c_bb.h"

    uint8_t rxbuf[2];
    uint8_t txbuf[1];
    msg_t status = MSG_OK;
    systime_t tmo = MS2ST(10);
    uint8_t as5048_addr = 0x40;

    uint16_t angMSB=0;
    uint16_t angLSB=0;
    uint32_t ang_jb=0;

    uint8_t txMSB[1] = {0xFE};
    uint8_t txLSB[1] = {0xFF};


// Defines
#define AS5047P_READ_ANGLECOM		(0x3FFF | 0x4000 | 0x8000) // This is just ones
#define AS5047_SAMPLE_RATE_HZ		20000
#define AD2S1205_SAMPLE_RATE_HZ		20000		//25MHz max spi clk
#define SINCOS_SAMPLE_RATE_HZ		20000
#define SINCOS_MIN_AMPLITUDE		1.0			// sqrt(sin^2 + cos^2) has to be larger than this
#define SINCOS_MAX_AMPLITUDE		1.65		// sqrt(sin^2 + cos^2) has to be smaller than this

#if (AS5047_USE_HW_SPI_PINS || AD2S1205_USE_HW_SPI_PINS)
#ifdef HW_SPI_DEV
#define SPI_SW_MISO_GPIO			HW_SPI_PORT_MISO
#define SPI_SW_MISO_PIN				HW_SPI_PIN_MISO
#define SPI_SW_MOSI_GPIO			HW_SPI_PORT_MOSI
#define SPI_SW_MOSI_PIN				HW_SPI_PIN_MOSI
#define SPI_SW_SCK_GPIO				HW_SPI_PORT_SCK
#define SPI_SW_SCK_PIN				HW_SPI_PIN_SCK
#define SPI_SW_CS_GPIO				HW_SPI_PORT_NSS
#define SPI_SW_CS_PIN				HW_SPI_PIN_NSS
#else
// Note: These values are hardcoded.
#define SPI_SW_MISO_GPIO			GPIOB
#define SPI_SW_MISO_PIN				4
#define SPI_SW_MOSI_GPIO			GPIOB
#define SPI_SW_MOSI_PIN				5
#define SPI_SW_SCK_GPIO				GPIOB
#define SPI_SW_SCK_PIN				3
#define SPI_SW_CS_GPIO				GPIOB
#define SPI_SW_CS_PIN				0
#endif
#else
#define SPI_SW_MISO_GPIO			HW_HALL_ENC_GPIO2
#define SPI_SW_MISO_PIN				HW_HALL_ENC_PIN2
#define SPI_SW_SCK_GPIO				HW_HALL_ENC_GPIO1
#define SPI_SW_SCK_PIN				HW_HALL_ENC_PIN1
#define SPI_SW_CS_GPIO				HW_HALL_ENC_GPIO3
#define SPI_SW_CS_PIN				HW_HALL_ENC_PIN3
#endif

// Private types
typedef enum {
	ENCODER_MODE_NONE = 0,
	ENCODER_MODE_ABI,
	ENCODER_MODE_AS5047P_SPI,
	RESOLVER_MODE_AD2S1205,
	ENCODER_MODE_SINCOS,
	ENCODER_MODE_TS5700N8501
} encoder_mode;

// Private variables
//I2C
static volatile bool stop_now = true;
static volatile bool is_running = false;
static volatile chuck_data chuck_d;
static volatile int chuck_error = 0;
static volatile chuk_config config;
static volatile bool output_running = false;
static volatile systime_t last_update_time;
// Software I2C
static i2c_bb_state i2cs;
// END I2C

static bool index_found = false;
static int32_t enc_counts = 10000;
static encoder_mode mode = ENCODER_MODE_NONE;
static float last_enc_angle = 0.0;
static float enc_angle_abs_deg = 0.0;
static float enc_angle_deg = 0.0;
static int32_t cumulative_encoder_counts = 0;
static uint32_t last_encoder_counts = 0;
static float spi_val = 0.0;
static uint32_t spi_error_cnt = 0;
static float spi_error_rate = 0.0;
static float resolver_loss_of_tracking_error_rate = 0.0;
static float resolver_degradation_of_signal_error_rate = 0.0;
static float resolver_loss_of_signal_error_rate = 0.0;
static uint32_t resolver_loss_of_tracking_error_cnt = 0;
static uint32_t resolver_degradation_of_signal_error_cnt = 0;
static uint32_t resolver_loss_of_signal_error_cnt = 0;

static float sin_gain = 0.0;
static float sin_offset = 0.0;
static float cos_gain = 0.0;
static float cos_offset = 0.0;
static float sincos_filter_constant = 0.0;
static uint32_t sincos_signal_below_min_error_cnt = 0;
static uint32_t sincos_signal_above_max_error_cnt = 0;
static float sincos_signal_low_error_rate = 0.0;
static float sincos_signal_above_max_error_rate = 0.0;

static int sw_i2c_err = 1;


static SerialConfig TS5700N8501_uart_cfg = {
		2500000,
		0,
		USART_CR2_LINEN,
		0
};
// I2C :)
// Threads
static THD_FUNCTION(as5048_thread, arg);
static THD_WORKING_AREA(as5048_thread_wa, 1024);


static THD_FUNCTION(ts5700n8501_thread, arg);
static THD_WORKING_AREA(ts5700n8501_thread_wa, 512);
static volatile bool ts5700n8501_stop_now = true;
static volatile bool ts5700n8501_is_running = false;
static volatile uint8_t ts5700n8501_raw_status[8] = {0};
static volatile bool ts5700n8501_reset_errors = false;
static volatile bool ts5700n8501_reset_multiturn = false;

// Private functions
static void spi_transfer(uint16_t *in_buf, const uint16_t *out_buf, int length);
static void spi_begin(void);
static void spi_end(void);
static void spi_delay(void);
static void TS5700N8501_send_byte(uint8_t b);

uint32_t encoder_spi_get_error_cnt(void) {
	return spi_error_cnt;
}

float encoder_spi_get_val(void) {
	return spi_val;
}

float encoder_get_abs_angle(void) {
    return enc_angle_abs_deg;
}

float encoder_get_angle(void) {
    return enc_angle_deg;
}

float encoder_spi_get_error_rate(void) {
	return spi_error_rate;
}

float encoder_resolver_loss_of_tracking_error_rate(void) {
	return resolver_loss_of_tracking_error_rate;
}

float encoder_resolver_degradation_of_signal_error_rate(void) {
	return resolver_degradation_of_signal_error_rate;
}

float encoder_resolver_loss_of_signal_error_rate(void) {
	return resolver_loss_of_signal_error_rate;
}

uint32_t encoder_resolver_loss_of_tracking_error_cnt(void) {
	return resolver_loss_of_tracking_error_cnt;
}

uint32_t encoder_resolver_degradation_of_signal_error_cnt(void) {
	return resolver_degradation_of_signal_error_cnt;
}

uint32_t encoder_resolver_loss_of_signal_error_cnt(void) {
	return resolver_loss_of_signal_error_cnt;
}

uint32_t encoder_sincos_get_signal_below_min_error_cnt(void) {
	return sincos_signal_below_min_error_cnt;
}

uint32_t encoder_sincos_get_signal_above_max_error_cnt(void) {
	return sincos_signal_above_max_error_cnt;
}

float encoder_sincos_get_signal_below_min_error_rate(void) {
	return sincos_signal_low_error_rate;
}

float encoder_sincos_get_signal_above_max_error_rate(void) {
	return sincos_signal_above_max_error_rate;
}

uint8_t* encoder_ts5700n8501_get_raw_status(void) {
	return (uint8_t*)ts5700n8501_raw_status;
}

int16_t encoder_ts57n8501_get_abm(void) {
	return (uint16_t)ts5700n8501_raw_status[4] |
			((uint16_t)ts5700n8501_raw_status[5] << 8);
}

void encoder_ts57n8501_reset_errors(void) {
	ts5700n8501_reset_errors = true;
}

void encoder_ts57n8501_reset_multiturn(void) {
	ts5700n8501_reset_multiturn = true;
}

void encoder_deinit(void) {
	nvicDisableVector(HW_ENC_EXTI_CH);
	nvicDisableVector(HW_ENC_TIM_ISR_CH);

	TIM_DeInit(HW_ENC_TIM);

	palSetPadMode(SPI_SW_MISO_GPIO, SPI_SW_MISO_PIN, PAL_MODE_INPUT);
	palSetPadMode(SPI_SW_SCK_GPIO, SPI_SW_SCK_PIN, PAL_MODE_INPUT);
	palSetPadMode(SPI_SW_CS_GPIO, SPI_SW_CS_PIN, PAL_MODE_INPUT);

	palSetPadMode(HW_HALL_ENC_GPIO1, HW_HALL_ENC_PIN1, PAL_MODE_INPUT_PULLUP);
	palSetPadMode(HW_HALL_ENC_GPIO2, HW_HALL_ENC_PIN2, PAL_MODE_INPUT_PULLUP);

	if (mode == ENCODER_MODE_TS5700N8501) {
		ts5700n8501_stop_now = true;
		while (ts5700n8501_is_running) {
			chThdSleepMilliseconds(1);
		}

		palSetPadMode(HW_UART_TX_PORT, HW_UART_TX_PIN, PAL_MODE_INPUT);
		palSetPadMode(HW_UART_RX_PORT, HW_UART_RX_PIN, PAL_MODE_INPUT);
#ifdef HW_ADC_EXT_GPIO
		palSetPadMode(HW_ADC_EXT_GPIO, HW_ADC_EXT_PIN, PAL_MODE_INPUT_ANALOG);
#endif
	}

	index_found = false;
	mode = ENCODER_MODE_NONE;
	last_enc_angle = 0.0;
	spi_error_rate = 0.0;
	sincos_signal_low_error_rate = 0.0;
	sincos_signal_above_max_error_rate = 0.0;
}

void encoder_init_abi(uint32_t counts) {
	EXTI_InitTypeDef   EXTI_InitStructure;

	// Initialize variables
	index_found = false;
	enc_counts = counts;

	palSetPadMode(HW_HALL_ENC_GPIO1, HW_HALL_ENC_PIN1, PAL_MODE_ALTERNATE(HW_ENC_TIM_AF));
	palSetPadMode(HW_HALL_ENC_GPIO2, HW_HALL_ENC_PIN2, PAL_MODE_ALTERNATE(HW_ENC_TIM_AF));
//	palSetPadMode(HW_HALL_ENC_GPIO3, HW_HALL_ENC_PIN3, PAL_MODE_ALTERNATE(HW_ENC_TIM_AF));

	// Enable timer clock
	HW_ENC_TIM_CLK_EN();

	// Enable SYSCFG clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	TIM_EncoderInterfaceConfig (HW_ENC_TIM, TIM_EncoderMode_TI12,
			TIM_ICPolarity_Rising,
			TIM_ICPolarity_Rising);
	TIM_SetAutoreload(HW_ENC_TIM, enc_counts - 1);

	// Filter
	HW_ENC_TIM->CCMR1 |= 6 << 12 | 6 << 4;
	HW_ENC_TIM->CCMR2 |= 6 << 4;

	TIM_Cmd(HW_ENC_TIM, ENABLE);

	// Interrupt on index pulse

	// Connect EXTI Line to pin
	SYSCFG_EXTILineConfig(HW_ENC_EXTI_PORTSRC, HW_ENC_EXTI_PINSRC);

	// Configure EXTI Line
	EXTI_InitStructure.EXTI_Line = HW_ENC_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	// Enable and set EXTI Line Interrupt to the highest priority
	nvicEnableVector(HW_ENC_EXTI_CH, 0);

	mode = ENCODER_MODE_ABI;
}

//static const SPIConfig ls_spicfg = {
//  false,
//  NULL,
//  GPIOA,
//  4,
//  SPI_CR1_BR_2 | SPI_CR1_BR_1,
//  0
//};

void encoder_init_as5047p_spi(void) {
    //commands_printf("Init Encoder Start");
	//TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

#ifdef USING_HARDWARE_I2C_ENCODER
	stop_now = false;
	hw_start_i2c();
	chThdCreateStatic(as5048_thread_wa, sizeof(as5048_thread_wa), NORMALPRIO, as5048_thread, NULL);
#endif
#ifdef USING_SOFTWARE_I2C_ENCODER

	i2cs.sda_gpio = GPIOC;
    i2cs.sda_pin = 11;
    i2cs.scl_gpio = GPIOB;
    i2cs.scl_pin = 7;
    i2c_bb_init(&i2cs);
    chThdCreateStatic(as5048_thread_wa, sizeof(as5048_thread_wa), NORMALPRIO, as5048_thread, NULL);
#endif
#ifdef USING_HARDWARE_SPI_ENCODER

#endif
	//HARDWARE I2C ABOVE THIS LINE

//	palSetPadMode(SPI_SW_MISO_GPIO, SPI_SW_MISO_PIN, PAL_MODE_INPUT);
//	palSetPadMode(SPI_SW_SCK_GPIO, SPI_SW_SCK_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
//	palSetPadMode(SPI_SW_CS_GPIO, SPI_SW_CS_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);

	// Set MOSI to 1
//#if (AS5047_USE_HW_SPI_PINS || AD2S1205_USE_HW_SPI_PINS)
//	palSetPadMode(SPI_SW_MOSI_GPIO, SPI_SW_MOSI_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
//	palSetPad(SPI_SW_MOSI_GPIO, SPI_SW_MOSI_PIN);
//#endif

	// Enable timer clock
//	HW_ENC_TIM_CLK_EN();
//
//	// Time Base configuration
//	TIM_TimeBaseStructure.TIM_Prescaler = 0;
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
//	TIM_TimeBaseStructure.TIM_Period = ((168000000 / 2 / AS5047_SAMPLE_RATE_HZ) - 1);
//	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
//	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
//	TIM_TimeBaseInit(HW_ENC_TIM, &TIM_TimeBaseStructure);
//
//	// Enable overflow interrupt
//	TIM_ITConfig(HW_ENC_TIM, TIM_IT_Update, ENABLE);
//
//	// Enable timer
//	TIM_Cmd(HW_ENC_TIM, ENABLE);
//
//	nvicEnableVector(HW_ENC_TIM_ISR_CH, 6);
//
	mode = ENCODER_MODE_AS5047P_SPI;
	index_found = true;
	spi_error_rate = 0.0;
}

//====================== AS5048 THREAD START ============================================================
static THD_FUNCTION(as5048_thread, arg) {
    (void)arg;

    chRegSetThreadName("AS5048B I2C");
    is_running = true;



#ifdef USING_HARDWARE_I2C_ENCODER
    hw_start_i2c();
    chThdSleepMilliseconds(10);

    for(;;) {
        bool is_ok = true;

        i2cAcquireBus(&HW_I2C_DEV);
        status = i2cMasterTransmitTimeout(&HW_I2C_DEV, as5048_addr, txbuf, 1, rxbuf, 2, tmo);
        i2cReleaseBus(&HW_I2C_DEV);
        angMSB = (rxbuf[0] << 6);
        angLSB = (rxbuf[1] & 0x3F);
        ang_jb = angMSB + angLSB;
        is_ok = status;

        uint32_t pos = ang_jb;
        // Handle encoder rollover up or down.
        if(last_encoder_counts < (16384 / 4) && pos > (3 * 16384 / 4))
        {
            cumulative_encoder_counts -= 16384;
        } else
        if(last_encoder_counts > (3 * 16384 / 4) && pos < (16384 / 4))
        {
            cumulative_encoder_counts += 16384;
        }

      // Now account for actual reading changes
        cumulative_encoder_counts += (pos - last_encoder_counts);
        last_encoder_counts = pos;

        last_enc_angle = ((float)pos * 360.0) / 16384.0;
        spi_val = last_enc_angle;
        else {
            //chuck_error = 2;
            hw_try_restore_i2c();
            chThdSleepMilliseconds(10);
        }
        chThdSleepMicroseconds(1000);
    }
#endif
#ifdef USING_SOFTWARE_I2C_ENCODER
    for(;;){

        bool rec = i2c_bb_tx_rx(&i2cs, as5048_addr, txMSB, 1, rxbuf, 2);

        if(rec){
          angMSB = (rxbuf[0] << 6);
          angLSB = (rxbuf[1] & 0x3F);
          ang_jb = angMSB + angLSB;

          sw_i2c_err = 0;
        }
        else {
            sw_i2c_err = 1;
        }
        uint32_t pos = ang_jb;
        // Handle encoder rollover up or down.
        if(last_encoder_counts < (16384 / 4) && pos > (3 * 16384 / 4))
        {
            cumulative_encoder_counts -= 16384;
        } else
        if(last_encoder_counts > (3 * 16384 / 4) && pos < (16384 / 4))
        {
            cumulative_encoder_counts += 16384;
        }

      // Now account for actual reading changes
        cumulative_encoder_counts += (int32_t)((int32_t)pos - (int32_t)last_encoder_counts);
        last_encoder_counts = pos;

        enc_angle_abs_deg = ((float)cumulative_encoder_counts * 360.0) / 16384.0;
        spi_val = enc_angle_abs_deg;

        enc_angle_deg = ((float)last_encoder_counts * 360.0) / 16384.0;

        chThdSleepMicroseconds(500);
    }
#endif
}
//====================== AS5048 THREAD END ============================================================

void encoder_init_ad2s1205_spi(void) {
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	resolver_loss_of_tracking_error_rate = 0.0;
	resolver_degradation_of_signal_error_rate = 0.0;
	resolver_loss_of_signal_error_rate = 0.0;
	resolver_loss_of_tracking_error_cnt = 0;
	resolver_loss_of_signal_error_cnt = 0;

	palSetPadMode(SPI_SW_MISO_GPIO, SPI_SW_MISO_PIN, PAL_MODE_INPUT);
	palSetPadMode(SPI_SW_SCK_GPIO, SPI_SW_SCK_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
	palSetPadMode(SPI_SW_CS_GPIO, SPI_SW_CS_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);

	// Set MOSI to 1
#if (AS5047_USE_HW_SPI_PINS || AD2S1205_USE_HW_SPI_PINS)
	palSetPadMode(SPI_SW_MOSI_GPIO, SPI_SW_MOSI_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
	palSetPad(SPI_SW_MOSI_GPIO, SPI_SW_MOSI_PIN);
#endif

	// TODO: Choose pins on comm port when these are not defined
#if defined(AD2S1205_SAMPLE_GPIO)
	palSetPadMode(AD2S1205_SAMPLE_GPIO, AD2S1205_SAMPLE_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
	palSetPad(AD2S1205_SAMPLE_GPIO, AD2S1205_SAMPLE_PIN);	// Prepare for a falling edge SAMPLE assertion
#endif
#if defined(AD2S1205_RDVEL_GPIO)
	palSetPadMode(AD2S1205_RDVEL_GPIO, AD2S1205_RDVEL_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
	palSetPad(AD2S1205_RDVEL_GPIO, AD2S1205_RDVEL_PIN);		// Will always read position
#endif


	// Enable timer clock
	HW_ENC_TIM_CLK_EN();

	// Time Base configuration
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = ((168000000 / 2 / AD2S1205_SAMPLE_RATE_HZ) - 1);
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(HW_ENC_TIM, &TIM_TimeBaseStructure);

	// Enable overflow interrupt
	TIM_ITConfig(HW_ENC_TIM, TIM_IT_Update, ENABLE);

	// Enable timer
	TIM_Cmd(HW_ENC_TIM, ENABLE);

	nvicEnableVector(HW_ENC_TIM_ISR_CH, 6);

	mode = RESOLVER_MODE_AD2S1205;
	index_found = true;
}

void encoder_init_sincos(float s_gain, float s_offset,
						 float c_gain, float c_offset, float filter_constant) {
	//ADC inputs are already initialized in hw_init_gpio()
	sin_gain = s_gain;
	sin_offset = s_offset;
	cos_gain = c_gain;
	cos_offset = c_offset;
	sincos_filter_constant = filter_constant;

	sincos_signal_below_min_error_cnt = 0;
	sincos_signal_above_max_error_cnt = 0;
	sincos_signal_low_error_rate = 0.0;
	sincos_signal_above_max_error_rate = 0.0;

	// ADC measurements needs to be in sync with motor PWM
#ifdef HW_HAS_SIN_COS_ENCODER
	mode = ENCODER_MODE_SINCOS;
	index_found = true;
#else
	mode = ENCODER_MODE_NONE;
	index_found = false;
#endif
}

void encoder_init_ts5700n8501(void) {
	mode = ENCODER_MODE_TS5700N8501;
	index_found = true;
	spi_error_rate = 0.0;
	spi_error_cnt = 0;
	ts5700n8501_is_running = true;
	ts5700n8501_stop_now = false;

	chThdCreateStatic(ts5700n8501_thread_wa, sizeof(ts5700n8501_thread_wa),
			NORMALPRIO - 10, ts5700n8501_thread, NULL);
}

bool encoder_is_configured(void) {
	return mode != ENCODER_MODE_NONE;
}

/**
 * Read angle from configured encoder.
 *
 * @return
 * The current encoder angle in degrees.
 */
float encoder_read_deg(void) {
	static float angle = 0.0;

	switch (mode) {
	case ENCODER_MODE_ABI:
		angle = ((float)HW_ENC_TIM->CNT * 360.0) / (float)enc_counts;
		break;

	case ENCODER_MODE_AS5047P_SPI:
	case RESOLVER_MODE_AD2S1205:
	case ENCODER_MODE_TS5700N8501:
		angle = enc_angle_deg;
		break;

#ifdef HW_HAS_SIN_COS_ENCODER
	case ENCODER_MODE_SINCOS: {
		float sin = ENCODER_SIN_VOLTS * sin_gain - sin_offset;
		float cos = ENCODER_COS_VOLTS * cos_gain - cos_offset;

		float module = SQ(sin) + SQ(cos);

		if (module > SQ(SINCOS_MAX_AMPLITUDE) )	{
			// signals vector outside of the valid area. Increase error count and discard measurement
			++sincos_signal_above_max_error_cnt;
			UTILS_LP_FAST(sincos_signal_above_max_error_rate, 1.0, 1./SINCOS_SAMPLE_RATE_HZ);
			angle = last_enc_angle;
		}
		else {
			if (module < SQ(SINCOS_MIN_AMPLITUDE)) {
				++sincos_signal_below_min_error_cnt;
				UTILS_LP_FAST(sincos_signal_low_error_rate, 1.0, 1./SINCOS_SAMPLE_RATE_HZ);
				angle = last_enc_angle;
			}
			else {
				UTILS_LP_FAST(sincos_signal_above_max_error_rate, 0.0, 1./SINCOS_SAMPLE_RATE_HZ);
				UTILS_LP_FAST(sincos_signal_low_error_rate, 0.0, 1./SINCOS_SAMPLE_RATE_HZ);

				float angle_tmp = utils_fast_atan2(sin, cos) * 180.0 / M_PI;
				UTILS_LP_FAST(angle, angle_tmp, sincos_filter_constant);
				last_enc_angle = angle;
			}
		}
		break;
	}
#endif

	default:
		break;
	}

	return angle;
}

/*
 * Note: This is not a good solution and needs a proper implementation later...
 */
float encoder_read_deg_multiturn(void) {
	if (mode == ENCODER_MODE_TS5700N8501) {
		encoder_ts57n8501_get_abm();
		float ts_mt = (float)encoder_ts57n8501_get_abm();
		if (fabsf(ts_mt) > 5000.0) {
			ts_mt = 0;
			encoder_ts57n8501_reset_multiturn();
		}

		ts_mt += 5000;

		return encoder_read_deg() / 10000.0 + (360 * ts_mt) / 10000.0;
	} else {
		return encoder_get_abs_angle();
	}
}

/**
 * Reset the encoder counter. Should be called from the index interrupt.
 */
void encoder_reset(void) {
	// Only reset if the pin is still high to avoid too short pulses, which
	// most likely are noise.
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	if (palReadPad(HW_HALL_ENC_GPIO3, HW_HALL_ENC_PIN3)) {
		const unsigned int cnt = HW_ENC_TIM->CNT;
		static int bad_pulses = 0;
		const unsigned int lim = enc_counts / 20;

		if (index_found) {
			// Some plausibility filtering.
			if (cnt > (enc_counts - lim) || cnt < lim) {
				HW_ENC_TIM->CNT = 0;
				bad_pulses = 0;
			} else {
				bad_pulses++;

				if (bad_pulses > 5) {
					index_found = 0;
				}
			}
		} else {
			HW_ENC_TIM->CNT = 0;
			index_found = true;
			bad_pulses = 0;
		}
	}
}

// returns true for even number of ones (no parity error according to AS5047 datasheet
bool spi_check_parity(uint16_t x) {
	x ^= x >> 8;
	x ^= x >> 4;
	x ^= x >> 2;
	x ^= x >> 1;
	return (~x) & 1;
}

/**
 * Timer interrupt
 */
void encoder_tim_isr(void) {
	uint16_t pos;


	if(mode == ENCODER_MODE_AS5047P_SPI) {

//	  i2cAcquireBus(&HW_I2C_DEV);
//	  txbuf[0] = AS5048B_ANGLMSB_REG;
//	  int status = i2cMasterTransmitTimeout(&HW_I2C_DEV, AS5048_ADDRESS, txbuf, 1, rxbuf, 1, tmo);
//      i2cReleaseBus(&HW_I2C_DEV);
//      uint16_t angMSB = (rxbuf[0] << 6);
//
//
//      i2cAcquireBus(&HW_I2C_DEV);
//      txbuf[0] = AS5048B_ANGLLSB_REG;
//      status = i2cMasterTransmitTimeout(&HW_I2C_DEV, AS5048_ADDRESS, AS5048B_ANGLLSB_REG, 1, rxbuf, 1, tmo);
//      i2cReleaseBus(&HW_I2C_DEV);
//      uint16_t angLSB = (rxbuf[0]);
//
//      uint16_t ang_jb = angMSB | angLSB;
//      spi_val = ang_jb;
//      last_enc_angle = ((float)ang_jb * 360.0) / 16384.0;
//	  spi_val = 11377;
//	  last_enc_angle = 250;

//	    uint16_t txbuf[1];
//	    uint16_t rxbuf[1] = {0};
//        txbuf[0] = 0xFFFF;
//        spiSelect(&SPID1);            /* Slave Select assertion.          */
//        spiExchange(&SPID1, 1, txbuf, rxbuf);          /* Atomic transfer operations.      */
//        spiUnselect(&SPID1);          /* Slave Select de-assertion.       */
//        spi_val = rxbuf[0];
//        rxbuf[0] &= 0x3FFF;
//        last_enc_angle = ((float)rxbuf[0] * 360.0) / 16384.0;
//	  txbuf[0] = 0xFF;
//	  txbuf[1] = 0xFF;
//
//	  spiAcquireBus(&SPID1);              /* Acquire ownership of the bus.    */
//      spiStart(&SPID1, &ls_spicfg);       /* Setup transfer parameters.       */
//      spiSelect(&SPID1);                  /* Slave Select assertion.          */
//      spiExchange(&SPID1, 2, txbuf, rxbuf);          /* Atomic transfer operations.      */
//      spiUnselect(&SPID1);                /* Slave Select de-assertion.       */
//      spiReleaseBus(&SPID1);              /* Ownership release.               */
//      uint16_t ang_jb = (rxbuf[0] <<8) | rxbuf[1];
//      ang_jb &= 0x3FFF;
//      last_enc_angle = ((float)ang_jb * 360.0) / 16384.0;

	    // HARDWARE SPI ABOVE THIS LINE
//		spi_transfer(&pos, 0, 1);
//		spi_end();
//
//		spi_val = pos;
//		if(spi_check_parity(pos) && pos != 0xffff) {  // all ones = disconnect
//			pos &= 0x3FFF;
//			last_enc_angle = ((float)pos * 360.0) / 16384.0;
//			UTILS_LP_FAST(spi_error_rate, 0.0, 1./AS5047_SAMPLE_RATE_HZ);
//		} else {
//			++spi_error_cnt;
//			UTILS_LP_FAST(spi_error_rate, 1.0, 1./AS5047_SAMPLE_RATE_HZ);
//		}
	}

	if(mode == RESOLVER_MODE_AD2S1205) {
		// SAMPLE signal should have been be asserted in sync with ADC sampling
#ifdef AD2S1205_RDVEL_GPIO
		palSetPad(AD2S1205_RDVEL_GPIO, AD2S1205_RDVEL_PIN);	// Always read position
#endif

		palSetPad(SPI_SW_SCK_GPIO, SPI_SW_SCK_PIN);
		spi_delay();
		spi_begin(); // CS uses the same mcu pin as AS5047
		spi_delay();

		spi_transfer(&pos, 0, 1);
		spi_end();

		//spi_val = pos;

		uint16_t RDVEL = pos & 0x08; // 1 means a position read
		bool DOS = ((pos & 0x04) == 0);
		bool LOT = ((pos & 0x02) == 0);
		bool LOS = DOS && LOT;
		bool parity_error = spi_check_parity(pos);	//16 bit frame has odd parity

		if(LOS) {
			LOT = DOS = 0;
		}

		if(!parity_error) {
			UTILS_LP_FAST(spi_error_rate, 0.0, 1./AD2S1205_SAMPLE_RATE_HZ);
		} else {
			++spi_error_cnt;
			UTILS_LP_FAST(spi_error_rate, 1.0, 1./AD2S1205_SAMPLE_RATE_HZ);
		}

		pos &= 0xFFF0;
		pos = pos >> 4;
		pos &= 0x0FFF;

		if(LOT) {
			++resolver_loss_of_tracking_error_cnt;
			UTILS_LP_FAST(resolver_loss_of_tracking_error_rate, 1.0, 1./AD2S1205_SAMPLE_RATE_HZ);
		} else {
			UTILS_LP_FAST(resolver_loss_of_tracking_error_rate, 0.0, 1./AD2S1205_SAMPLE_RATE_HZ);
		}

		if(DOS) {
			++resolver_degradation_of_signal_error_cnt;
			UTILS_LP_FAST(resolver_degradation_of_signal_error_rate, 1.0, 1./AD2S1205_SAMPLE_RATE_HZ);
		} else {
			UTILS_LP_FAST(resolver_degradation_of_signal_error_rate, 0.0, 1./AD2S1205_SAMPLE_RATE_HZ);
		}

		if(LOS) {
			++resolver_loss_of_signal_error_cnt;
			UTILS_LP_FAST(resolver_loss_of_signal_error_rate, 1.0, 1./AD2S1205_SAMPLE_RATE_HZ);
		} else {
			UTILS_LP_FAST(resolver_loss_of_signal_error_rate, 0.0, 1./AD2S1205_SAMPLE_RATE_HZ);
		}

		if((RDVEL != 0) && (LOS != 0) && (DOS != 0) && (LOT != 0) && (!parity_error)) {
			last_enc_angle = ((float)pos * 360.0) / 4096.0;
		}
	}
}

/**
 * Set the number of encoder counts.
 *
 * @param counts
 * The number of encoder counts
 */
void encoder_set_counts(uint32_t counts) {
	if (counts != (uint32_t)enc_counts) {
		enc_counts = counts;
		TIM_SetAutoreload(HW_ENC_TIM, enc_counts - 1);
		index_found = false;
	}
}

/**
 * Check if the index pulse is found.
 *
 * @return
 * True if the index is found, false otherwise.
 */
bool encoder_index_found(void) {
	return index_found;
}

// Software SPI
static void spi_transfer(uint16_t *in_buf, const uint16_t *out_buf, int length) {
	for (int i = 0;i < length;i++) {
		uint16_t send = out_buf ? out_buf[i] : 0xFFFF;
		uint16_t receive = 0;

		for (int bit = 0;bit < 16;bit++) {
			//palWritePad(HW_SPI_PORT_MOSI, HW_SPI_PIN_MOSI, send >> 15);
			send <<= 1;

			palSetPad(SPI_SW_SCK_GPIO, SPI_SW_SCK_PIN);
			spi_delay();

			int samples = 0;
			samples += palReadPad(SPI_SW_MISO_GPIO, SPI_SW_MISO_PIN);
			__NOP();
			samples += palReadPad(SPI_SW_MISO_GPIO, SPI_SW_MISO_PIN);
			__NOP();
			samples += palReadPad(SPI_SW_MISO_GPIO, SPI_SW_MISO_PIN);
			__NOP();
			samples += palReadPad(SPI_SW_MISO_GPIO, SPI_SW_MISO_PIN);
			__NOP();
			samples += palReadPad(SPI_SW_MISO_GPIO, SPI_SW_MISO_PIN);

			receive <<= 1;
			if (samples > 2) {
				receive |= 1;
			}

			palClearPad(SPI_SW_SCK_GPIO, SPI_SW_SCK_PIN);
			spi_delay();
		}

		if (in_buf) {
			in_buf[i] = receive;
		}
	}
}

static void spi_begin(void) {
	palClearPad(SPI_SW_CS_GPIO, SPI_SW_CS_PIN);
}

static void spi_end(void) {
	palSetPad(SPI_SW_CS_GPIO, SPI_SW_CS_PIN);
}

static void spi_delay(void) {
	__NOP();
	__NOP();
	__NOP();
	__NOP();
}

#pragma GCC push_options
#pragma GCC optimize ("O0")

void TS5700N8501_delay_uart(void) {
	__NOP(); __NOP(); __NOP();
	__NOP(); __NOP(); __NOP();
	__NOP(); __NOP(); __NOP();
	__NOP(); __NOP(); __NOP();
	__NOP(); __NOP(); __NOP();
	__NOP(); __NOP();
}

/*
 * It is important to switch to receive mode immediately after sending the readout command,
 * as the TS5700N8501 starts sending the reply after 3 microseconds. Therefore use software
 * UART on TX so that the enable signal can be controlled manually. This function runs while
 * the system is locked, but it should finish fast enough to not cause problems for other
 * things due to the high baud rate.
 */
static void TS5700N8501_send_byte(uint8_t b) {
	utils_sys_lock_cnt();
#ifdef HW_ADC_EXT_GPIO
	palSetPad(HW_ADC_EXT_GPIO, HW_ADC_EXT_PIN);
#endif
	TS5700N8501_delay_uart();
	palWritePad(HW_UART_TX_PORT, HW_UART_TX_PIN, 0);
	__NOP(); __NOP(); __NOP();
	__NOP(); __NOP(); __NOP();
	__NOP(); __NOP(); __NOP();
	__NOP(); __NOP(); __NOP();
	__NOP(); __NOP(); __NOP();
	for (int i = 0;i < 8;i++) {
		palWritePad(HW_UART_TX_PORT, HW_UART_TX_PIN,
				(b & (0x80 >> i)) ? PAL_HIGH : PAL_LOW);
		TS5700N8501_delay_uart();
	}
	__NOP(); __NOP(); __NOP();
	__NOP(); __NOP(); __NOP();
	__NOP(); __NOP(); __NOP();
	__NOP(); __NOP(); __NOP();
	__NOP(); __NOP(); __NOP();
	__NOP(); __NOP(); __NOP();
	palWritePad(HW_UART_TX_PORT, HW_UART_TX_PIN, 1);
	TS5700N8501_delay_uart();
#ifdef HW_ADC_EXT_GPIO
	palClearPad(HW_ADC_EXT_GPIO, HW_ADC_EXT_PIN);
#endif
	utils_sys_unlock_cnt();
}

#pragma GCC pop_options

static THD_FUNCTION(ts5700n8501_thread, arg) {
	(void)arg;

	chRegSetThreadName("TS5700N8501");

	sdStart(&HW_UART_DEV, &TS5700N8501_uart_cfg);
	palSetPadMode(HW_UART_TX_PORT, HW_UART_TX_PIN, PAL_MODE_OUTPUT_PUSHPULL |
			PAL_STM32_OSPEED_HIGHEST |
			PAL_STM32_PUDR_PULLUP);
	palSetPadMode(HW_UART_RX_PORT, HW_UART_RX_PIN, PAL_MODE_ALTERNATE(HW_UART_GPIO_AF) |
			PAL_STM32_OSPEED_HIGHEST |
			PAL_STM32_PUDR_PULLUP);
#ifdef HW_ADC_EXT_GPIO
	palSetPadMode(HW_ADC_EXT_GPIO, HW_ADC_EXT_PIN, PAL_MODE_OUTPUT_PUSHPULL |
			PAL_STM32_OSPEED_HIGHEST |
			PAL_STM32_PUDR_PULLUP);
#endif

	for(;;) {
		// Check if it is time to stop.
		if (ts5700n8501_stop_now) {
			ts5700n8501_is_running = false;
			return;
		}

		if (ts5700n8501_reset_errors) {
			for (int i = 0;i < 20;i++) {
				TS5700N8501_send_byte(0b01011101);
				chThdSleep(2);
			}

			ts5700n8501_reset_errors = false;
		}

		if (ts5700n8501_reset_multiturn) {
			for (int i = 0;i < 20;i++) {
				TS5700N8501_send_byte(0b01000110);
				chThdSleep(2);
			}

			ts5700n8501_reset_multiturn = false;
		}

		TS5700N8501_send_byte(0b01011000);

		chThdSleep(2);

		uint8_t reply[11];
		int reply_ind = 0;

		msg_t res = sdGetTimeout(&HW_UART_DEV, TIME_IMMEDIATE);
		while (res != MSG_TIMEOUT) {
			if (reply_ind < (int)sizeof(reply)) {
				reply[reply_ind++] = res;
			}
			res = sdGetTimeout(&HW_UART_DEV, TIME_IMMEDIATE);
		}

		uint8_t crc = 0;
		for (int i = 0;i < (reply_ind - 1);i++) {
			crc = (reply[i] ^ crc);
		}

		if (reply_ind == 11 && crc == reply[reply_ind - 1]) {
			uint32_t pos = (uint32_t)reply[2] + ((uint32_t)reply[3] << 8) + ((uint32_t)reply[4] << 16);
			spi_val = pos;
			last_enc_angle = (float)pos / 131072.0 * 360.0;
			UTILS_LP_FAST(spi_error_rate, 0.0, 1.0 / AS5047_SAMPLE_RATE_HZ);

			ts5700n8501_raw_status[0] = reply[1]; // SF
			ts5700n8501_raw_status[1] = reply[2]; // ABS0
			ts5700n8501_raw_status[2] = reply[3]; // ABS1
			ts5700n8501_raw_status[3] = reply[4]; // ABS2
			ts5700n8501_raw_status[4] = reply[6]; // ABM0
			ts5700n8501_raw_status[5] = reply[7]; // ABM1
			ts5700n8501_raw_status[6] = reply[8]; // ABM2
			ts5700n8501_raw_status[7] = reply[9]; // ALMC
		} else {
			++spi_error_cnt;
			UTILS_LP_FAST(spi_error_rate, 1.0, 1.0 / AS5047_SAMPLE_RATE_HZ);
		}
	}
}

