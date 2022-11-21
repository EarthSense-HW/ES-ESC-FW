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

long last_pressed = 0;
int estop_button_state = PAL_LOW;
bool estop_fault_state = FALSE;

static THD_WORKING_AREA(estop_thread_wa, 256);
static THD_FUNCTION(estop_thread, arg) {
	(void)arg;

    while(1) {
        // Read ESTOP button state
        estop_button_state = palReadPad(ESTOP_INPUT_PORT, ESTOP_INPUT_PAD);

        // 50 ms time buffer
        if (chVTGetSystemTime() - last_pressed > ESTOP_DEBOUNCE_DELAY) {
            // Condition: ESTOP button pressed & no faults currently 
            // Outcome: Trigger ESTOP fault
            if (estop_button_state == PAL_HIGH && estop_fault_state == FALSE) {
                mc_interface_fault_stop(FAULT_CODE_ESTOP, FALSE, FALSE);
                estop_fault_state = TRUE;
                last_pressed = chVTGetSystemTime();
            }
            // Condition: ESTOP button released & estop fault active
            // Outcome: Remove ESTOP fault and set to FAULT_CODE_NONE
            else if (estop_button_state == PAL_LOW && estop_fault_state == TRUE) {
                mc_interface_set_fault(FAULT_CODE_NONE);
                estop_fault_state = FALSE;
                last_pressed = chVTGetSystemTime();
            }
        }

        // Check ESTOP_FAULT to stop motors input
        if (mc_interface_get_fault() == FAULT_CODE_ESTOP) {
            mc_interface_ignore_input(150);
        }

        chThdSleepMilliseconds(10);
    }
}

void estop_init(void) {
    // Create Estop Thread
    chThdCreateStatic(estop_thread_wa, sizeof(estop_thread_wa), NORMALPRIO, estop_thread, NULL);
}
#endif
