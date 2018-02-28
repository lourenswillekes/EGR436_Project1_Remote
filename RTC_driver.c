/*
 * RTC_driver.c
 *
 *  Created on: Feb 2018
 *      Author: joe and lourw
 */

#include "RTC_driver.h"

RTC_C_Calendar currentTime =
{
     1,
     3,
     10,
     12,
     11,
     00,
     2018
};


void RTC_init(void){

    /* Configuring pins for peripheral/crystal usage */
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ,
            GPIO_PIN0 | GPIO_PIN1, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Starting LFXT in non-bypass mode without a timeout. */
    CS_startLFXT(false);

    /* Specify an interrupt to assert every minute */
    MAP_RTC_C_setCalendarEvent(RTC_C_CALENDAREVENT_MINUTECHANGE);

    /* Enable interrupt for RTC Ready Status, which asserts when the RTC
     * Calendar registers are ready to read.
     * Also, enable interrupts for the Calendar alarm and Calendar event. */
    MAP_RTC_C_clearInterruptFlag(
            RTC_C_CLOCK_READ_READY_INTERRUPT | RTC_C_TIME_EVENT_INTERRUPT);
    MAP_RTC_C_enableInterrupt(
            RTC_C_CLOCK_READ_READY_INTERRUPT | RTC_C_TIME_EVENT_INTERRUPT);

}

void RTC_initialSetHardcoded(void)
{
    MAP_RTC_C_initCalendar(&currentTime, RTC_C_FORMAT_BINARY);
    MAP_RTC_C_startClock();
}

RTC_C_Calendar RTC_read(void)
{
    RTC_C_Calendar time;

    time = MAP_RTC_C_getCalendarTime();

    return time;
}
