## Teleop
RC car teleop system with haptic feedback using C on a raspberry Pi.

There are 2 modules:
	1.teleop_client		runs on a laptop
	2.teleop_server		runs on the pi



#usage
#
Get the controller running in windows then capture it in VMWARE
then go into lite ot lite 2 and run the demo code

#Limitations
Currently if the server restarts the client doesn't reconnect.  Since this is 
just test code I think it's ok to relainch the client
Currently the settings are passed by argument, this is a bit of a mess and should
move over to ini files.

#Commands
read needs /dev/input/js0 as an argument

use cat /proc/bus/input/devices, look at 'Handlers' if unsure!
cat /dev/bus/input/devices
fftest /dev/input/event3
jstest /dev/input/js0

#Motion
Install motion on the pi as follows.
http://www.instructables.com/id/Raspberry-Pi-remote-webcam/

	sudo apt-get install motion
	sudo nano /etc/motion/motion.conf
	sudo nano /etc/default/motion
	sudo service motion start
	sudo service motion stop
	
Note - I could only get it working on my laptop.
	
#Launch web browser
	firefox 192.168.1.6:8081


#Git cmds
Clone from master repo
	git clone https://github.com/lawsonkeith/Teleop.git
	
Other cmds
	git push origin master
	git commit -am "comment"
	git pull origin master

#UDP tests
send data to a client interactively:
	ncat -vv 192.168.1.6 8888 -u
	
look for open port on:
	netstat -u -a
	netstat -lnpu

#nano
CTRL+6 block sel
CTRL+6 copy
CTRL+K cut
CTRL+U uncut
F4 		SEL DN

#references
Linux timers - 2net.co.uk: periodic tasks in linux

#Pi Cmds

sudo apt-get update
sudo apt-get dist-upgrade
sudo apt-get instal raspberrypi-ui-mods

