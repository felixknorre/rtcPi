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
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/i2c.h>
 
 // i2c
#define I2C_DRIVER_NAME "i2c-rtc-driver"
#define I2C_RTC_ADDRESS 0x68
#define I2C_ADAPTER_NR 1
#define RTC_NAME "i2x-rtc-device"

static struct i2c_client *rtc_client;

/***************************************/
/*   driver read/write operations      */
/***************************************/

// read from i2c bus
int rtc_i2c_read(char * buf, int byte){
    return i2c_master_recv(rtc_client, buf, byte);
}

// write to i2c bus
int rtc_i2c_write(const char * buf, int byte){
  return i2c_master_send(rtc_client, buf, byte);
}

EXPORT_SYMBOLE(rtc_i2c_read);
EXPORT_SYMBOLE(rtc_i2c_write);

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

int rtc_i2c_init(){
  int result;
  struct i2c_adapter *adapter = i2c_get_adapter(I2C_ADAPTER_NR);
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


void rtc_i2c_exit(){
  i2c_unregister_device(rtc_client);
  i2c_del_driver(&rtc_driver);

}


module_init(rtc_i2c_init);
module_exit(rtc_i2c_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Felix Knorre <felix-knorre@hotmail.de>");
MODULE_DESCRIPTION("Raspberry Pi I2C RTC Driver");
