
/*
 * $teleop.c$
 *
 * Teleop controller 
 * -----------------
 * This program allows an XBOX 360 controller to controll a 1/10 RC car.
 *  
 * The RC car is controlled over wifi using a wireless controller plugged
 * into a laptop.  This then uses WIFI to communicate with raspberry PI on
 * the car.  This uses an MPU 6050 to locally read shock.
 * 
 * UDP messages allow the gampad to control the cars servos using soft
 * PWM daemon on the Pi.  The pi then sends back haptic feedback commands
 * derived from the MPU.
 * 
 * An XBOX webcam on the car can be accessed on the laptop via a web browser.
 *  #firefox 192.168.1.6:8081
 *
 * 
 * The aim is to evaluate the use of XBOX
 * 
 * usage
 * -----
 * sudo ./teleop /dev/input/event3
 *
 */


#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/joystick.h>

#define BITS_PER_LONG (sizeof(long) * 8)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define BIT(x)  (1UL<<OFF(x))
#define LONG(x) ((x)/BITS_PER_LONG)
#define test_bit(bit, array)    ((array[LONG(bit)] >> OFF(bit)) & 1)
#define NAME_LENGTH 128

void FF_Rumble(unsigned int magnitude);
void JS_Read(void);
void FF_Init(char *device_file_name);
void JS_Init(char *joy_file_name);

// Main shared structures
struct ff_effect effects;	
struct input_event play, stop;
int fd_e,fd_j;
	
	
// Main control program for Teleop program. 
//	
int main(int argc, char** argv)
{
	
	char event_file_name[64];
	char joy_file_name[64];
	int i;

	/* Read args */
	if(argc != 3){
		printf("usage: sudo ./teleop /dev/input/event3  /dev/input/js0\n");
		exit(1);	
	}	
	strncpy(event_file_name, argv[1], 64);
	strncpy(joy_file_name, argv[2], 64);

	FF_Init(event_file_name);
	JS_Init(joy_file_name);
	
	FF_Rumble(0x1000);
	JS_Read();
	
	
	while(1);
}//END main


// JS_Read  Left axis is throttle, right axis is steering
//
void JS_Read(void)
{
		unsigned char axes = 6;
		unsigned char buttons = 4;
		int *axis;
		int *button;
		int i;
		struct js_event js;

		axis = calloc(axes, sizeof(int));
		button = calloc(buttons, sizeof(char));

		while (1) {
			if (read(fd_j, &js, sizeof(struct js_event)) != sizeof(struct js_event)) {
				perror("\njstest: error reading");
				exit (1);
			}

			switch(js.type & ~JS_EVENT_INIT) {
			case JS_EVENT_BUTTON:
				button[js.number] = js.value;
				break;
			case JS_EVENT_AXIS:
				axis[js.number] = js.value;
				break;
			}

			printf("\r");

			if (axes) {
				printf("Axes: ");
				for (i = 0; i < axes; i++)
					printf("%2d:%6d ", i, axis[i]);
			}

			if (buttons) {
				printf("Btns: ");
				for (i = 0; i < buttons; i++)
					printf("%2d:%s ", i, button[i] ? "on " : "off");
			}

			fflush(stdout);
		}

}//END JS_Read



// Initialise file handle to read joystick data from the gamepad
// 
void JS_Init(char *joy_file_name)
{
	unsigned char axes = 2;
	unsigned char buttons = 2;
	int version = 0x000800;
	char name[NAME_LENGTH] = "Unknown";
	
	if ((fd_j = open(joy_file_name, O_RDONLY)) < 0) {
		perror("jstest");
		exit(1);
	}
	
	ioctl(fd_j, JSIOCGVERSION, &version);
	ioctl(fd_j, JSIOCGAXES, &axes);
	ioctl(fd_j, JSIOCGBUTTONS, &buttons);
	ioctl(fd_j, JSIOCGNAME(NAME_LENGTH), name);
	
	printf("Joystick (%s) has %d axes and %d buttons. Driver version is %d.%d.%d.\n",
		name, axes, buttons, version >> 16, (version >> 8) & 0xff, version & 0xff);
	printf("Testing ... (interrupt to exit)\n");
}//END JS_Init
	


// Initialise the Force feedback device handlers to write data to gamepad
// open file handle.
//
void FF_Init(char *device_file_name)
{
	unsigned long features[4];
	int n_effects;	/* Number of effects the device can play at the same time */

	/* Open device */
	fd_e = open(device_file_name, O_RDWR);
	if (fd_e == -1) {
		perror("Open device file");
		exit(1);
	}
	printf("Device %s opened\n", device_file_name);
	
	/* Query device */
	if (ioctl(fd_e, EVIOCGBIT(EV_FF, sizeof(unsigned long) * 4), features) < 0) {
		perror("Ioctl query");
		exit(1);
	}
	if (ioctl(fd_e, EVIOCGEFFECTS, &n_effects) < 0) {
		perror("Ioctl number of effects");
	}
	if(n_effects > 10)
		printf("Found XBOX 360 controller with [%d] effects\n", n_effects);
}//END FF_Init




// Set force feedback to rumble joypad foe 0-0xFFFF magnitude.
// we need to bu root to do this.
//
void FF_Rumble(unsigned int magnitude)
{
	/* download a periodic sinusoidal effect */
	effects.type = FF_PERIODIC;
	effects.id = -1;
	effects.u.periodic.waveform = FF_SINE;
	effects.u.periodic.period = 0.1*0x100;	/* 0.1 second */
	effects.u.periodic.magnitude = magnitude;	/* 0.5 * Maximum magnitude */
	effects.u.periodic.offset = 0;
	effects.u.periodic.phase = 0;
	effects.direction = 0x4000;	/* Along X axis */
	effects.u.periodic.envelope.attack_length = 0x100;
	effects.u.periodic.envelope.attack_level = 0;
	effects.u.periodic.envelope.fade_length = 0x100;
	effects.u.periodic.envelope.fade_level = 0;
	effects.trigger.button = 0;
	effects.trigger.interval = 0;
	effects.replay.length = 2000;  /* 2 seconds */
	effects.replay.delay = 0;

	if (ioctl(fd_e, EVIOCSFF, &effects) < 0) {
		perror("Upload effects[0]");
	}
	
	play.type = EV_FF;
	play.code = effects.id;
	play.value = 1;

	if (write(fd_e, (const void*) &play, sizeof(play)) == -1) {
		perror("Play effect");
		exit(1);
	}
}//END FF_Rumble
   
