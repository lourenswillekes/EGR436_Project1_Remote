/*
 * rf_driver.h
 *
 *  Created on: Feb 2018
 *      Author: lourw
 */

#ifndef RF_DRIVER_H_
#define RF_DRIVER_H_

#include "driverlib.h"
#include "msprf24.h"
#include "nrf_userconfig.h"


void rf_Init(void);
int rf_Send(uint8_t len, uint8_t *buf);


#endif /* RF_DRIVER_H_ */
