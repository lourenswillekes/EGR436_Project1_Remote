/*
 * clockConfig.c
 *
 * **NOTE: This file will only work with the remote station because
 *         mclk and smclk are set to 3MHz because of the fatfs. The
 *         home station has mclk at 48Mhz and smclk at 12Mhz.
 *
 *  Created on: Feb 2018
 *      Author: joe and lourw
 */

#include <stdio.h>

#include "clockConfig.h"
#include "driverlib.h"

volatile int MCLKfreq, SMCLKfreq;

// this function divides hfxt to necessary frequencies
void clockStartUp(void){

    clockInit48MHzXTL();  // set up the clock to use the crystal oscillator
    // divide the clock down to 3MHz because of the fatfs
    MAP_CS_initClockSignal(CS_MCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_16);
    MAP_CS_initClockSignal(CS_SMCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_16);
    MCLKfreq = MAP_CS_getMCLK();
    SMCLKfreq = MAP_CS_getSMCLK();

    if (3000000 != MCLKfreq)
    { // crystal oscillator not responding so we try this
        MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_3);
        MAP_CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
        MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
        MCLKfreq = MAP_CS_getMCLK();
        SMCLKfreq = MAP_CS_getSMCLK();
    }

}

// sets the clock module to use the external 48 MHz crystal
void clockInit48MHzXTL(void) {

    /* Configuring pins for peripheral/crystal usage */
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ,
                        GPIO_PIN3 | GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);

    // enables getMCLK, getSMCLK to know externally set frequencies
    MAP_CS_setExternalClockSourceFrequency(32000,48000000);

    /* Starting HFXT in non-bypass mode without a timeout. Before we start
     * we have to change VCORE to 1 to support the 48MHz frequency */
    MAP_PCM_setCoreVoltageLevel(PCM_VCORE1);
    MAP_FlashCtl_setWaitState(FLASH_BANK0, 2);
    MAP_FlashCtl_setWaitState(FLASH_BANK1, 2);
    // false means no timeouts set, will return when stable
    MAP_CS_startHFXT(false);

}

int getMCLKfreq(void){
    return MCLKfreq;
}

int getSMCLKfreq(void){
    return SMCLKfreq;
}
