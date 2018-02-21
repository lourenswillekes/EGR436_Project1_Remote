/*
 * ADC.h
 *
 *  Created on: Feb 2018
 *      Author: lourw
 */

#include "driverlib.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "clockConfig.h"
#include "environment_sensor.h"

#include "RTC_Module.h"
#include "UART_Init.h"


struct bme280_dev dev;
struct bme280_data compensated_data;

int second_count;
int reset_time = 0;


int main(void)
{
    int res;

    /* Halting the Watchdog  */
    MAP_WDT_A_holdTimer();

    clockStartUp();

    UART_init();

    RTC_Config();

    RTC_Initial_Set();

    BME280_Init(&dev);

    /* Enabling the FPU for floating point operation */
    MAP_FPU_enableModule();
    MAP_FPU_enableLazyStacking();

    MAP_Interrupt_enableMaster();

    while(1)
    {
        // wait for the sensor to complete a measurement
        timer32_Wait_ms(50);
        // and get the data
        res = BME280_Read(&dev, &compensated_data);
    }
}

/* RTC ISR */
void RTC_C_IRQHandler(void)
{
    uint32_t status;

    status = MAP_RTC_C_getEnabledInterruptStatus();
    MAP_RTC_C_clearInterruptFlag(status);

    if (status & RTC_C_CLOCK_READ_READY_INTERRUPT)
    {
        second_count++;
    }
    if (status & RTC_C_TIME_EVENT_INTERRUPT)
    {
        /* Interrupts every minute - Set breakpoint here */
        reset_time = 1;
    }
}
