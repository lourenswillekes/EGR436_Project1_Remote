/*
 * adc.c
 *
 *  Created on: Feb 2018
 *      Author: lourw and joe
 */

#include "adc_driver.h"


void adc_Init(void)
{
    // initializing adc clk = 3MHz
    MAP_ADC14_enableModule();
    MAP_ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1,
                ADC_DIVIDER_1, 0);

    // configure gpio 4.7 for analog input
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4,
                GPIO_PIN7, GPIO_TERTIARY_MODULE_FUNCTION);

    // configure adc memory adc_mem6 for single sample, at 3.3V
    MAP_ADC14_configureSingleSampleMode(ADC_MEM6, true);
    MAP_ADC14_configureConversionMemory(ADC_MEM6, ADC_VREFPOS_AVCC_VREFNEG_VSS,
                ADC_INPUT_A6, false);
    MAP_ADC14_enableSampleTimer(ADC_MANUAL_ITERATION);

    // enable the interrupt on adc 6
    MAP_ADC14_enableInterrupt(ADC_INT6);
    MAP_ADC14_enableConversion();

}

uint16_t adc_Read(void)
{
    uint16_t res;

    // start a conversion
    MAP_ADC14_toggleConversionTrigger();
    // wait until it completes
    while (0 == (ADC14->IFGR0 & ADC_INT6));
    // and get the result
    res = MAP_ADC14_getResult(ADC_MEM6);

    return res;
}
