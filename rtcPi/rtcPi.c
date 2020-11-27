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

static int __init rtcPi_init()
{

  return 0;
}

static void __exit rtcPi_exit()
{


}


module_init(rtcPi_init);
module_exit(rtcPi_exit);

MODULE_LICENSE("GPL-2.0");
MODULE_AUTHOR("Felix Knorre <felix-knorre@hotmail.de>");
MODULE_DESCRIPTION("Raspberry Pi RTC Driver");
