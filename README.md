# Raspberry Pi RTC Kernel Module
WS20 LKESP2 Project

## Setup Raspberry Pi for kernel module development

### 1. Set timezone 
* important for make
*
### 2. Expand filesystem

```bash
raspi-config
```
* go to advanced settings and expand filesystem

### 3. Update system and  install git and build dependencies
```bash
sudo apt update -y
sudo apt upgrade -y
sudo apt install git bc bison flex libssl-dev make
```
  
### 4. Get sources
```bash 
git clone --depth=1 https://github.com/raspberrypi/linux
```
* __Note:__ this will take some time...

### 5. Kernel configuration
* for pi 2/3
```bash
cd linux
KERNEL=kernel7
sudo make bcm2709_defconfig
```

### 6. Building
```bash
sudo make -j4 zImage modules dtbs
sudo make modules_install
sudo make headers_install
sudo cp arch/arm/boot/dts/*.dtb /boot/
sudo cp arch/arm/dts/overlays/*.dtb* /boot/overlays/
sudo cp arch/arm/boot/overlays/README /boot/overlays/
sudo cp arch/arm/boot/zImage /boot/$KERNEL.img

```
* __Note:__``-j4`` flag splits the work between all four cores

### 7. Create link
```bash
ln -s /usr/src/linux /lib/modules/$(uname -r)/build
```

### 8. Install additiona packages
```bash
sudo apt install module-assistant
sudo apt install kernel-package
sudo apt install  fakeroot
sudo apt install libncurses5-dev
```

### 9. Create ssh key and add to github 

```bash
# create key
ssh-keygen -t rsa -b 4096 -C "<your@mail.com>"
eval $(ssh-agent -s)
ssh-add ~/.ssh/id_rsa

#  copy to clipboard
sudo apt install xclip
xclip -selection clipboard < ~/.ssh/id_rsa.pub
```
