# Teleop
RC car teleop system with haptic feedback using C on a raspberry Pi.

There are 2 modules:
	teleop_client		runs on a laptop
	teleop_server		runs on the pi



#contents
#
trash/
	lite - force feedback
	lite2 - read data
	gamepad - bollox

#usage
#
Get the controller running in windows then capture it in VMWARE
then go into lite ot lite 2 and run the demo code


#Commands
read needs /dev/input/js0 as an argument

use cat /proc/bus/input/devices, look at 'Handlers' if unsure!
cat /dev/bus/input/devices
fftest /dev/input/event3
jstest /dev/input/js0

firefox 192.168.1.6:8081
ss
netstat -u -a
netcat -ul 2115


#references
Linux timers - 2net.co.uk: periodic tasks in linux
