/*
 * sd_driver.h
 *
 *  Created on: Feb 2018
 *      Author: lourw
 */

#ifndef SD_DRIVER_H_
#define SD_DRIVER_H_


#include "driverlib.h"
#include "fatfs/src/diskio.h"
#include "fatfs/src/ff.h"


int sd_Init(FATFS *fs, FIL *fo);
int sd_Append(char *line, FATFS *fs, FIL *fo);


#endif /* SD_DRIVER_H_ */
