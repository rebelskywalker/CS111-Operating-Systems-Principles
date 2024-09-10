#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <ctype.h>
#include <mraa.h>
#include <mraa/aio.h>
#include <string.h>
#include <poll.h>
//#include <signal.h>
//#include <sys/errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>


/*
MACROS
*/
#define ALG 1
#define SRVWRITE 1
#define SE 's'
#define LOG 'l'
#define PD 'p'
#define UID 'i'
#define HOST 'h'
#define BSIZE 1024
#define ASIZE 256


/* 
Globals 
*/
mraa_aio_context tsens; // set context for analog and general IO pins
//mraa_gpio_context btn; 

int failed = 0; int running = 1; //Determine if prgm runs and exit code
const int B  = 4275; const int R0 = 100000; // Thersistor and constant from Grove
FILE *ftext = 0;

char* host = "";
char* id = "";
char scale; int period; // global options
int dolog = 1;
int pnum = -1;
int srvRun = 0;

//Format message for using program
char fmsg[] =
  "\tProgram Proper Format Message\n"
  "\t./lab4c_tls : [OPTIONS]\n"
  "\tOptions: \n"
  "\t--scale=[F|C]    Determines temp scaling in F or C degrees\n"
  "\t--period=VALUE   Determines frequency of printing temp ever VALUE sec\n"
  "\t--host=NAME      Required host name given by NAME\n"
  "\t--id=DIGITS      Required 9 digit ID given DIGITS\n"
  "\t--log=NAME       Required file name to log given by NAME\n"
  "\tPORTNUM          Pnum number given by PORTNUM:expecting 180000 or 190000\n"
  "\tEnd Proper Format Message\n";

struct tm *curr_time;
struct timeval atime;
time_t next_time = 0;

struct hostent *server;
struct sockaddr_in srvLocation;
int sock;


/*
Protos
*/
void exitO(const char * msg); // Exits on program argument errors
float tCalc(); // Collects temperature from sensor pin
void newShutdown(); // Commences newShutdown procedure
void handleIn(char *input); // Handles inputs
void printScale(char *str, int srv, char c); // Prints inputs and determines scale/newShutdown
void printInput(char *str, int swrite, int r); // Prints inputs and determines log of time
float fcalc(float c); // Determines F temp values from C
void exitE(const char * msg); // exit function with signal 1 errors
void parseOpts(int argc, char** argv); // Parses options
int createPins(); // Creates the intialization of pins
void closePins(); // Closes the pins from GROVE tutorial
void checkErrors(); // Checks errors for input flags
void handleSrv(char* input); // Handles server



/*
Functions
*/

int main(int argc, char* argv[]) {
        char *input;
	//Allocate a buffer size to contain up to BSIZE chars
	input = (char *)malloc(BSIZE * sizeof(char));
	char out[0x32]; // Sets an output buffer for logging

	//Throw failure if error occurs in allocation
	if(input == NULL) { exitE("Unable to set input buffer"); }
	
	//Run parse opts to set up longopts
	parseOpts(argc, argv);
	
	//Handles Error Checking to make sure all required arguments are input
	checkErrors();

	//create a socket and find host
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		exitE("Failed creating socket and finding host");
	}

	//Retrieve host through get by hostname
	if ((server = gethostbyname(host)) == NULL) {
		fprintf(stderr, "Could not find host\n");
	}

	//Handles setting of socket struct through youtube video tutorial
	//See sources
	memset( (void *) &srvLocation, 0, sizeof(srvLocation));
	srvLocation.sin_family = AF_INET;

	memcpy( (char *) &srvLocation.sin_addr.s_addr, 
		    (char *) server->h_addr, 
		    server->h_length);

	srvLocation.sin_port = htons(pnum);

	//Creates a check variable to check for socket success or failure
	int checkCon;
	checkCon = connect(sock, (struct sockaddr *) &srvLocation, sizeof(srvLocation));
	if (checkCon  < 0) { exitE("Failed connecting socket"); }
	
	//https://www.tutorialspoint.com/c_standard_library/c_function_sprintf.htm
	//End logs with newline termination
	sprintf(out, "ID=%s", id);
	
	//Handle logging and communication with server for temp logs
	printInput(out, SRVWRITE, 2);
	
	//Handles analog (ALG) pin creation
	createPins();

	//Sets the file descriptor to read from socket input
	//Sets events to read high prio data without blocking
	struct pollfd pollInput; 
	pollInput.fd = sock; 
	pollInput.events = POLLIN; 


	if(input == NULL) { exitE("Failed allocating buffer"); }

	//Loop while the program is running will poll the input
	//and print it from the data stream of standard input
	while(running) {
	  char out[200]; int tscaled; float temp;
		gettimeofday(&atime, 0);
		if (dolog && atime.tv_sec >= next_time) {
			temp = tCalc();
			tscaled = temp * 10;
			//Sets the current time to local time in seconds
			curr_time = localtime(&atime.tv_sec);
			if(curr_time == NULL)
			{
				exitE("Failed to retrieve time");
			}
			//Add period value to initial every run
			sprintf(out, "%02d:%02d:%02d %d.%1d", curr_time->tm_hour, 
				curr_time->tm_min, curr_time->tm_sec, tscaled/10, tscaled%10);
			printInput(out, SRVWRITE, 2);
			next_time = atime.tv_sec + period; 
		}

		int ret = poll(&pollInput, 1, 0);
		if(ret) {
		  //char* fgets (char* string, int num, file *stream)
		  //pointer to array of chars, size to be read, stream
		  //fgets (input, BSIZE, stdin)
			handleSrv(input);
		}
	}
	//Test if program stops running prematurely
	running = 0;
	//if(!running) 
	//fprintf(stdout, "running became zero");

	closePins();
	return failed;
}


/*
Handles exits with signal 1
Provides reason
Closes the file system if opened
*/
void exitE(const char * msg)
{
  fprintf(stderr, "\nError: %s\n", msg);
  failed = 1;
  exit(1);
}

/*
Handles exits for options
Provides reason
Closes the file system if opened
*/
void exitO(const char * msg)
{
  fprintf(stderr, "\nError: %s\n", msg);
  fprintf(stderr, fmsg);
  failed = 1;
  exit(1);
}


/*
Obtains temperature then decides if F or C
*/
float tCalc()
{
  float result = 0;

  //inital analog reading and convert to float
  //int mraa_aio_read(mraa_aio context dev) returns input voltage if success
  int itemp = mraa_aio_read(tsens);

  //Maybe check if the value is 0 and return? or Null?
  //Decided this was a bad idea, what if the C or F reading was meant to be 0 !
  float a = (float)itemp;
  float R = 1023.0/a - 1.0;
  R = R*R0;
  float fval = 1.0/(log(R/R0)/B + 1/298.15) - 273.15;

  //If the scale if f, then we will run a convert to farenheit function
  if(scale == 'F')
  {
	result = fcalc(fval);
  }
  else
  {
	result = fval;
  }
  return result;
}

/*
Performs calculation from C
*/
float fcalc(float c)
{
  float z = ((c*9)/5 + 32);
  return z;
}


/* 
Prints the input and determines logging
*/
void printInput(char *str, int swrite, int r) {
	//Checks if server is running and handle print for socket
	if (swrite) {
		dprintf(sock, "%s\n", str);
		srvRun = 1;
	}
	else
	{
		srvRun = 0;
	}
	fprintf(stderr, "%s\n", str);
	fprintf(ftext, "%s\n", str);
	fflush(ftext);
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


/* shuts down, prints shut down time */
void newShutdown(){
	curr_time = localtime(&atime.tv_sec);
	char out[200];
	sprintf(out, "%02d:%02d:%02d SHUTDOWN", curr_time->tm_hour, 
    		curr_time->tm_min, curr_time->tm_sec);

	
	//Checks if server is running and handle print for socket
	if (SRVWRITE) {
		dprintf(sock, "%s\n", out);
		srvRun = 1;
	}
	else
	{
		srvRun = 0;
	}
	fprintf(stderr, "%s\n", out);
	fprintf(ftext, "%s\n", out);
	fflush(ftext);

   	exit(0);
}

void printScale(char *str, int srv, char c)
{
	if (srv) {
		dprintf(sock, "%s\n", str);
	}
	fprintf(stderr, "%s\n", str);
	fprintf(ftext, "%s\n", str);
	fflush(ftext);
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
	  newShutdown();
	}
}

/* reads input from the server */
void handleSrv(char* input) {
	int ret = read(sock, input, ASIZE);
	if (ret > 0) {
		input[ret] = 0;
	}
	char *s = input;
	while (s < &input[ret]) {
		char* e = s;
		while (e < &input[ret] && *e != '\n') {
			e++;
		}
		*e = 0;
		handleIn(s);
		s = &e[1];
	}
}

void parseOpts(int argc, char** argv)
{
	scale = 'F';
	period = 1;
	int o;
	unsigned digs;
	struct option ops[] = {
		{"period", required_argument, NULL, PD},
		{"log", required_argument, NULL, LOG},
		{"id", required_argument, NULL, UID},
   		{"scale", required_argument, NULL, SE},
		{"host", required_argument, NULL, HOST},
		{0, 0, 0, 0}
	};
	while ((o = getopt_long(argc, argv, "", ops, NULL)) != -1) {
		switch (o) {
			case PD:
 				for(digs=0; digs<strlen(optarg); digs++)
				{
				  if(isdigit(optarg[digs]) == 0)
				  {
				    exitO("Entered non digit value arg\n");
				  }
				}
				period = atoi(optarg);
				break;
			case LOG:
				ftext = fopen(optarg, "w+");
            			if (ftext == NULL) {
				  perror("Error opening file");
				}
				break;
			case SE:
				if (strlen(optarg) > 1)
				{
				  exitO("Used more then 1 arg for scale");
				}
				if (optarg[0] == 'F') {
					scale = optarg[0];
				}
				else if (optarg[0] == 'C') {
					scale = optarg[0];
				}
				else {
					exitE("Unrecognized scale arg");
				}
				break;
			case UID:
				id = optarg;
				break;
			case HOST:
				host = optarg;
				break;
			default:
				fprintf(stderr, "Bad Arguments\n");
				exitO("Incorrect use of options, see format");
				break;
		}
	}
	if (optind < argc) {
		pnum = atoi(argv[optind]);
		if (pnum <= 0) {
			fprintf(stderr, "Invalid port num\n");
			exit(1);
		}
	}
}


/*
Handles input from stdin and decides which options are logged
*/
void handleIn(char *input) {
	while(*input == ' ' || *input == '\t') {
		input++;
	}

	//http://www.cplusplus.com/reference/cstring/strstr/
	char *in_per = strstr(input, "PERIOD=");
	char *in_log = strstr(input, "LOG");

	if(strcmp(input, "SCALE=F") == 0) {
		printScale(input, 0, 'F');
	} else if(strcmp(input, "SCALE=C") == 0) {
		printScale(input, 0, 'C');
	} else if(strcmp(input, "STOP") == 0) {

		printInput(input, 0, 0);
	} else if(strcmp(input, "START") == 0) {

		printInput(input, 0, 1);
	} else if(strcmp(input, "OFF") == 0) {
		printScale(input, 0, 'd');
	} else if(in_per == input) {
		char *n = input;
		n = n + 0x7; 
		if(*n != 0) {
			//Convert the period character into integer
			int p = atoi(n);
			//Check again if the character is a digit
			while(*n != 0) {
				if (!isdigit(*n)) {
					return;
				}
				n++;
			}
			//Set period equal to the period integer
			period = p;
		}
		printInput(input, 0, 2);
	} else if (in_log == input) {
		printInput(input, 0, 2);
	}
}



void checkErrors()
{
  if ( !(strlen(host) | !ftext | (strlen(id) != 9)) )
	exitE("Must provide argument for host, file, and 9 digit UID");
}


/*
Creates the pins: initialize, direct, interrupt and checks errors
http://iotdk.intel.com/docs/mraa/v1.7.0/aio_8h.html#ada196b58ef32f702f5ddb32a7c612fbc
https://navinbhaskar.wordpress.com/2016/07/05/cc-on-intel-edisongalileo-part5temperature-sensor/
*/
int createPins()
{
	tsens = mraa_aio_init(ALG);
	if (tsens== NULL) {
		fprintf(stderr, "Failed to initialize AIO\n");
		mraa_deinit();
		return EXIT_FAILURE;
    	}

	/*
    	if (button == NULL) {
        	fprintf(stderr, "Failed to initialize GPIO_50\n");
        	mraa_deinit();
        	return EXIT_FAILURE;
    	}
	
	//mraa_gpio_isr(mode, function pointer when interrupt trigger, args)
	//args to interrupt handler of function pointer
    	mraa_gpio_dir(button, MRAA_GPIO_IN);
    	mraa_gpio_isr(button, MRAA_GPIO_EDGE_RISING, &newShutdown, NULL);
	*/
	return 0;
}

//Closes the pin sensors
void closePins()
{
	mraa_aio_close(tsens);
	//mraa_gpio_close(btn);
}
