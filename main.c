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



int main(void)
{
    /* Halting the Watchdog  */
    MAP_WDT_A_holdTimer();

    clockStartUp();

    /* Enabling the FPU for floating point operation */
    MAP_FPU_enableModule();
    MAP_FPU_enableLazyStacking();

    /* Configuring SysTick */
    MAP_SysTick_enableModule();
    MAP_SysTick_setPeriod(12000000);

    MAP_SysTick_enableInterrupt();
    MAP_Interrupt_enableMaster();

    while(1)
    {

    }
}
void SysTick_Handler(void)
{

}
