/*
 * SD_driver.h
 *
 *  Created on: Feb 2018
 *      Author: lourw
 */

#ifndef SD_DRIVER_H_
#define SD_DRIVER_H_


#include "driverlib.h"
#include "fatfs/src/diskio.h"
#include "fatfs/src/ff.h"
#include "spiDriver.h"


void SD_init(FATFS *fs, FIL *fo);
void SD_append(char *line, FATFS *fs, FIL *fo);


#endif /* SD_DRIVER_H_ */
