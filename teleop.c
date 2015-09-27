
/*
 * $teleop.c$
 *
 * Teleop controller 
 * -----------------
 * This program allows an XBOX 360 controller to controll a 1/10 RC car
 * and transmit haptic feedback off an onboard IMU back to the operator
 * using the gamepads force feedback motors.
 * 
 * The RC car is controlled over wifi using a raspberry PI, an MPU 6050
 * is used for the Pi to locally read shock.
 * 
 * UDP messages allow the gampad to control the cars servos using soft
 * PWM daemon on the Pi.  The pi then sends back haptic feedback commands
 * derived from the MPU.
 * 
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/input.h>

#define BITS_PER_LONG (sizeof(long) * 8)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define BIT(x)  (1UL<<OFF(x))
#define LONG(x) ((x)/BITS_PER_LONG)
#define test_bit(bit, array)    ((array[LONG(bit)] >> OFF(bit)) & 1)

#define N_EFFECTS 6

void FF_Rumble(unsigned int magnitude);
void FF_Init(char *device_file_name);

char* effect_names[] = {
	"Sine vibration",
	"Constant Force",
	"Spring Condition",
	"Damping Condition",
	"Strong Rumble",
	"Weak Rumble"
};

// Main shared structures
struct ff_effect effects;	
struct input_event play, stop;
int fd;
	
	
// Main control program for Teleop program. 
//	
int main(int argc, char** argv)
{
	
	char device_file_name[64];
	int i;

	/* Read args */
	strncpy(device_file_name, "/dev/input/event0", 64);

	for (i=1; i<argc; ++i) {
		if (strncmp(argv[i], "--help", 64) == 0) {
			printf("Usage: %s /dev/input/eventXX\n", argv[0]);
			printf("Tests the force feedback driver\n");
			exit(1);
		}
		else {
			strncpy(device_file_name, argv[i], 64);
		}
	}
	
	FF_Init(device_file_name);
	
	FF_Rumble(0x1000);
	
	while(1);
}//END main



// Initialise the Force feedback device handlers
// open file handle.
//
void FF_Init(char *device_file_name)
{
	unsigned long features[4];
	int n_effects;	/* Number of effects the device can play at the same time */

	/* Open device */
	fd = open(device_file_name, O_RDWR);
	if (fd == -1) {
		perror("Open device file");
		exit(1);
	}
	printf("Device %s opened\n", device_file_name);
	
	/* Query device */
	if (ioctl(fd, EVIOCGBIT(EV_FF, sizeof(unsigned long) * 4), features) < 0) {
		perror("Ioctl query");
		exit(1);
	}
	if (ioctl(fd, EVIOCGEFFECTS, &n_effects) < 0) {
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

	if (ioctl(fd, EVIOCSFF, &effects) < 0) {
		perror("Upload effects[0]");
	}
	
	play.type = EV_FF;
	play.code = effects.id;
	play.value = 1;

	if (write(fd, (const void*) &play, sizeof(play)) == -1) {
		perror("Play effect");
		exit(1);
	}
}//END FF_Rumble
   
