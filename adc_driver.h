/*
 * adc.h
 *
 *  Created on: Feb 2018
 *      Author: lourw and joe
 */

#ifndef ADC_DRIVER_H_
#define ADC_DRIVER_H_

#include "driverlib.h"


void adc_Init(void);
uint16_t adc_Read(void);


#endif /* ADC_DRIVER_H_ */
