
/*
 * $teleop_client.c$
 *
 * K Lawson
 * 28 Sep 2015
 * 
 * 
 * Teleop controller 
 * -----------------
 * This program allows an XBOX 360 controller to controll a 1/10 RC car.
 *  
 * The RC car is controlled over wifi using a wireless controller plugged
 * into a laptop.  This then uses WIFI to communicate with raspberry PI on
 * the car.  
 * 
 * A periodic UDP message is used to send commands to the car and receive
 * status information back from the car.  Accelerometer info fromt he car is then 
 * used to control the dual shock on the hand controller.
 * 
 * The laptop is client and the car is the server.
 * 
 * An XBOX webcam on the car can be accessed on the laptop via a web browser.
 *  #firefox 192.168.1.6:8081
 *
 * 
 * The aim is to evaluate the use of XBOX tech for Sensabot
 * 
 * usage
 * -----
 * You must pass the joystick driver handles as well as the servers IP 
 * address.
 * 
 * sudo ./teleop_client /dev/input/event3 /dev/input/js0 192.168.1.6
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
#include <signal.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <linux/input.h>
#include <linux/joystick.h>
#include <arpa/inet.h>

 
#define BUFLEN 	512  //Max length of buffer
#define PORT 	8888   //The port on which to send data

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
void Periodic (int sig);
void SetupTimer(void);
void Task_Init(void);
void UDP_Init(char *ip_address);
void UDP_Send(int Fore, int Port, int *Accel);

// Main shared structures
struct ff_effect effects;	
struct input_event play, stop;
int fd_e,fd_j;
struct itimerval old;
struct itimerval new;
struct sockaddr_in si_other;
	
	 
// 20Hz timed task.  Put UDP Comms in here
//	
void TimedTask (int sig)
{
	static int x;
	
	//JS_Read(&Fore,&Port);
	//UDP_Send(Fore,Port,&Accel)
	
	// 10Hz task
	if(x % 2 == 0) {
	//FF_Rumble(Accel);
	;
	}
	
	signal (sig, TimedTask);
}//END Periodic

	
// Main control program for Teleop program. 
//	
int main(int argc, char** argv)
{
	
	char event_file_name[64];
	char joy_file_name[64];
	char ip_address[64];
	int x;
	

	/* Read args */
	if(argc != 4){
		printf("usage: teleop_client [event] [joystick] [server_IP]\n");
		printf("e.g.   sudo ./teleop_client /dev/input/event3  /dev/input/js0 192.168.1.6\n");
		exit(1);	
	}	
	strncpy(event_file_name, argv[1], 64);
	strncpy(joy_file_name, argv[2], 64);
	strncpy(ip_address, argv[2], 64);
	
	// Init timer task and joystick and UDP stack
	Task_Init();
	FF_Init(event_file_name);
	JS_Init(joy_file_name);
	UDP_Init(ip_address);
	
	// test area
	FF_Rumble(0x1000);
	UDP_Send(x,x,&x);
	
	JS_Read();

	// hand over to timed task
	while(1);
	
	close(s);
	sleep(2);
}//END main


// Send a UDP datagram to the server on the car.
//
void UDP_Send(int Fore, int Port, int *Accel)
{
 
	printf("Enter message : ");
	gets(message);
	 
	//send the message
	if (sendto(s, message, strlen(message) , 0 , (struct sockaddr *) &si_other, slen)==-1) 	{
		die("sendto()");
	}
	 
	//receive a reply and print it
	//clear the buffer by filling null, it might have previously received data
	memset(buf,'\0', BUFLEN);
	//try to receive some data, this is a blocking call
	if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1){
		die("recvfrom()");
	}
	 
	puts(buf);
     
   
}//END UDP_Send	



// Initialise UDP to talk to the client
//
UDP_Init(ip_address)
{   
    int s, i, slen=sizeof(si_other);
    char buf[BUFLEN];
    char message[BUFLEN];
 
    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        die("UDP_Init: socket");
    }
 
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
     
    if (inet_aton(ip_address , &si_other.sin_addr) == 0) {
		die("UDP_Init: aton");
    }
	
}//END UDP_Init



// Setup a 20 Hz Timer to poll the server on
// use POSIX timers
//
void Task_Init(void)
{
	signal (SIGALRM, TimedTask);
	new.it_interval.tv_sec = 0; 
	new.it_interval.tv_usec = 50000; //50ms = 20Hz = 50000us
	new.it_value.tv_sec = 0;
	new.it_value.tv_usec = 50000;
   
	old.it_interval.tv_sec = 0;
	old.it_interval.tv_usec = 0;
	old.it_value.tv_sec = 0;
	old.it_value.tv_usec = 0;
   
	if (setitimer (ITIMER_REAL, &new, &old) < 0) {
      die("Task_Init: timer init failed\n");
	}
}//END Task_Init


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
			die("\njstest: error reading");
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
		die("jstest");
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
		die("Open device file");
	}
	printf("Device %s opened\n", device_file_name);
	
	/* Query device */
	if (ioctl(fd_e, EVIOCGBIT(EV_FF, sizeof(unsigned long) * 4), features) < 0) {
		die("Ioctl query");
	}
	if (ioctl(fd_e, EVIOCGEFFECTS, &n_effects) < 0) {
		die("Ioctl number of effects");
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
		die("Upload effects[0]");
	}
	
	play.type = EV_FF;
	play.code = effects.id;
	play.value = 1;

	if (write(fd_e, (const void*) &play, sizeof(play)) == -1) {
		die("Play effect");
	}
}//END FF_Rumble
   
   
   
// Does what it says...
//   
void die(char *s)
{
    perror(s);
    exit(1);
    
}//END die
