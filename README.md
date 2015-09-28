## Teleop
RC car teleop system with haptic feedback using C on a raspberry Pi.

There are 4 software modules:

1. **teleop_client**		- Laptop
2. Web browser client		- Laptop 
3. **teleop_server**		- Pi
4. Motion webcam server		- Pi
 
There are 6 hardware modules:

1. XBOX 360 gamepad		- Laptop	
2. Wireless xbox receiver	- Laptop
3. MPU6050 IMU			- Pi
4. Wifi dongle 1000mw		- Pi	
5. DCDC Converter		- Pi
6. Webcam			- Pi

Functionally there is a remote RC car and a laptop operator control unit (Laptop).

##usage
Acquire the windows XBOX wireless driver and install the drivers.  There's a tutorial here http://www.s-config.com/archived-xbox-360-receiver-install-for-win-xp-and-win-7/ if you buy a cheap Chinese copy and can't get it working.
The LED on the controller will indicate when it's paired.

Run the program inside a linux VM, get the controller running in windows then capture it in VMWAREs device capture menu.  The LED ont he controller should stay the same indicating it's working.

Install the client code on the laptop then the server code on the raspberry pi.  The Pi needs the Monitor code installing on it too.

Setup the Pi to boot it's server on startup as with the Monitor webcam device.  Wire up the Pi as per the attached schematic, power up.  Then boot the client code, you should be able to control the car and receive haptic feedback off the controller.


##Limitations
1. Currently if the server restarts the client doesn't reconnect.  Since this is just test code I think it's ok to relainch the client
2. Currently the settings are passed by argument, this is a bit of a mess and should move over to ini files.
3. Error checking is limited.
4. Joystick device enumeration is hard coded e.g. js0 event3 etc.


##XBOX 360 controller testing
This has been tested on Xubuntu 15.  Use the following commands to look for or test the XBOX controller.

1. use cat /proc/bus/input/devices, look at 'Handlers' if unsure!
2. fftest /dev/input/event3
3. jstest /dev/input/js0
4. ls /dev/input
 

##Installing Motion on the Pi
Install motion on the pi as follows:

1. Look at http://www.instructables.com/id/Raspberry-Pi-remote-webcam/
2. sudo apt-get install motion
3. sudo nano /etc/motion/motion.conf; follow instructables info
4. sudo nano /etc/default/motion; follow instructables info
5. sudo service motion start; starts service
6. sudo service motion stop; stops service
7. firefox 192.168.1.6:8081; launch browser
	
Note - I could only get the remote viewing working on my linux laptop.
	
##Git cmds
Usefull cmds:

1. git clone https://github.com/lawsonkeith/Teleop.git
2. git push origin master
3. git commit -am "comment"
4. git pull origin master


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

##references
Usfull notes etc.
Linux timers - 2net.co.uk: periodic tasks in linux

##Pi Cmds
General housekeeping..

sudo apt-get update
sudo apt-get dist-upgrade
sudo apt-get instal raspberrypi-ui-mods

