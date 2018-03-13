/*
 * SD_driver.c
 *
 *  Created on: Feb 2018
 *      Author: lourw
 */

#include "SD_driver.h"

#define LOGGER_LINE_SIZE    39
static const char *LOGGER_FILE_NAME = "log.txt";


void SD_init(FATFS *fs, FIL *fo)
{
    UINT write_bytes;
    char *LOGGER_HEADER = "Year,Mo,Da,Ho,Mi,Humi,Press,Temp,Light\n";

    // open spi to the sd card
    spi_Open();
    // mount the file system using logical disk 0
    f_mount(0, fs);
    // open a new log file in the fileobject
    f_open(fo, LOGGER_FILE_NAME, FA_WRITE | FA_CREATE_ALWAYS);
    // write the header line to the log file
    f_write(fo, LOGGER_HEADER, LOGGER_LINE_SIZE, &write_bytes);
    // close the file again
    f_close(fo);

}


void SD_append(char *line, FATFS *fs, FIL *fo)
{
    UINT write_bytes;

    // Open a new file in the fileobject
    f_open(fo, LOGGER_FILE_NAME, FA_WRITE);
    // move pointer to the end of the file
    f_lseek(fo, fo->fsize);
    // write a data string to the sd card
    f_write(fo, line, LOGGER_LINE_SIZE, &write_bytes);
    // close the file
    f_close(fo);

}
