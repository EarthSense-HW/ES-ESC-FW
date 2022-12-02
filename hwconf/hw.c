/*
	Copyright 2019 Benjamin Vedder	benjamin@vedder.se

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

#include "conf_general.h"
#include "utils.h"
#include <math.h>
#include HW_SOURCE

uint8_t hw_id_from_uuid(void) {
	uint8_t id = utils_crc32c(STM32_UUID_8, 12) & 0x7F;
	// CAN ID 10 and 11 are often used by DieBieMS / FlexiBMS
	uint8_t reserved[] = {10, 11};
	for (size_t i = 0; i < sizeof(reserved); ++i) {
		if (id == reserved[i]) {
			id = (id + 1) & 0x7F;
			i = 0;
		}
	}
	return id;
}

#if defined(HW_ID_PIN_GPIOS) && defined(HW_ID_PIN_PINS)
uint8_t hw_id_from_pins(void) {
	stm32_gpio_t *hw_id_ports[]={HW_ID_PIN_GPIOS};
	const uint16_t hw_id_pins[] = {HW_ID_PIN_PINS};
	const uint16_t hw_id_pins_size = sizeof(hw_id_pins)/sizeof(uint16_t);

	const uint16_t DELAY_MS = 5;
	uint8_t trits[hw_id_pins_size];
	uint8_t position = 0u; //Start at position 0
	uint8_t id = 0u; //Return VESC_ID
	for (uint8_t i=0; i < hw_id_pins_size; i++) {
		//Initialize pulldown
		palSetPadMode(hw_id_ports[i], hw_id_pins[i], PAL_MODE_INPUT_PULLDOWN);
		
		//Delay a little for the resistor to take affect
		chThdSleepMilliseconds(DELAY_MS);
		bool pin_set_pulldown = (palReadPad(hw_id_ports[i], hw_id_pins[i]));
		//Initialize pullup
		palSetPadMode(hw_id_ports[i], hw_id_pins[i], PAL_MODE_INPUT_PULLUP);
		//Delay a little for the resistor to take affect
		chThdSleepMilliseconds(DELAY_MS);
		bool pin_set_pullup = (palReadPad(hw_id_ports[i], hw_id_pins[i]));
		//Now determine the trit state
		if (!pin_set_pulldown && !pin_set_pullup) {
			//Tied to GND
			trits[i] = 0u;
		} else if (pin_set_pulldown && pin_set_pullup) {
			//Tied to VCC
			trits[i] = 1u;
		} else if (!pin_set_pulldown && pin_set_pullup) {
			//Floating
			trits[i] = 2u;
		} else {
			return hw_id_from_uuid();
			//To satisfy compiler warning
			trits[i] = 3u;
		}
		position += trits[i] * pow(2, i); // Calculate ID : Both 
		palSetPadMode(hw_id_ports[i], hw_id_pins[i], PAL_MODE_INPUT);
	}

	/* Position Map for Earthsense 2020 System Board V1.2
	 * Position     VESC_ID
	 * 	  0	           2
	 *    1            0      
	 *    2            1
	 *    3            3
	 * 
	 * Diagram:
	 * -------------------------	      -----------------\
	 * |   POS_2   |   POS_3   |__________|			GPM		|
	 * \   VESC_1  |   VESC_3                 GPS           |
	 *  | ---------|---------   MOS     IMU             PI  |
	 * /	POS_1  |   POS_0    __________                  |
	 * |   VESC_0  |   VESC_2  |	      |   VRM           | 
	 * -------------------------          -----------------/
	*/
	switch (position) {
	case 0:
		id = POS_0_VESC_ID;
		break;
	case 1:
		id = POS_1_VESC_ID;
		break;
	case 2:
		id = POS_2_VESC_ID;
		break;
	case 3:
		id = POS_3_VESC_ID;
		break;
	default:
		id = POS_INVALID_ID;
		break;
	}
	return id;
}
#endif //defined(HW_ID_PIN_GPIOS) && defined(HW_ID_PIN_PINS)
