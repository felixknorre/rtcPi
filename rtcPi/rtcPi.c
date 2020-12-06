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

// registration
#define DRIVER_NAME "rtcPi"
#define CLASS_NAME "rtcPiClass"
#define DEVICE_NAME  "rtcPi"

// set debug mode to log program execution
#define DEBUG_MODE

// set rtc type mode
#define RTC_TYPE_I2C
//#define RTC_TYPE_USB

// i2c
#define I2C_DRIVER_NAME "i2c-rtc"
#define I2C_RTC_ADDRESS 0x68
#define I2C_ADAPTER_NR 1
#define RTC_NAME "ds3231"


 // create and delete 
int rtc_i2c_init(void);
void rtc_i2c_exit(void);

// read and write to the i2c bus
int rtc_i2c_read(const struct i2c_client *, char *, int);
int rtc_i2c_write(const struct i2c_client *, const char *, int);

/***************************************/
/*       Application Interface         */
/***************************************/


static int rtcpi_open(struct inode* fsdev, struct file * mm_entity) {
  /*
#ifdef RTC_TYPE_I2C

    struct i2c_client *client;
    struct i2c_adapter *adapter;
    char client_name[] = "ds3231";

    adapter = i2c_get_adapter(I2C_ADAPTER_NR);
    if(adapter == NULL) {
        return -1;
    }

    // kzalloc alloc memory and set it to zero
    //GFP_KERNEL -> get free pages
    client = kzalloc(sizeof(*client), GFP_KERNEL);
    if(client == NULL) {
        return -1;
    }

    memcpy(&client->name, client_name, sizeof(client_name));
    client->addr = I2C_ADDRESS;
    client->adapter = adapter;

    // store data
    mm_entity->private_data = client;

#endif
*/



#ifdef DEBUG_MODE
    printk("rtcPi: opend...\n");
#endif

    return 0;
}

static int rtcpi_close(struct inode* fsdev, struct file * mm_entity) {
/*
#ifdef RTC_TYPE_I2C
    struct i2c_client *client = mm_entity->private_data;
    i2c_put_adapter(client->adapter);
    kfree(client);
    mm_entity->private_data = NULL;
#endif
*/

#ifdef DEBUG_MODE
    printk("rtcPi: closed...\n");
#endif

    return 0;
}

static ssize_t rtcpi_write(struct file * mm_entity, const char * buffer, size_t count, loff_t * offset) {
#ifdef DEBUG_MODE
    printk("rtcPi: write...\n");
#endif
    return 0;
}

static ssize_t rtcpi_read(struct file * mm_entity, char * buffer, size_t count, loff_t * offset) {
#ifdef DEBUG_MODE
    printk("rtcPi: read...\n");
#endif
    return 0;
}

static long rtcpi_ioctl(struct file * mm_entitiy, unsigned int cmd, unsigned long arg) {
    // use arg to set client->addr = 0x68
    // or don't use it and set client->addr = 0x68 as default
#ifdef DEBUG_MODE
    printk("rtcPi: ioctl...\n");
#endif
    return 0;
}


/***************************************/
/*             I2C Client              */
/***************************************/

static int rtc_probe(struct i2c_client * client, const struct i2c_device_id * id){
    return 0;
}

static int rtc_remove(struct i2c_client * client){
    return 0;
}

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

static struct i2c_client *rtc_client;

static struct i2c_board_info rtc_i2c_board_info[] = {
    {
        I2C_BOARD_INFO(RTC_NAME, I2C_RTC_ADDRESS),
    },
};

  

 // create client/driver
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

// delete client/driver
void rtc_i2c_exit(){
  i2c_unregister_device(rtc_client);
  i2c_del_driver(&rtc_driver);

}

// read from i2c bus
int rtc_i2c_read(const struct i2c_client * client, char * buf, int byte){
    return i2c_master_recv(client, buf, byte);
}

// write to i2c bus
int rtc_i2c_write(const struct i2c_client *client, const char * buf, int byte){
  return i2c_master_send(client, buf, byte);
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

    result = registration();
    if(result == -1){
        return -1;
    }
    
    result = rtc_i2c_init();

    return 0;
}

static void __exit rtcpi_exit(void) {
    int result = 0;

    result = deregistration();


}

module_init(rtcpi_init);
module_exit(rtcpi_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Felix Knorre <felix-knorre@hotmail.de>");
MODULE_DESCRIPTION("Raspberry Pi RTC Driver");
