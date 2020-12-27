/*
 * rtcPi.c
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

#include <linux/module.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/i2c.h>
#include <linux/time.h>

#include <linux/uaccess.h>
#include <linux/string.h>

// registration
#define DRIVER_NAME "rtcPi"
#define CLASS_NAME "rtcPiClass"
#define DEVICE_NAME  "rtcPi"

// set debug mode to log program execution
#define DEBUG_MODE

// set rtc type mode
#define RTC_TYPE_I2C
//#define RTC_TYPE_USB

#define DATETIME_BUFFER_SIZE 32 // 16x2 lcd -> 32 chars


// define i2c functions
extern int rtc_i2c_read(struct tm *curr_time);
extern int rtc_i2c_write(struct tm *curr_time);
extern int rtc_i2c_client_connected(void);

/***************************************/
/*           help functions            */
/***************************************/

int substr(const char * src, char * dst, int start, int end){
		int length = strlen(src);
		int i,k = 0;
		
		if(start < 0  ){
			printk("rtcPi error: substring invalid start index...\n");
			return -1;
		}
		
		if(end > length){
			printk("rtcPi error: substring invalid end index...\n");
			return -1;
		}
		
		if(end < start){
			printk("rtcPi error: substring start index greater end index...\n");
			return -1;
		}
		
		for(i = start; i <= end; i++){
				dst[k] = src[i];
				k++;
		}
		
		dst[k] = '\0';
		
		return 0;
}

/***************************************/
/*       Application Interface         */
/***************************************/


static int rtcpi_open(struct inode* fsdev, struct file * mm_entity) {
#ifdef DEBUG_MODE
    printk("rtcPi: opend...\n");
#endif
    return 0;
}

static int rtcpi_close(struct inode* fsdev, struct file * mm_entity) {
#ifdef DEBUG_MODE
    printk("rtcPi: closed...\n");
#endif

    return 0;
}

static ssize_t rtcpi_write(struct file * mm_entity, const char * buffer, size_t count, loff_t * offset) {
    char c_wday[4], c_mday[3], c_mon[3], c_year[5], c_hour[3], c_min[3], c_sec[3];
    long l_wday, l_mday, l_mon, l_year, l_hour, l_min, l_sec;
    int res;
    
    // struct to save datetime
    struct tm curr_time = {
      .tm_sec = 0,
      .tm_min = 0,
      .tm_hour = 0,
      .tm_mday = 1,
      .tm_mon = 0,
      .tm_year = 2020,
      .tm_wday = 0,
      .tm_yday = 1
      };
    
     
    #ifdef DEBUG_MODE
      printk("User Space time: %s", buffer);
    #endif
    
    substr(buffer, c_wday, 0, 2);
    substr(buffer, c_mday, 5, 6);
    substr(buffer, c_mon, 8, 9);
    substr(buffer, c_year, 11, 14);
    substr(buffer, c_hour, 16, 17);
    substr(buffer, c_min, 19, 20);
    substr(buffer, c_sec, 22, 23);
    
    
    // convert string to long
    res = kstrtol(c_mday, 10, &l_mday);
    if(res != 0){
        printk("rtcPi error: convert mday failed...\n");
        return -1;
    } 
    
    res = kstrtol(c_mon, 10, &l_mon);
    if(res != 0){
        printk("rtcPi error: convert month failed...\n");
        return -1;
    }
     
    res = kstrtol(c_year, 10, &l_year);
    if(res != 0){
        printk("rtcPi error: convert year failed...\n");
        return -1;
    }
    
    res = kstrtol(c_hour, 10, &l_hour);
    if(res != 0){
        printk("rtcPi error: convert hour failed...\n");
        return -1;
    }
    
    res = kstrtol(c_min, 10, &l_min);
    if(res != 0){
        printk("rtcPi error: convert min failed...\n");
        return -1;
    }
    
    res = kstrtol(c_sec, 10, &l_sec);
    if(res != 0){
        printk("rtcPi error: convert sec failed...\n");
        return -1;
    }
    
    // set new tm values
    curr_time.tm_wday = 1;
    curr_time.tm_mday = (int)l_mday;
    curr_time.tm_mon = (int)l_mon;
    curr_time.tm_year = (int)l_year;
    curr_time.tm_hour = (int)l_hour;
    curr_time.tm_min = (int)l_min;
    curr_time.tm_sec = (int)l_sec;
 
    #ifdef DEBUG_MODE
    printk("rtcPi READ: Get DATE: %02d-%02d-%4ld (wday = %d) TIME: %2d:%02d:%02d\n",
        curr_time.tm_mday, curr_time.tm_mon, curr_time.tm_year, curr_time.tm_wday,
        curr_time.tm_hour, curr_time.tm_min, curr_time.tm_sec);
    #endif
    
    //write to rtc
    if(rtc_i2c_client_connected() == 0){
      rtc_i2c_write(&curr_time);
    }
    
    #ifdef DEBUG_MODE
        printk("rtcPi: write...\n");
    #endif
    
    return 0;
}

static ssize_t rtcpi_read(struct file * mm_entity, char * buffer, size_t count, loff_t * offset) {
    int result = 0;
    int format_result;
    unsigned long copy_result;
    char datetime_buffer[DATETIME_BUFFER_SIZE+1]; // +1 -> \0 end of string
    
    // struct to save datetime
    struct tm curr_time = {
      .tm_sec = 0,
      .tm_min = 0,
      .tm_hour = 0,
      .tm_mday = 1,
      .tm_mon = 0,
      .tm_year = 2020,
      .tm_wday = 0,
      .tm_yday = 1
      };
    
    // get time from rtcI2c driver, if client is connected
    if(rtc_i2c_client_connected() == 0){
        rtc_i2c_read(&curr_time);
    }
     
    #ifdef DEBUG_MODE
    printk("rtcPi READ: Get DATE: %02d-%02d-%4ld (wday = %d) TIME: %2d:%02d:%02d\n",
        curr_time.tm_mday, curr_time.tm_mon, curr_time.tm_year, curr_time.tm_wday,
        curr_time.tm_hour, curr_time.tm_min, curr_time.tm_sec);
    #endif
    
    // format depending on the week day
    switch(curr_time.tm_wday){
        case 1:
          format_result = snprintf(datetime_buffer, sizeof(datetime_buffer), "Sun, %02d-%02d-%4ld %2d:%02d:%02d\n",
          curr_time.tm_mday, curr_time.tm_mon, curr_time.tm_year, 
          curr_time.tm_hour, curr_time.tm_min, curr_time.tm_sec);
          break;
        case 2:
          format_result = snprintf(datetime_buffer, sizeof(datetime_buffer), "Mon, %02d-%02d-%4ld %2d:%02d:%02d\n",
          curr_time.tm_mday, curr_time.tm_mon, curr_time.tm_year, 
          curr_time.tm_hour, curr_time.tm_min, curr_time.tm_sec);
          break;
        case 3:
          format_result = snprintf(datetime_buffer, sizeof(datetime_buffer), "Tue, %02d-%02d-%4ld %2d:%02d:%02d\n",
          curr_time.tm_mday, curr_time.tm_mon, curr_time.tm_year, 
          curr_time.tm_hour, curr_time.tm_min, curr_time.tm_sec);
          break;
        case 4:
          format_result = snprintf(datetime_buffer, sizeof(datetime_buffer), "Wed, %02d-%02d-%4ld %2d:%02d:%02d\n",
          curr_time.tm_mday, curr_time.tm_mon, curr_time.tm_year, 
          curr_time.tm_hour, curr_time.tm_min, curr_time.tm_sec);
          break;
        case 5:
          format_result = snprintf(datetime_buffer, sizeof(datetime_buffer), "Thu, %02d-%02d-%4ld %2d:%02d:%02d\n",
          curr_time.tm_mday, curr_time.tm_mon, curr_time.tm_year, 
          curr_time.tm_hour, curr_time.tm_min, curr_time.tm_sec);
          break;
        case 6:
          format_result = snprintf(datetime_buffer, sizeof(datetime_buffer), "Fri, %02d-%02d-%4ld %2d:%02d:%02d\n",
          curr_time.tm_mday, curr_time.tm_mon, curr_time.tm_year, 
          curr_time.tm_hour, curr_time.tm_min, curr_time.tm_sec);
          break;
        case 7:
          format_result = snprintf(datetime_buffer, sizeof(datetime_buffer), "Sat, %02d-%02d-%4ld %2d:%02d:%02d\n",
          curr_time.tm_mday, curr_time.tm_mon, curr_time.tm_year, 
          curr_time.tm_hour, curr_time.tm_min, curr_time.tm_sec);
          break;
        default:
         format_result = snprintf(datetime_buffer, sizeof(datetime_buffer), ", %02d-%02d-%4ld %2d:%02d:%02d\n",
          curr_time.tm_mday, curr_time.tm_mon, curr_time.tm_year, 
          curr_time.tm_hour, curr_time.tm_min, curr_time.tm_sec);
    }
    
    // copy current datetime into user space
    copy_result = copy_to_user(buffer, datetime_buffer, min(count, sizeof(datetime_buffer)));
    if(copy_result  != 0){
        printk("rtcPi error: read() counld not copy to user space...\n");
        result = -1;
    }
    
    return result;
}

static long rtcpi_ioctl(struct file * mm_entitiy, unsigned int cmd, unsigned long arg) {
#ifdef DEBUG_MODE
    printk("rtcPi: ioctl...\n");
#endif
    return 0;
}



/***************************************/
/*      Module De-/Registration        */
/***************************************/

// device number
static dev_t mod_dn;

// cdev object
static struct cdev* mod_cdev_object = NULL;

// file operations, POSIX functions
static struct file_operations f_op = {
    .owner = THIS_MODULE,
    .open = rtcpi_open,
    .release = rtcpi_close,
    .read = rtcpi_read,
    .write = rtcpi_write,
    .unlocked_ioctl = rtcpi_ioctl
};

// class
static struct class * mod_class = NULL;
// device
static struct device * mod_device = NULL;

// cdev registration
int registration(void) {

    int result;

    // 1. alloc a device number
    if(alloc_chrdev_region(&mod_dn, 0, 1, DRIVER_NAME)) {
        printk("rtcPi ERROR: dyn. device number assignment failed...\n");
        return -EIO;
    }

#ifdef DEBUG_MODE
    printk("rtcPi DEBUG: Dynamic device number assignment: DN=%d MAJOR=%d MINOR=%d", mod_dn, MAJOR(mod_dn), MINOR(mod_dn));
#endif

    // 2. alloc character device object
    mod_cdev_object = cdev_alloc();
    if(mod_cdev_object == NULL) {
        printk("rtcPi ERROR: driver object allocation failed...\n");
        goto free_device_number;
    }

    // 3. init character device object
    cdev_init(mod_cdev_object, &f_op);
    kobject_set_name(&mod_cdev_object->kobj, DRIVER_NAME);
    mod_cdev_object->owner = THIS_MODULE;

    // 4. add cdev into kernel
    result = cdev_add(mod_cdev_object, mod_dn, 1);
    if(result != 0) {
        printk("rtcPi ERROR: adding device failed...\n");
        goto free_cdev;
    }

    // 5. create class
    mod_class = class_create(THIS_MODULE, CLASS_NAME);
    if(mod_class == NULL) {
        printk("rtcPi ERROR: create class failed...\n");
        goto free_cdev;
    }

    // 6. create device in /dev
    mod_device = device_create(mod_class, NULL, mod_dn, NULL, DEVICE_NAME);
    if(mod_device == NULL) {
        printk("rtcPi ERROR: create device failed...\n");
        goto free_class;
    }


    return 0;

free_class:
    class_destroy(mod_class);

free_cdev:
    kobject_put(&mod_cdev_object->kobj);
    mod_cdev_object = NULL;

free_device_number:
    unregister_chrdev_region(mod_dn, 1);
    return -1;

}

// cdev deregistration
int deregistration(void) {

    device_destroy(mod_class, mod_dn);
    class_destroy(mod_class);

    if(mod_cdev_object) {
        cdev_del(mod_cdev_object);
    }

    unregister_chrdev_region(mod_dn, 1);

    return 0;

}

/***************************************/
/*      Module De-/Initialization      */
/***************************************/

static int __init rtcpi_init(void) {

    int result = 0;
    
    printk("rtcPi: init...\n");

    result = registration();
    if(result == -1){
        return -1;
    }

    return 0;
}

static void __exit rtcpi_exit(void) {
    int result = 0;
    
    printk("rtcPi: exit...\n");

    result = deregistration();

}

module_init(rtcpi_init);
module_exit(rtcpi_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Felix Knorre <felix-knorre@hotmail.de>");
MODULE_DESCRIPTION("rtc cdev driver");
