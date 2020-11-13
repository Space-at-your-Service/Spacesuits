#!/bin/sh

# Install script for the raspbuino environment
# THIS ONLY NEEDS TO RUN ONCE (succesfully)

sudo apt-get update -y

# Create persistent data folders

mkdir ~/raspbuino-persistent
mkdir ~/raspbuino-persistent/scripts
mkdir ~/raspbuino-persistent/data
mkdir ~/raspbuino-persistent/backups
mkdir ~/raspbuino-persistent/modules


# Installing Arduino

cd ~
sudo apt-get install git make gcc ant openjdk-8-jdk -y
sudo apt-get install arduino -y
#git clone --depth 1 https://github.com/arduino/Arduino.git
#cd Arduino/build
#ant dist
#ant run
#cd ~

# Install dhcp server for ad-hoc connections

sudo apt-get install dnsmasq hostapd isc-dhcp-server -y

# Install python, flask, etc.
## Could try installing pip without sudo
sudo apt-get install python python-pip python-dev sqlite3 -y
sudo pip install --upgrade pip
sudo pip install wheel flask wtforms 
sudo apt-get install libatlas3-base 

#Unsure about this step
#sudo apt-get remove python-numpy python-matplotlib -y
sudo pip install freetype-py pypng 
#libpng-devel libfreetype-devel
sudo pip install numpy matplotlib --no-cache-dir --progress-bar ascii

# Installing OpenCV in python3 or python2
#sudo apt install python3-opencv 
#sudo apt install python-opencv
pip3 install imutils
pip3 install  opencv-contrib-python==3.4.6.27
sudo apt-get install libatlas-base-dev gfortran libjasper-dev libqtgui4 libqt4-test libgstreamer1.0-0 libhdf5-dev libhdf5-serial-dev libharfbuzz-dev libilmbase23 libopenexr-dev libavcodec-dev libavformat-dev libswscale-dev libv4l-dev libxvidcore-dev libx264-dev


# Check I2C
#gpio readall
#sudo i2cdetect -y 1
#i2cget -y 1 0x08 0x00 w
#printf "%d\n" 0x0150

## Lower display brightness to save power
#sudo rm /sys/class/backlight/rpi_backlight/brightness
#sudo touch /sys/class/backlight/rpi_backlight/brightness
#sudo echo "60" >> /sys/class/backlight/rpi_backlight/brightness

# Installing raspbuino and automatic updates

cp ~/raspbuino/boot.ssh.init/rsa/* ~/.ssh/
chmod 600 ~/.ssh/id_rsa
ssh-copy-id -i ~/.ssh/id_rsa.pub git@gitlab.com

cp ~/raspbuino/continuous-updates.sh ~/.continuous-updates.sh
chmod +x ~/.continuous-updates.sh
# Don't run the following line more than once
crontab -l | { cat; echo "* * * * * ~/.continuous-updates.sh"; } | crontab -

# Set rc.local to turn on Access Point on every boot
# Not currently working.. :/ 
sudo cp /etc/rc.local ~/raspbuino-persistent/backups/rc.local.sav
cat ~/raspbuino/data/access-point-rc.local.txt | sudo tee /etc/rc.local
 
cd ~

echo "Install should be complete!"
echo "(Do not run the installer again)"
echo "Please restart your machine"