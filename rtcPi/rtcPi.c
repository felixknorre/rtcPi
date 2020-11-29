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

// #include <wiringPi.h>

#define DRIVER_NAME "rtcPi"
#define CLASS_NAME "rtcPiClass"
#define DEVICE_Name  "rtcPi"

// set debug mode to log program execution
#define DEBUG_MODE

/***************************************/
/*       Application Interface         */
/***************************************/

static int mod_open(struct inode* fsdev, struct file * mm_entity){
  printk("rtcPi: opend...\n");
  return 0;
}

static int mod_close(struct inode* fsdev, struct file * mm_entity){
  printk("rtcPi: closed...\n");
    return 0;
}

static ssize_t mod_write(struct file * mm_entity, const char * buffer, size_t count, loff_t * offset){
  printk("rtcPi: write...\n");
    return 0;
}

static ssize_t mod_read(struct file * mm_entity, char * buffer, size_t count, loff_t * offset){
  printk("rtcPi: read...\n");
    return 0;
}

static long mod_ioctl(struct file * mm_entitiy, unsigned int cmd, unsigned long arg){
  printk("rtcPi: ioctl...\n");
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
  .open = mod_open,
  .release = mod_close,
  .read = mod_read,
  .write = mod_write,
  .unlocked_ioctl = mod_ioctl
};
  
// class
static struct class * mod_class = NULL;
// device 
static struct device * mod_device = NULL;
  
// cdev registration
int registration(void){
  
  int result;
  
  // 1. alloc a device number 
  if(alloc_chrdev_region(&mod_dn, 0, 1, DRIVER_NAME)){
      printk("rtcPi ERROR: dyn. device number assignment failed...\n");
      return -EIO;
  }
  
  #ifdef DEBUG_MODE
    printk("rtcPi DEBUG: Dynamic device number assignment: DN=%d MAJOR=%d MINOR=%d", mod_dn, MAJOR(mod_dn), MINOR(mod_dn));
  #endif
  
  // 2. alloc character device object
  mod_cdev_object = cdev_alloc();
  if(mod_cdev_object == NULL){
      printk("rtcPi ERROR: driver object allocation failed...\n");
      goto free_device_number;
  }
  
  // 3. init character device object
  cdev_init(mod_cdev_object, &f_op);
  kobject_set_name(&mod_cdev_object->kobj, DRIVER_NAME);
  mod_cdev_object->owner = THIS_MODULE;
  
  // 4. add cdev into kernel 
  result = cdev_add(mod_cdev_object, mod_dn, 1);
  if(result != 0){
    printk("rtcPi ERROR: adding device failed...\n");
    goto free_cdev;
  }
  
  // 5. create class
  mod_class = class_create(THIS_MODULE, CLASS_NAME);
  if(mod_class == NULL){
      printk("rtcPi ERROR: create class failed...\n");
      goto free_cdev;
  }
  
  // 6. create device in /dev
  mod_device = device_create(mod_class, NULL, mod_dn, NULL, DEVICE_Name);
  if(mod_device == NULL){
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
int deregistration(void){

  device_destroy(mod_class, mod_dn);
  class_destroy(mod_class);
  
  if(mod_cdev_object){
      cdev_del(mod_cdev_object);
  }
  
  unregister_chrdev_region(mod_dn, 1);
  
  return 0;
  
}


/***************************************/
/*      Module De-/Initialization      */
/***************************************/

static int __init rtcPi_init(void){
  
  int result = 0;
  
  result = registration();

  return 0;
}

static void __exit rtcPi_exit(void){
  int result = 0;
  
  result = deregistration();


}


module_init(rtcPi_init);
module_exit(rtcPi_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Felix Knorre <felix-knorre@hotmail.de>");
MODULE_DESCRIPTION("Raspberry Pi RTC Driver");
