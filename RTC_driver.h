/*
 * RTC_driver.h
 *
 *  Created on: Feb 2018
 *      Author: joe and lourw
 */

#ifndef RTC_DRIVER_H_
#define RTC_DRIVER_H_

#include "driverlib.h"


void RTC_init(void);

void RTC_initialSetUart(void);
void RTC_initialSetHardcoded(void);

RTC_C_Calendar RTC_read(void);


#endif /* RTC_DRIVER_H_ */
