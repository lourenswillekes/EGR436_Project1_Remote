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

#include "ADC_driver.h"
#include "BME280_driver.h"          // eusci_b1
#include "clockConfig.h"
#include "nRF24_driver.h"           // eusci_b2
#include "RTC_driver.h"
#include "SD_driver.h"              // eusci_b0
#include "UART_Init.h"              // eusci_a0


// this function associates a light level with the adc value
int getLightEnumFromADC(int val);

// updated by the rtc
volatile int second_count;


int main(void)
{
    int res;

    // data used by the environmental sensor
    struct bme280_dev dev;
    struct bme280_data compensated_data;
    float normal_humidity = 0;
    float normal_pressure = 0;
    float normal_temperature = 0;

    // data used by the photocell
    int light_level;
    int normal_light = 3;

    // data used to write to the sd card
    RTC_C_Calendar current_time;
    char sd_data[40];

    // data used by the fatfs
    FATFS g_sFatFs;
    FIL g_sFileObject;

    // data used by the rf
    char rf_data[32];



    /* Halting the Watchdog  */
    MAP_WDT_A_holdTimer();

    // set mclk to 3MHz, smclk to 3MHz
    clockStartUp();

    /* Enabling the FPU for floating point operation */
    MAP_FPU_enableModule();
    MAP_FPU_enableLazyStacking();

    // red led initialization
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);

    // initiate eusci_a0 to 9600
    UART_init();

    // initiate the adc module
    ADC_init();

    // setup the rtc
    RTC_init();
    RTC_initialSetHardcoded();

    // initialize the environment sensor
    BME280_init(&dev);

    // fatfs driver wants 10 ms tick (100hz)
    MAP_SysTick_setPeriod(30000);
    MAP_SysTick_enableModule();

    // initialize the fatfs driver
    SD_init(&g_sFatFs, &g_sFileObject);

    // initiate rf transceiver
    nRF24_init();

    // enable interrupts
    MAP_SysTick_enableInterrupt();
    MAP_Interrupt_enableInterrupt(INT_RTC_C);
    MAP_Interrupt_enableMaster();



    while(1)
    {

        if(second_count >= 15){

            // reset second_count
            second_count = 0;

            // red led on
            MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);

            // read the rtc and sensors
            current_time = RTC_read();
            res = BME280_read(&dev, &compensated_data);
            light_level = ADC_read();

            // format the sensor data properly
            normal_humidity = compensated_data.humidity / 1000;
            normal_pressure = (compensated_data.pressure / 13332.237) + 17;
            normal_temperature = compensated_data.temperature * 0.018 + 32;
            normal_light = getLightEnumFromADC(light_level);

            // format data for packet sending and sd write
            sprintf(rf_data, "%2.1f,%3.1f,%2.1f,%05d", normal_humidity,
                normal_pressure, normal_temperature, normal_light);
            sprintf(sd_data, "%04d,%02d,%02d,%02d,%02d,%2.1f,%3.1f,%2.1f,%05d\n",
                current_time.year, current_time.month, current_time.dayOfmonth,
                current_time.hours, current_time.minutes, normal_humidity,
                normal_pressure, normal_temperature, light_level);

            // send and write the data
            nRF24_send(32, (uint8_t *)rf_data);
            SD_append(sd_data, &g_sFatFs, &g_sFileObject);

            // debugging statements
            printf(rf_data); printf("\n\r");
            printf(sd_data); printf("\r");

            // red led off
            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);

        }

    }
}



void RTC_C_IRQHandler(void)
{
    uint32_t status;

    // get status and clear interrupt flag
    status = MAP_RTC_C_getEnabledInterruptStatus();
    MAP_RTC_C_clearInterruptFlag(status);

    // increment counter to keep time
    if (status & RTC_C_CLOCK_READ_READY_INTERRUPT)
    {
        second_count++;
    }

}

void SysTick_ISR(void) {
    // this function should be called every 10ms (100Hz) for the fatfs
    disk_timerproc();
}

void PORT5_ISR(void)
{
    uint32_t status;

    // get status and clear interrupt flag
    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P5);
    P5IFG = 0;

    // set rf_irq with interrupt value
    if (status & BIT0)
    {
        rf_irq |= RF24_IRQ_FLAGGED;
    }
}


int getLightEnumFromADC(int val)
{
    int ret = 0;

    if (13000 < val)
    {
        ret = 4;            // sunny
    } else if (11000 < val)
    {
        ret = 3;            // partly sunny
    } else if (8000 < val)
    {
        ret = 2;            // overcast
    } else if (4000 < val)
    {
        ret = 1;            // twilight
    } // else it will still be 0 (dark)

    return ret;

}
