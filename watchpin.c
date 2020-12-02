/* xtforever 2020/11/29
   use wiringPi to detect pin change interrupt on gpio 
   call a program or shell script on rising or falling edge
   tested on orangepi zero 

   gcc -lwiringPi watchpin.c -o watchpin
*/

#include <stdio.h>
#include <sys/time.h>
#include <wiringPi.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>

char *help="\n"
  "-pin\n"
  "-p    <pin>	- wiringPi pin number to watch\n"
  "\n"
  "-script\n"
  "-s <script>	- which script to execute on edge detect\n"
  "\n"
  "-r		- exec script on rising edge (instead of falling)\n"
  "example:\n"
  "watchpin --pin=14 --script='/bin/echo hello world' \n"
  "\n"
  "wait for falling edge on pin 14  and  execute /bin/echo\n"
  "\n"
  "rewritten by xtforever 2020/11/29 original author unknown\n"
"";

// Which GPIO pin we're using
int button_pin = 14;

// execute on falling edge
int button_falling = 1;

// Which command shall be called 
char *button_script = NULL;

// How much time a change must be since the last in order to count as a change
#define IGNORE_CHANGE_BELOW_USEC 10000

// Current state of the pin
static volatile int state;

// Time of last change
struct timeval last_change;

void execute_command(void)
{
  if( button_script ) system(button_script);
}

// Handler for interrupt
void handle(void) {
	struct timeval now;
	unsigned long diff;
	int next_state;
	
	gettimeofday(&now, NULL);
	
	next_state = digitalRead(button_pin);

	// Time difference in usec
	diff = (now.tv_sec * 1000000 + now.tv_usec) - (last_change.tv_sec * 1000000 + last_change.tv_usec);
	last_change = now;
	// Filter jitter
	if (diff < IGNORE_CHANGE_BELOW_USEC) goto leave_isr;

	if (next_state) {
	  printf("Rising\n");
	}
	else {
	  printf("Falling\n");
	}
	
	if( next_state != button_falling ) execute_command();

 leave_isr:
	state = next_state;
}

static struct option long_options[] =
{
    {"pin", required_argument, NULL, 'p'},
    {"script", required_argument, NULL, 's'},
    {"rising", no_argument, NULL, 'r' },
    {NULL, 0, NULL, 0}
};

void check_opts(int argc, char **argv)
{
  int ch;
  // loop over all of the options
  while ((ch = getopt_long(argc, argv, "rp:s:", long_options, NULL)) != -1)
    {
      // check to see if a single character or long option came through
      switch (ch)
	{
	case 'p':
	  button_pin = atoi(optarg); 
	  break;
	case 's':
	  button_script = optarg;
	  break;
	case 'r':
	  button_falling = 0;
	  break;
	default:
	  printf("Usage: %s %s\n", argv[0], help, argv[0] );
	  exit(1);	  
	}
    }
}

int main(int argc, char **argv) {
	// Init
	wiringPiSetup();
	check_opts(argc,argv);
	// Set pin to output in case it's not
	pinMode(button_pin, OUTPUT);
	pullUpDnControl (button_pin, PUD_DOWN);
	// Time now
	gettimeofday(&last_change, NULL);

	// Bind to interrupt
	wiringPiISR(button_pin, INT_EDGE_BOTH, &handle);

	// Get initial state of pin
	state = digitalRead(button_pin);

	printf("Started watching pin %d. Initial state is %s\n",
	       button_pin,
	       state ? "ON" : "OFF" 
	       );	
	
	// Waste time but not CPU
	for (;;) {
		sleep(1);
	}
}
