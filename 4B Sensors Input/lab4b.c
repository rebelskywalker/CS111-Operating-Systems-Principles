#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <ctype.h>
#include <mraa.h>
#include <string.h>
#include <poll.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/errno.h>

/*
Macros
*/
#define ALG 1
#define SE 's'
#define LOG 'l'
#define PD 'p'
#define BSIZE 1024
#define GP50 60


/*
Globals
*/
mraa_aio_context tsens; // set context for analog and general IO pins
mraa_gpio_context btn; 

int failed = 0; int running = 1;  //Determine if prgm runs and exit code
const int b = 4275; const int r0 = 100000; // Thersistor and constant From Grove
int fileIsZero = 0; // Determines if file is zer0
FILE *ftext = 0; //Holds pointer to File , used with fopen()
int dolog = 1; // Determines if should log time
char scale; int period; //
char ostrm [240]; // Creates character buffer for holding input 

//Format message for using program
char fmsg[] =
  "\tProgram Proper Format Message\n"
  "\t./lab4b : [OPTS]"
  "C program to obtain analog signals for temp and\n\t\t print out ambient temp\n"
  "\tOptions: \n"
  "\t--scale=[F|C]: Determines scale to be F or C degrees\n"
  "\t--period=VALUE: Determines frequency of printing temp every VALUE seconds\n"
  "\tEnd Proper Format Message\n\n";

struct tm *curr_time;
struct timeval atime;
time_t next_time = 0;

/*
Function Prototypes
*/
void exitO(const char * msg); // Exits on program argument errors
float tCalc(); // Collects temperature from sensor pin
void shutdown(); // Commences shutdown procedure
void handleIn(char *input); // Handles inputs
void printScale(char *str, char c); //Prints inputs, and determines scale/shutdown
void printInput(char *str, int r); //Prints input and deterimines log of time
float fcalc(float c); //Determines F temp values from C
void exitE(const char * rsn); //exit function with signal 1 errors
void parseOpts(int argc, char** argv); // Parses options
int createPins(); // Creates the initialization of pints, directions, interr.
void closePins(); // Closes the pints for temp sensor and button from GROVE

int main(int argc, char* argv[]) {
	char *input;
	//Allocate a buffer size to contain a limited amount of characters
    	input = (char *)malloc(BSIZE * sizeof(char));

	//Throw failure if error occurs in allocation
    	if(input == NULL) { exitE("Unable to set the input buffer");}

	//Run parse opts to set up longopts
	parseOpts(argc, argv);
 
	//Create pins for Grove Temperature Sensor and Button	
	createPins();
	
	//Sets the file descriptor to read standar input
	//Sets the events to read high prio data without blocking
	struct pollfd pollInput; 
    		pollInput.fd = STDIN_FILENO; 
    		pollInput.events = POLLIN; 
	
	//Loop that while the program is running will poll the input and 
	//and print it from the data stream of standard input
    	while(running) {
		gettimeofday(&atime, 0);
		if (dolog && atime.tv_sec >= next_time) 
		{
			float temp = tCalc();
			int t = temp * 10;

			//Sets current time to the local time in seconds
			curr_time = localtime(&atime.tv_sec);
    			sprintf(ostrm, "%02d:%02d:%02d %d.%1d", curr_time->tm_hour, 
    				curr_time->tm_min, curr_time->tm_sec, t/10, t%10);
 
			fprintf(stdout, "%s\n", ostrm);
			if (ftext != 0) 
			{
		 		 fprintf(ftext, "%s\n", ostrm);
		  		fflush(ftext);
			}
			//Adds the period value to the initial time every run
    			next_time = atime.tv_sec + period; 
		}	

    		int ret = poll(&pollInput, 1, 0);
    		if(ret) {
		  //char* fgets (char* string, int num, file *stream)
		  //pointer to array of chars, size to be read, stream
    			fgets(input, BSIZE, stdin);
    			handleIn(input); 
    		}
	}

	//Test if program stops running prematurely
	running = 0;
	//fprintf(stdout, "running became zero");

	
	//Close pins for Grove Temperature Sensor and Button
	closePins();
    	return failed; //0 if success, 1 if failed occurred
}


/*
Obtains temperature then decides if F or C
https://iotdk.intel.com/docs/master/mraa/aio_8h.html
*/
float tCalc() {
  float result = 0;
  //inital analog reading and convert to float
  //int mraa_aio_read(mraa_aio context dev) returns input voltage if success
  int itemp = mraa_aio_read(tsens);
  float a = (float)itemp;

  //Maybe check if the value is 0 and return? or Null?
  //Decided this was a bad idea, what if the C or F reading was meant to be 0 !

  //Following comes from Grove Website Tutorial and Youtube
  float R = 1023.0/a - 1.0;
  R = R* r0;
  float fval = 1.0/(log(R/r0)/b + 1/298.15) - 273.15; 
  //If the scale if f, then we will run a convert to farenheit function
  if (scale == 'F')
    { 
      result = fcalc(fval); 
    }
  //Else we return the result as Celsius value
  else
    {
      result = fval;
    }
  return result;
}

/*
Performs F calcultation from C
*/
float fcalc(float c)
{
  float z = ((c*9)/5 + 32); 
  return z;
}

/*
Handle exits with signal 1
Provides reason
Closes the file system if opened
*/
void exitE(const char * rsn){
  fprintf(stderr, "\nError: %s\n", rsn);
  failed = 1;
  exit(1); 
}

void printInput(char *str, int r) {
	if (ftext != 0) {
		fprintf(ftext, "%s\n", str);
		fflush(ftext);
	}
	else
	{  
		fileIsZero = 1;
	}
	if (r == 0)
	{
		dolog = 0;
	}
	else if (r == 1)
	{
		dolog = 1;
	}
	else if (r == 2)
	{
		return;
	}
}

/*
Creates the pins: initialize, direct, interrupt and checks errors
http://iotdk.intel.com/docs/mraa/v1.7.0/aio_8h.html#ada196b58ef32f702f5ddb32a7c612fbc
https://navinbhaskar.wordpress.com/2016/07/05/cc-on-intel-edisongalileo-part5temperature-sensor/
*/
int createPins()
{
	//Create variables to hold the initialize analog and Gen Purp IO
	tsens = mraa_aio_init(ALG);
    	btn = mraa_gpio_init(GP50);

	//Check if there was an error in either
	if (tsens == NULL || btn == NULL) {
        	fprintf(stderr, "Failed analog or discrete in/out init\n");
        	mraa_deinit();
        	return EXIT_FAILURE;
    	}
	//Create a result variable to check if MRAA SUCCESS occurs in both
	mraa_result_t status;
	
	//Direct input of gen purp IO buttong
	status = mraa_gpio_dir(btn, MRAA_GPIO_IN);
	if(status != MRAA_SUCCESS)
	{
	  fprintf(stderr, "Failed Settig up btn");
	}

	//mraa_gpio_isr(mode, function pointer when interrupt trigger, args)
	//args to interrupt handler of function pointer
	status = mraa_gpio_isr(btn, MRAA_GPIO_EDGE_RISING, &shutdown, NULL);
	if(status != MRAA_SUCCESS)
	{
	  fprintf(stderr, "Failed Settig up interrupt");
	}
	return 0;

}

/*
Handles printing the file text string as well
as the setting of scales.
*/
void printScale(char *str, char c) {
	if (ftext != 0) {
		fprintf(ftext, "%s\n", str);
		fflush(ftext);
	}
	else
	{  
		fileIsZero = 1;
	}
	if (c == 'F')
	{
		scale = 'F';
	}
	else if (c == 'C')
	{
		scale = 'C';
	}
	else if (c == 'd')
	{
		shutdown();
	}
}

/*
Initates the proper shutdown procedure for temp sensor
program.
*/
void shutdown() {
	char tstr[200];
	curr_time = localtime(&atime.tv_sec);
	if(curr_time == NULL)
	{
	  exitE("Local time function failed to get curr time");
	}
	sprintf(tstr, "%02d:%02d:%02d SHUTDOWN", curr_time->tm_hour, 
    		curr_time->tm_min, curr_time->tm_sec);
 
	fprintf(stdout, "%s\n", tstr);
	if (ftext != 0) {
		fprintf(ftext, "%s\n", tstr);
		fflush(ftext);
	}
    	exit(0);
}

/*
Handle option argument errors and display proper format message
*/
void exitO(const char * msg){
  fprintf(stderr, "\nError: %s\n", msg);
  fprintf(stderr, fmsg);
  failed = 1;
  exit(1);
}

/*
Handles Creation of options
*/
void parseOpts(int argc, char** argv)
{
  scale = 'F';
  period = 1;
  int o; 
  //char a;
  unsigned digs;

  struct option ops[] = {
			 {"period", required_argument, NULL, PD},
			 {"scale", required_argument, NULL, SE},
			 {"log", required_argument, NULL, LOG},
			 {0, 0, 0, 0}
  };
  while ((o = getopt_long(argc, argv, "", ops, NULL)) != -1) {
    switch (o) {      
    case PD:
      //for each period argument, check if it is a digit value
      for(digs = 0; digs < strlen(optarg); digs++)
        {
	  //a = getch(optarg[digs]); 
	  if(isdigit(optarg[digs]) == 0)  //&& (a> 9 || a<0))
	  {
	    //period = atoi(optarg);
	    exitO("Entered a non digit value as period arg\n");
	 }
        }

      period = atoi(optarg);
      break;
    case LOG:

      //Opens a file for writing using optarg as the file name.
      // Originally used a+ but this appended, needed w+ to rewrite the file
      // fresh each time
      ftext = fopen(optarg, "w+");
      if (ftext == NULL)
	{
	  perror("error opening file");
	}
      break;
    case SE:
      if(strlen(optarg) > 1)
	{
	  exitO("Used more then 1 arg for scale, please use one");
	}

      if (optarg[0] == 'F')
	{
	  scale = optarg[0];
	}
      else if (optarg[0] == 'C')
	{
	  scale = optarg[0];
	}
      else
	{
	  exitE("uncrecognized scale argument");
	}
      break;
    default:
      fprintf(stderr, "Bad arguments\n");
      exitO("Incorrect use of options, see format and retry");
      break;
    }
  }
}


/*
Handles input from stdin and decides which options are logged
*/
void handleIn(char *in) {
	//Collect number of inputs
	int numIn = strlen(in); 
	//Store the last input at EOF
	in[numIn-1] = '\0';

	while(*in == '\t' || *in == ' ') {
		in = in + 1;
	}
	//http://www.cplusplus.com/reference/cstring/strstr/
	char *isper = strstr(in, "PERIOD=");
	char *islog = strstr(in, "LOG");

	if(strcmp(in, "SCALE=F") == 0) 
	{
		printScale(in, 'F');
	} 
	else if(strcmp(in, "SCALE=C") == 0) 
	{
		printScale(in, 'C');
	} 
	else if(strcmp(in, "STOP") == 0) 
	{
		printInput(in, 0);
	} 
	else if(strcmp(in, "START") == 0) 
	{
		printInput(in, 1);
	} 
	else if(strcmp(in, "OFF") == 0) 
	{
		printScale(in, 'd');
	} 
	else if(isper == in) 
	{	
		//Set point to the input
		char *n = in;
		//Go past the characters in preiod= to collect the integer
		n = n + 0x7; 
		if(*n != 0) {
			//Conver the period character into integer
			int pint = atoi(n);
			//Check again if the character is a digit
			while(*n != 0) {
				if (!isdigit(*n)) 
				{
					return;
				}
				n++;
			}
			//Set period equal to the period integer
			period = pint;
		}
		printInput(in, 2);
	} 
	else if (islog == in)
	{
		printInput(in, 2); 
	}
}


//Closes the pint sensors
void closePins()
{
	mraa_aio_close(tsens);
    	mraa_gpio_close(btn);
}

