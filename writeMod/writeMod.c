/*
 * writeMod.c
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
#define BUFFER_SIZE 7

int main(int argc, char *argv[]){
	
	int result;
	char buf[BUFFER_SIZE];
	// file descriptor
	int fd; 

	fd = open(DEVICE_NAME, O_WRONLY | O_NONBLOCK);
	if(fd < 0){
		printf("could not open %s\n", DEVICE_NAME);
		return -1;
	}
	
	// get current time and copy into buffer
	time_t curr_time = time(NULL);
	struct tm curr_timestamp = *localtime(&curr_time); 
	
	
	// write current time to mod
	result = write(fd, &buf, BUFFER_SIZE-1);
	if(result < 0){
			printf("could not write to module\n");
			close(fd);
			return -1;
	} else {
			printf("writing successful\n");
			close(fd);
			return 0;
	}
	
	
	
	
}
