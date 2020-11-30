/*
 * readMod.c
 * 
 * Copyright 2020 Felix Knorre <felix-knorre@hotmail.de>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wiringPi.h>
#include <lcd.h>

// WiringPi Pin Numbers
#define LCD_ROWS 2			// lcd display rows
#define LCD_COLS 16  	    // lcd display columns
#define LCD_BITS 4			// lcd bit mode
#define LCD_RS 25			// register select(GPIO 26)
#define LCD_E 24			//enable (GPIO 19)
#define LCD_D0 0			// not in use (4 bit mode)
#define LCD_D1 0			// not in use (4 bit mode)
#define LCD_D2 0			// not in use (4 bit mode)
#define LCD_D3 0			// not in use (4 bit mode)
#define LCD_D4 23			// Data 4 (GPIO 13)
#define LCD_D5 22			// Data 5 (GPIO 6)
#define LCD_D6 21			// Data 6 (GPIO 5)
#define LCD_D7 14			// Data  (GPIO 11)

// Device and Buffer setup
#define DEVICE_NAME "/dev/rtcPI"
#define BUFFER_SIZE 128

int main(int argc, char *argv[])
{
	int lcd;
	// file descriptor
	int fd; 
	// buffer to store the time
	char buf[BUFFER_SIZE];
	int read_result;

	// lcd display setup
	wiringPiSetup();
	lcd = lcdInit(LCD_ROWS, LCD_COLS, LCD_BITS, LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7, 0, 0, 0, 0);
	
	// start text
	lcdClear(lcd);
	lcdPosition(lcd, 4, 0);
    lcdPuts(lcd, "readMod");
    lcdPosition(lcd, 4, 1);
    lcdPuts(lcd, "loaded...");
    sleep(2);
    lcdClear(lcd);
    
    // open mod
    fd = open(DEVICE_NAME, O_RDWR | O_NONBLOCK);
    if(fd < 0){
		lcdPosition(lcd, 2, 0);
		lcdPuts(lcd, "can't open");
		lcdPosition(lcd, 4, 1);
		lcdPuts(lcd, "device...");
		close(fd);
		sleep(5);
		lcdClear(lcd);
		return -1;
	} else {
		lcdPosition(lcd, 4, 0);
		lcdPuts(lcd, "opened");
		lcdPosition(lcd, 4, 1);
		lcdPuts(lcd, "device...");
		close(fd);
		sleep(5);
		lcdClear(lcd);
		return 0;
	}
	
	// read time from mod every second
	/*while(1){
		// BUFFER_SIZE-1 because of \0
		read_result = read(fd, buf, BUFFER_SIZE-1);
		if(read_result < 0){
			lcdClear(lcd);
			lcdPosition(lcd, 2, 0);
			lcdPuts(lcd, "ERROR reading");
			lcdPosition(lcd, 4, 1);
			lcdPuts(lcd, "device...");
				
		} else {
			// new time and date
			lcdClear(lcd);
			// date
			lcdPosition(lcd, 2, 0);
			lcdPuts(lcd, "Date: ");
			// time
			lcdPosition(lcd, 2, 1);
			lcdPuts(lcd, "Time: ");
		}
	} */
	
	// TODO: when should it close ?
	//close(fd);
	return 0;
}
