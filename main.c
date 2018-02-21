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

/* currently out of main so that viewing this during debgging is easy */
// data used by the environmental sensor
struct bme280_dev dev;
struct bme280_data compensated_data;


int main(void)
{
    int res;
    char data[5];





    // data used by the fatfs
    char g_pcTmpBuf[80];
    FATFS g_sFatFs;
    FIL g_sFileObject;
    FRESULT iFResult;
    UINT readBytes = 0;
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

    MAP_SysTick_enableInterrupt();
    MAP_Interrupt_enableInterrupt(INT_RTC_C);
    MAP_Interrupt_enableMaster();





    // Print message to user.
    printf("\n\nSD Card Test Program\n\r");
    // Mount the file system, using logical disk 0.
    iFResult = f_mount(0, &g_sFatFs);
    // Open a new file in the fileobject
    iFResult = f_open(&g_sFileObject, "log.txt", FA_WRITE | FA_CREATE_ALWAYS);
    // write a 16 byte string to the sd card
    iFResult = f_write(&g_sFileObject, "This is a test\r\n", 16, &writeBytes);
    // print the number of characters written and close the file
    printf("%d characters written\n\r", writeBytes);
    iFResult = f_close(&g_sFileObject);
    // open the file
    iFResult = f_open(&g_sFileObject, "newFile1", FA_READ);
    // read a string from the file on the sd card
    iFResult = f_read(&g_sFileObject, g_pcTmpBuf, sizeof(g_pcTmpBuf) - 1, &readBytes);
    // print the number of characters read and the line read
    printf("%d characters read\n\r",readBytes);
    printf("%s\n\r", g_pcTmpBuf);
    // close the file again
    iFResult = f_close(&g_sFileObject);






    printf("\nTime Set\n\r");
    char out[20];
    RTC_C_Calendar time = MAP_RTC_C_getCalendarTime();
    sprintf(out,"%02.0d:%02.0d:%02.0d    %02.0d/%02.0d/%02.0d",
                time.hours,
                time.minutes,
                time.seconds,
                time.month,
                time.dayOfmonth,
                time.year);
    printf("Current Time: %s\n\r",out);
    getRTCtime(data);
    printf("\n\rTime: %s",data);

    while(1)
    {

        if(second_count >= 30){
        //if(second_count >= 500){
            second_count = 0;
            getRTCtime(data);
            res = BME280_Read(&dev, &compensated_data);
            printf("\n\rTime: %s",data);
            printf("\n\r\tTemp: %d",compensated_data.temperature);
            printf("\n\r\tHumidity: %d",compensated_data.humidity);
            printf("\n\r\tPressure: %d",compensated_data.pressure);
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
