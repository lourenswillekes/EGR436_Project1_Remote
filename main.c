/*
 * main.c
 * Entry point for the remote station
 *
 *  Created on: Feb 2018
 *      Author: lourw and joe
 */

#include "driverlib.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "adc_driver.h"
#include "clockConfig.h"
#include "environment_sensor.h"     // eusci_b1
#include "RTC_Module.h"
#include "sd_driver.h"
#include "spiDriver.h"              // eusci_b0
#include "UART_Init.h"              // eusci_a0


volatile int second_count;
volatile int reset_time = 0;
volatile int light_level = 8192;


int main(void)
{
    int res;

    // data used by the environmental sensor
    struct bme280_dev dev;
    struct bme280_data compensated_data;

    // data used to write to the sd card
    RTC_C_Calendar current_time;
    char data_line[45];

    // data used by the fatfs
    FATFS g_sFatFs;
    FIL g_sFileObject;

    /* Halting the Watchdog  */
    MAP_WDT_A_holdTimer();

    // set mclk to 3MHz, smclk to 3MHz
    clockStartUp();

    /* Enabling the FPU for floating point operation */
    MAP_FPU_enableModule();
    MAP_FPU_enableLazyStacking();

    // initiate eusci_a0 to 9600
    UART_init();

    // initiate the adc module
    adc_Init();

    // setup the rtc
    RTC_Config();
    RTC_Initial_Set_Hardcoded();

    // initialize the environment sensor
    BME280_Init(&dev);

    // fatfs driver wants 10 ms tick (100hz)
    MAP_SysTick_setPeriod(30000);
    MAP_SysTick_enableModule();

    // open spi to the sd and initialize the fatfs driver
    spi_Open();
    res = sd_Init(&g_sFatFs, &g_sFileObject);

    // enable interrupts
    MAP_SysTick_enableInterrupt();
    MAP_Interrupt_enableInterrupt(INT_RTC_C);
    MAP_Interrupt_enableMaster();


    while(1)
    {

        if(second_count >= 5){

            second_count = 0;

            current_time = MAP_RTC_C_getCalendarTime();
            res = BME280_Read(&dev, &compensated_data);
            light_level = adc_Read();

            sprintf(data_line, "%04d,%02d,%02d,%02d,%02d,%05d,%05d,%08d,%05d\r\n",
                current_time.year, current_time.month, current_time.dayOfmonth,
                current_time.hours, current_time.minutes, compensated_data.temperature,
                compensated_data.humidity, compensated_data.pressure, light_level);
            res = sd_Append(data_line, &g_sFatFs, &g_sFileObject);
            printf(data_line);

        }

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


void SysTick_ISR(void) {
    // this function should be called every 10ms (100Hz) for the fatfs
    disk_timerproc();
}
