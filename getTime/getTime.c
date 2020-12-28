/*
 * getTime.c
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
#include <string.h>

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
#define DEVICE_NAME "/dev/rtcPi"
#define BUFFER_SIZE 32

int substr(char * src, char * dst, int start, int end){
		int length = strlen(src);
		int i,k = 0;
		
		if(start < 0  ){
			printf("invalid start index...\n");
			return -1;
		}
		
		if(end > length){
			printf("invalid end index...\n");
			return -1;
		}
		
		if(end < start){
			printf("start index greater end index...\n");
			return -1;
		}
		
		for(i = start; i <= end; i++){
				dst[k] = src[i];
				k++;
		}
		
		dst[k] = '\0';
		
		return 0;
}

int main(int argc, char *argv[])
{
	int lcd;
	// file descriptor
	int fd; 
	// buffer to store the time
	char buf[BUFFER_SIZE+1];
	// return value of read 
	int read_result;
	// how often should it read ?
	int count = 10;
	// interval between read operation(in seconds)
	int readInterval = 1;
	// date
	char date[16];
	// time
	char time[16];

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
	fd = open(DEVICE_NAME, O_RDONLY | O_NONBLOCK);
	if(fd < 0){
		// open device failed
		lcdPosition(lcd, 2, 0);
		lcdPuts(lcd, "can't open");
		lcdPosition(lcd, 4, 1);
		lcdPuts(lcd, "device...");
		close(fd);
		sleep(3);
		lcdClear(lcd);
		return -1;
	} else {
		// open device success
		while(count > 0){
			// read datetime
			read_result = read(fd, buf, BUFFER_SIZE);
			if(read_result < 0){
				// stop if there is an error while reading
				lcdClear(lcd);
				lcdPosition(lcd, 2, 0);
				lcdPuts(lcd, "error while");
				lcdPosition(lcd, 4, 1);
				lcdPuts(lcd, "reading...");
				sleep(3);
				break;
			}
			

			substr(buf, date, 0, 14);			
			substr(buf, time, 16, 23);
	
			// //display datetime on lcd
			lcdPosition(lcd, 0, 0);
			//lcdPuts(lcd, "read device");
			lcdPrintf(lcd, "%s",date);
			lcdPosition(lcd, 4, 1);
			lcdPrintf(lcd, "%s", time);
			
			sleep(readInterval);
			count--;	
		}
		
		// close dev
		lcdClear(lcd);
		lcdPosition(lcd, 4, 0);
		lcdPuts(lcd, "readMod");
		lcdPosition(lcd, 4, 1);
		lcdPuts(lcd, "exiting...");
		sleep(3);
		
		// clean up
		close(fd);
		lcdClear(lcd);
		return 0;
	}
}
