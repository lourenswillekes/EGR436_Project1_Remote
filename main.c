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

#include "clockConfig.h"
#include "environment_sensor.h"     // eusci_b1
#include "fatfs/src/diskio.h"
#include "fatfs/src/ff.h"
#include "RTC_Module.h"
#include "spiDriver.h"              // eusci_b0
#include "UART_Init.h"              // eusci_a0



volatile int second_count;
volatile int reset_time = 0;

/* currently out of main so that viewing this during debugging is easy */
// data used by the environmental sensor
struct bme280_dev dev;
struct bme280_data compensated_data;
// data used to write to the sd card
RTC_C_Calendar current_time;
char data_line[45];



int main(void)
{
    int res;

    // data used by the fatfs
    FATFS g_sFatFs;
    FIL g_sFileObject;
    FRESULT iFResult;
    UINT writeBytes = 0;


    /* Halting the Watchdog  */
    MAP_WDT_A_holdTimer();

    // set mclk to 48MHz, smclk to 3MHz
    clockStartUp();
    //MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_3);
    //MAP_CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    //MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    /* Enabling the FPU for floating point operation */
    MAP_FPU_enableModule();
    MAP_FPU_enableLazyStacking();

    // initiate eusci_a0 to 9600
    UART_init();

    // setup the rtc
    RTC_Config();
    RTC_Initial_Set_Hardcoded();

    // initialize the environment sensor
    BME280_Init(&dev);

    // fatfs driver wants 10 ms tick (100hz)
    MAP_SysTick_setPeriod(30000);
    MAP_SysTick_enableModule();


    // initialize the fatfs driver
    spi_Open();
    // mount the file system using logical disk 0
    iFResult = f_mount(0, &g_sFatFs);
    // open a new log file in the fileobject
    iFResult = f_open(&g_sFileObject, "log.txt", FA_WRITE | FA_CREATE_ALWAYS);
    // write a the header line to the log file
    iFResult = f_write(&g_sFileObject, "Year Mo Da Ho Mi Tempe Humid Pressure Light\r\n", 45, &writeBytes);
    // close the file again
    iFResult = f_close(&g_sFileObject);


    MAP_SysTick_enableInterrupt();
    MAP_Interrupt_enableInterrupt(INT_RTC_C);
    MAP_Interrupt_enableMaster();




    while(1)
    {

        if(second_count >= 5){

            second_count = 0;

            current_time = MAP_RTC_C_getCalendarTime();
            res = BME280_Read(&dev, &compensated_data);
            sprintf(data_line, "%04d,%02d,%02d,%02d,%02d,%05d,%05d,%08d,%05d\r\n",
                current_time.year, current_time.month, current_time.dayOfmonth,
                current_time.hours, current_time.minutes, compensated_data.temperature,
                compensated_data.humidity, compensated_data.pressure, 16383);
            printf(data_line);

            // Open a new file in the fileobject
            iFResult = f_open(&g_sFileObject, "log.txt", FA_WRITE);
            // move pointer to the end of the file
            iFResult = f_lseek(&g_sFileObject, g_sFileObject.fsize);
            // write a data string to the sd card
            iFResult = f_write(&g_sFileObject, data_line, 45, &writeBytes);
            // close the file
            iFResult = f_close(&g_sFileObject);

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
