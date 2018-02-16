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


struct bme280_dev dev;
struct bme280_data compensated_data;


int main(void)
{
    int res;

    /* Halting the Watchdog  */
    MAP_WDT_A_holdTimer();

    clockStartUp();

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

