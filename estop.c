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

#include "estop.h"

#if ESTOP_ENABLE
// Threads
static THD_WORKING_AREA(estop_thread_wa, 256);
thread_t* tt_estop_thread;
event_source_t estop_event_source;

static THD_FUNCTION(estop_thread, arg) {
    (void)arg;
    chRegSetThreadName("ESTOP Triggered");

    event_listener_t estop_listener;

    // Registering as event 0
    chEvtRegisterMask(&estop_event_source, &estop_listener, EVT_ESTOP);

    for (;;) {
        // TEST CODE: Blink LED
        eventmask_t evt = chEvtWaitAny(ALL_EVENTS);

        if (evt & EVT_ESTOP) {
            ledpwm_set_intensity(LED_RED, 0.5);
            chThdSleepMilliseconds(100);
            LED_RED_ON();
            chThdSleepMilliseconds(75);
            LED_RED_OFF();
        }
    }
}
#endif

void estop_init(void) {
	EXTI_InitTypeDef   EXTI_InitStructure;

    // Connect EXTI Line to pin
	// Set ESTOP EXTI interrupt to highest priority
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource9);
	EXTI_InitStructure.EXTI_Line = EXTI_Line9;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
    nvicEnableVector(EXTI9_5_IRQn, 0);

    tt_estop_thread = chThdCreateStatic(estop_thread_wa, sizeof(estop_thread_wa), ABSPRIO - 1, estop_thread, NULL);

    estop_init_done = true;
}

CH_IRQ_HANDLER(EXTI9_5_IRQHandler) {
    eventmask_t events = 0;

    // Wakes up ESTOP thread
    if (EXTI_GetITStatus(EXTI_Line9) != RESET) {
        // Set ESTOP event
        events |= EVT_ESTOP;

        // Signal events to ESTOP thread
        if (events) {
            chSysLockFromISR();
            chEvtSignalI(tt_estop_thread, events);
            chSysUnlockFromISR();
        }

        // Clear the ESTOP interrupt pending bit
        EXTI_ClearITPendingBit(EXTI_Line9);
    }
}
