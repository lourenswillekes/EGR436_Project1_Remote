/*
 * timer_32.c
 *
 *  Created on: Feb 2018
 *      Author: lourw
 */

#include "timer_32.h"


// Setup Timer32_0 in Freerun mode
void timer32_Init(void) {

    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_16,
                           TIMER32_32BIT, TIMER32_FREE_RUN_MODE);

}

void timer32_Wait_ms(uint32_t ms) {

    // 187.5 clock cycles per ms
    int count = ms * 187.5;

    // set the count and start the timer
    MAP_Timer32_setCount(TIMER32_0_BASE, count);
    MAP_Timer32_startTimer(TIMER32_0_BASE, true);

    // wait until counter hits 0
    while (0 < MAP_Timer32_getValue(TIMER32_0_BASE));
}
