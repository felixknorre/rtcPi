/*
 * rtcI2C.c
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
#include <linux/init.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/i2c.h>
#include <linux/time.h>
#include <linux/bcd.h>
 
 // i2c device and driver
#define I2C_DRIVER_NAME "i2c-rtc-driver"
#define I2C_RTC_ADDRESS 0x68
#define I2C_ADAPTER_NR 1
#define RTC_NAME "i2x-rtc-device"

#define DEBUG_MODE

// rtc register addresses
#define SEC_ADD 0x0
#define MIN_ADD 0x1
#define HR_ADD 0x2
#define DAY_ADD 0x3
#define DATE_ADD 0x4
#define MON_ADD 0x5
#define YEAR_ADD 0x2

static struct i2c_client *rtc_client;

/***************************************/
/*   driver read/write operations      */
/***************************************/

/*
 * get time
 */
int rtc_i2c_read(struct tm *curr_time){
    int result = 0;
    int sec, min, hour, mday, mon, year, wday;
    
    // get current time from rtc
    // i2c_master_recv(rtc_client, buf, byte);
    sec = i2c_smbus_read_byte_data(rtc_client, SEC_ADD);
    
    
    // convert from 
    curr_time->tm_sec = bcd2bin(sec);
    
    
    #ifdef DEBUG_MODE
    printk("rtcI2C READ: Get DATE: %02d-%02d-%4ld (wday = %d) TIME: %2d:%02d:%02d\n",
        curr_time->tm_mday, curr_time->tm_mon, curr_time->tm_year, curr_time->tm_wday,
        curr_time->tm_hour, curr_time->tm_min, curr_time->tm_sec);
    #endif
    
    return result;
}

/*
 * set time
 */
int rtc_i2c_write(struct tm *curr_time){
    int result = 0;
    
    #ifdef DEBUG_MODE
    printk("rtcI2C WRITE: Set DATE: %02d-%02d-%4ld (wday = %d) TIME: %2d:%02d:%02d\n",
          curr_time->tm_mday, curr_time->tm_mon, curr_time->tm_year, curr_time->tm_wday,
          curr_time->tm_hour, curr_time->tm_min, curr_time->tm_sec);
    #endif
  
  // set current time to rtc
  // i2c_master_send(rtc_client, buf, byte);
  return result;
}

EXPORT_SYMBOL(rtc_i2c_read);
EXPORT_SYMBOL(rtc_i2c_write);

/***************************************/
/*      I2C driver operations          */
/***************************************/


static int rtc_probe(struct i2c_client * client, const struct i2c_device_id * id){
    return 0;
}

static int rtc_remove(struct i2c_client * client){
    return 0;
}

/***************************************/
/*      Module De-/Initialization      */
/***************************************/

// driver handels just one rtc
static struct i2c_device_id rtc_idtable[] = {
    {RTC_NAME, 0},
    {},
};

MODULE_DEVICE_TABLE(i2c, rtc_idtable);

static struct i2c_driver rtc_driver = {
    .driver = {
          .name = I2C_DRIVER_NAME,
      },
    .id_table = rtc_idtable,
    .probe = rtc_probe,
    .remove = rtc_remove,
};


static struct i2c_board_info rtc_i2c_board_info[] = {
    {
        I2C_BOARD_INFO(RTC_NAME, I2C_RTC_ADDRESS),
    },
};

int rtc_i2c_init(void){
  int result;
  struct i2c_adapter *adapter = i2c_get_adapter(I2C_ADAPTER_NR);

  printk("rtcI2C: init...\n");
  
  if(adapter == NULL){
      printk("rtcPi ERROR: get i2c adapter failed...\n");
      goto no_adapter;
  }
  
  result = i2c_add_driver(&rtc_driver);
  if(result != 0){
      printk("rtcPi ERROR: create i2c driver failed...\n");
      goto put_adapter;
  }
  
  rtc_client = i2c_new_device(adapter, &rtc_i2c_board_info[0]);
  if(rtc_client  == NULL){
    printk("rtcPi ERROR: create i2c device failed...\n");
    goto free_driver;
  }
  
  
  i2c_put_adapter(adapter);
  return 0;
  
free_driver:
    i2c_del_driver(&rtc_driver);
put_adapter:
    i2c_put_adapter(adapter);
no_adapter:
    return -1;
}


void rtc_i2c_exit(void){
  
  printk("rtcI2C: exit...\n");
  
  i2c_unregister_device(rtc_client);
  i2c_del_driver(&rtc_driver);

}


module_init(rtc_i2c_init);
module_exit(rtc_i2c_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Felix Knorre <felix-knorre@hotmail.de>");
MODULE_DESCRIPTION("I2C RTC DS3231 Driver");
