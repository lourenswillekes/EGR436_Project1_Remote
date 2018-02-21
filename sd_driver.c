/*
 * sd_driver.c
 *
 *  Created on: Feb 2018
 *      Author: lourw
 */

#include "sd_driver.h"

#define LOGGER_LINE_SIZE    45
static const char *LOGGER_FILE_NAME = "log.txt";


int sd_Init(FATFS *fs, FIL *fo)
{
    FRESULT res;
    UINT write_bytes;
    char *LOGGER_HEADER = "Year Mo Da Ho Mi Tempe Humid Pressure Light\r\n";

    // mount the file system using logical disk 0
    res = f_mount(0, fs);
    // open a new log file in the fileobject
    res = f_open(fo, LOGGER_FILE_NAME, FA_WRITE | FA_CREATE_ALWAYS);
    // write the header line to the log file
    res = f_write(fo, LOGGER_HEADER, LOGGER_LINE_SIZE, &write_bytes);
    // close the file again
    res = f_close(fo);

    return res;

}


int sd_Append(char *line, FATFS *fs, FIL *fo)
{
    FRESULT res;
    UINT write_bytes;

    // Open a new file in the fileobject
    res = f_open(fo, LOGGER_FILE_NAME, FA_WRITE);
    // move pointer to the end of the file
    res = f_lseek(fo, fo->fsize);
    // write a data string to the sd card
    res = f_write(fo, line, LOGGER_LINE_SIZE, &write_bytes);
    // close the file
    res = f_close(fo);

    return res;
}
