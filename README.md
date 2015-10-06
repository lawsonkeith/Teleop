# Teleop introduction
RC car teleop system with haptic feedback using C on a raspberry Pi.  This uses much hardware from the XBOX 360 to create a very budget teleop system.

![](https://github.com/lawsonkeith/Teleop/raw/master/images/DSC_0019.JPG)

There are 4 software modules:

1. **teleop_client**		- Laptop
2. Web browser client		- Laptop 
3. **teleop_server**		- Pi
4. Webcam server		- Pi
 
There are 6 hardware modules:

1. XBOX 360 gamepad		- Laptop	
2. Wireless XBOX receiver	- Laptop
3. MPU6050 IMU			- Pi
4. Wifi dongle 1000mw		- Pi	
5. DCDC Converter		- Pi
6. XBOX Webcam			- Pi

![](https://github.com/lawsonkeith/Teleop/raw/master/images/Schematic.png)

Functionally there is a remote RC car and a laptop operator control unit (Laptop).

![](https://github.com/lawsonkeith/Teleop/raw/master/images/DSC_0015.JPG)

##quickstart commands cheat sheet
1. [PC] ssh 192.168.1.8
2. [PI] cd mjpeg-streamer
3. [PI] ./mjpeg-streamer start
4. [PI] cd ~/Teleop/server
5. [PI] sudo ./teleop_server
6. [PC] firefox http://192.168.1.8:8080/?action=stream
7. [PC] Player-removable-devices->microsoft XBOX->Capture
8. [PC] cd ~/client
9. [PC] sudo ./teleop_client /dev/input/event3 /dev/input/js0 192.168.1.8
10.[PC] as required....   scp ../server/teleop_server.c pi@192.168.1.8:/home/pi/Teleop/server

![](https://github.com/lawsonkeith/Teleop/raw/master/images/Screenshot_2015-09-27_17-24-26.png)

 
##General installation and usage

1. Run linux in a VM, get the controller running in windows then capture it in VMWAREs device capture menu.  
2. The LED on the controller should stay the same indicating it's working.
3. Install the client code on the laptop then the server code on the raspberry pi.  
4. Recomile both using make; resolve and dependencies.
4. The Pi needs the video streamer server installing on it, check webcam.
5. Wire up the Pi as per the attached schematic, power up.  
6. Then boot the server then client code, you should be able to control the car and receive haptic feedback off the XBOX controller when the car crashes into walls etc.

![](https://github.com/lawsonkeith/Teleop/raw/master/images/Capture2.JPG)

##Limitations
1. Run from command line currently.
2. Currently the settings are passed by argument, this is a bit of a mess and should move over to ini files.
3. Error checking is limited.
4. Resolution of the video stream has to be really low to get low latency.
5. I didn't implement the LEDs on the schematic.
6. I used a smaller dongle type USB which had limited range with my TALK TALK router.
7. Because the coding for the mjpeg takes place on the pi the image res is limited, perhaps a better camera would help here i.e. PI-camera.



#Detailed explanation

##XBOX 360 controller testing
Acquire the windows XBOX wireless driver and install the drivers.  There's a tutorial here http://www.s-config.com/archived-xbox-360-receiver-install-for-win-xp-and-win-7/ if you buy a cheap Chinese copy and can't get it working.
The LED on the controller will indicate when it's paired.  The drivers should already be on linux if it's a recent release, I used Xubuntu 15.  Use the following commands to look for or test the XBOX controller.

1. use cat /proc/bus/input/devices, look at 'Handlers' if unsure!
2. fftest /dev/input/event3, this tests force feedback.
3. jstest /dev/input/js0, this test analogs.
4. ls /dev/input, you should see the joystick FIFOs here.
5. I didn't have to install anything in linux (apart from maybe jstest), all the pain was in windows.
 

##Installing video streaming on the pi
To do this it's best to run a lower res to get a faster update, I run at <320x240 and zoom in on the web page.  On higher res it was un-usable.

1. Make sure you have an updated version of Raspberry PI's OS.
2. Install libv4l-0 package, available in Raspbian: sudo aptitude install libv4l-0.
3. Connect the web camera to USB. The web camera must be Linux compatible; to check this, make sure /dev/video0 file is available on Raspberry PI, else the camera does not have a Linux driver or required extra configuration to work (this issue is not discussed here).
4. Download mjpg-streamer-rpi.tar.gz archive on Raspberry PI and extract it. Destination folder is not relevant. You don't need root access if you are using the default pi user. Go to mjpg-streamer folder, where you extracted the tar.gz file.
5. Open mjpg-streamer.sh file; this is a simple bash script to control the mini-webserver. The header contains some writable parameters, as refresh rate or resolution. The default settings should work in most situations.
6. In sh file Halve video res then  set YUV to 'true'.
7. Start the server with ./mjpg-streamer.sh start command in the current folder.
8. Run your prefered web browser and go to http://raspberrypi:8080/?action=stream (where raspberrypi is it's IP address). You should see the image from the webcam. Current version has some issues with Chrome, just use Firefox if the image is not refreshed.
9. If the system doesn't work, see the mjpg-streamer.log file for debug info.


	
##Git/misc cmds
Usefull cmds:

1. git clone https://github.com/lawsonkeith/Teleop.git
2. git push origin master
3. git commit -am "comment"
4. git pull origin master
5. scp teleop_server.c pi@192.168.1.8:/home/pi/Teleop/server (copy to pi)
6. make | head
7. git reset --hard origin/master (force local to repo ver)


##UDP tests
send data to a client (Pi) interactively:

1. ncat -vv 192.168.1.6 8888 -u
	
look for open port on:

1. netstat -u -a
2. netstat -lnpu


##nano
Some usefull nano cmds...

1. CTRL+6 block sel
2. CTRL+6 copy
3. CTRL+K cut
4. CTRL+U uncut
5. F4 		SEL DN


##Pi Cmds
General housekeeping..

sudo apt-get update
sudo apt-get dist-upgrade
sudo apt-get instal raspberrypi-ui-mods


##MPU6050
The PI uses a 6050 IMU.  First off oyu have to enable I2C on the pi, wire it up then test it using the commands in 'scripts'. Again there may be some dependencies.  The server transmits impacts back to the client to transmit to the
operator as haptic feedback using the XBOX force feedback.

http://www.instructables.com/id/Reading-I2C-Inputs-in-Raspberry-Pi-using-C/?ALLSTEPS

1. Install i2c tools...
2. sudo apt-get install libi2c-dev
3. Edit the i2c On the pi's BIOS then reboot.
4. sudo raspi-config
5. sudo nano /etc/modules
6. sudo nano /etc/modprobe.d/raspi-blacklist.conf 
7. sudo i2cdetect -y 1
8. The MPU605 should appear as 68


##Pi Blaster
The PWM on the Pi is done with the Pi blaster Daemon.  The server sends data to the servos direct from the Pi and in software to the PI-BLASTER FIFO.

1. sudo apt-get install autoconf
2. git clone https://github.com/sarfata/pi-blaster.git
3. cd pi-blaster
4. ./autogen.sh
5. ./configure
6. make
7. sudo make install
8. sudo make uninstall - to stop auto start

FIFO is at /dev/pi-blaster

GPIO number| Pin in P1 header
--- | ---
4    |     P1-7
17   |    P1-11
18   |     P1-12
21   |   P1-13
22   |    P1-15
23   |    P1-16
24   |    P1-18
25   |    P1-22
     
![](https://github.com/lawsonkeith/Teleop/raw/master/images/Capture.JPG)
      
To completely turn on GPIO pin 17:

* echo "17=1" > /dev/pi-blaster

To set GPIO pin 17 to a PWM of 20%

* echo "17=0.2" > /dev/pi-blaster



**NOTE** - I've had issues with this interfering with the PIs windows environment in the past with lockups so I don't tend to boot into the PI X windows interface.

##wifi
Follow adafruits guide to seting up the wifi using the terminal on the Pi.  I found it easier to do it via the
command line. 

1. sudo apt-get install avahi-daemon
2/ sudo nano /etc/network/interfaces

##Refs
1. http://beej.us/guide/bgipc/output/html/singlepage/bgipc.html#fork
2. http://gnosis.cx/publish/programming/sockets2.html
3. http://www.2net.co.uk/tutorial/periodic_threads
4. IMU pulled from PiBits repo.
5. Use fftest and jstest for XBOX code
