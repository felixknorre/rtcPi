/*
 * setTime.c
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
#include <time.h>



#define DEVICE_NAME "/dev/rtcPi"
#define BUFFER_SIZE 32
#define SET_TIME 0x01


int main(int argc, char *argv[])
{
	// file descriptor
	int fd; 
	// return value of read 
	int result;
	// buffer for date time
	char buff[BUFFER_SIZE];
	// keep time
	time_t u_time;
	struct tm * curr_time;
	
	//get current time and format into buffer
	time(&u_time);
	curr_time = localtime(&u_time);
	strftime(buff, sizeof(buff), "%a, %d-%m-%Y %X", curr_time);
	printf("%s\n", buff);

	// open mod
	fd = open(DEVICE_NAME, O_WRONLY | O_NONBLOCK);
	if(fd < 0){
		printf("can't open device...\n");
		return -1;
	} else {
		// open device success
		result = write(fd, buff, sizeof(buff));
		if(result < 0){
			printf("can't set time");
			return -1;
		}
					
	}
	
	close(fd);
	
	return 0;
}

