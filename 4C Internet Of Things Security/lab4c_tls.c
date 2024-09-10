#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <signal.h>
#include <time.h>
#include <mraa.h>
#include <math.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <sys/types.h>

/*Macros*/
#define ALG 1
#define SE 's'
#define LOG 'l'
#define PD 'p'
#define UID 'i'
#define HOST 'h'
#define BSIZE 1024
#define GP50 60

/*Globals*/
mraa_aio_context sensor; //Set Context for analog and general IO pins
//mraa_gpio_context btn; 

int failed = 0; //Determine exit code
const int B  = 4275; const int R0 = 100000; // Thersistor and constant from Grove

//For Options Default Values
int period = 1;
int scale = 0;
int portNum = 0;

//Option Flags
int uidFlag = 0;
int hFlag = 0;
int pFlag = 0;
int lFlag = 0; //use this as flag for logging

//Other Flags
int isRunning = 1; 
int socket_flag = 0; 
int stop = 0; 
int gatherFlag = 0;
int sslFlag = 0;

//File Desc
int log_fd; 
int sockfd = -1; 

//Other
char id[10];
char host[BSIZE]; // use this to record host address
SSL *ssl; //ssl pointer
int rcount; // Performance checker for number of reports
int tempSensing = 0;

char fmsg[] =
  "\tProgram Proper Format Message\n"
  "\t./lab4c_tls : [OPTIONS]\n"
  "\tOptions: \n"
  "\t--scale=[F|C]    Determines temp scaling in F or C degrees\n"
  "\t--period=VALUE   Determines frequency of printing temp ever VALUE sec\n"
  "\t--host=NAME      Required host name given by NAME\n"
  "\t--id=DIGITS      Required 9 digit ID given DIGITS\n"
  "\t--log=NAME       Required file name to log given by NAME\n"
  "\tPORTNUM          Port number given by PORTNUM:expesction 180000 or 190000\n"
  "\tEnd Proper Format Message\n";

/*Protos*/
float fCalc(float c);
void grabTime(char* str, time_t* rTime, struct tm* timeData);
void report(char* str, float temp, time_t* rTime, struct tm* timeData);
float tCalc();
void flagSetting();
void parseCmd(char * str);
int createPins();
void exitChecks();
void tlsCreation();
void parseOpts(int argc, char** argv);
void exitS(int status);
void exitF(const char * msg, int status);
void exitE(const char * msg);
void exitO(const char * msg);
void closeAll();


/*Functions*/

/*
Handles creation of options and parses them
while performing error checks
*/
void parseOpts(int argc, char** argv)
{
  int o = 0;
static struct option ops[] = { //get opts and handle
    {"scale", required_argument, 0, SE},
    {"period", required_argument, 0, PD},
    {"log", required_argument, 0, LOG},
    {"id", required_argument, 0, UID},
    {"host", required_argument, 0, HOST},
    {0, 0, 0, 0}
  };

  while ( (o =  getopt_long(argc, argv, "s:p:l:i:h:", ops, NULL)) != -1) {
    unsigned int i;
    switch(o) {
    case SE:
      if (strlen(optarg) > 1) 
      { 
        exitO("Only one argument allowed for scale arg");
      }
      if (optarg[0] == 'C') { scale = 1; } //Celsius 
      else if (optarg[0] == 'F') { scale = 0; } // Farenheit
      else { exitO("Only C or F allowed for scale arg "); } //Can't detect
      break;
    case PD:
      for (i = 0; i < strlen(optarg); i++) { 
      	if (! isdigit(optarg[i])) { exitO("Must use 9 digit ID"); }
      }
      period = atoi(optarg); //assign optarg to period
      break;
    case LOG: //log
      log_fd = open(optarg, O_CREAT|O_APPEND|O_RDWR, 0666); 
      if (log_fd < 0) {
		exitO("Issue opening file to log");
	    }
      lFlag = 1;
      break;
    case UID: //id
      if (strlen(optarg) != 9) { exitE("Must use a 9 digit ID"); }
      strcpy(id, optarg); //copy optarg into id
      uidFlag = 1;
      break;
    case HOST:
      if (strlen(optarg) > (BSIZE - 1)) 
	{
	  exitE("Please use a smaller host name");
        }
      strcpy(host, optarg);
      hFlag = 1;
      break;
    case '?': //bad arg
	exitO("Incorrect options/args: please see format");
      break;
    }
  }
  if (optind < argc) //report extra arguments
  {
    int i;
    for (i = optind; i < argc; i++) {
        if (i != optind)
          fprintf(stderr, "invalid argument: %s\n", argv[i]);
        else {//get port number
          portNum = atoi(argv[i]);
          pFlag = 1;
        }
    }
    if ((i-optind) > 1) {
	exitO("See proper format");
    }
  }
}

/*
Performs F calculation from C
*/ 
float fCalc(float c)
{
  float z = ((c*9)/5 + 32);
  return z;
}

/*
Obtains temperature then decides if F or C
http://iotdk.intel.com/docs/master/mraa/aio_8h.html
See Grove Tutorial video for calculation from lab 4b
*/
float tCalc()
{
  float result = 0;
  int itemp;
  float R; float cVal;
  itemp = mraa_aio_read(sensor); 
/*
  if (sensor_val == -1) {
	exitF("Failed reading sensor", 2);
  }
*/
  float a = (float)itemp;
  R = 1023.0/a - 1.0;
  R = R*R0;
  cVal = 1.0/(log(R/R0)/B+1/298.15)-273.15;
  if (!scale) { //Convert to Far
    result = fCalc(cVal);
  }
  else 
  {
    result = cVal;
  }
  return result;
}


int main(int argc, char** argv) {
  int i, m, n;
  int fd;

  char output[9];
  //Buffers to read in stream and to copy stream
  char buf[BSIZE] = {0};
  char cpBuf[BSIZE];
  float temperature;

  parseOpts(argc, argv);
  if (! (uidFlag && hFlag && lFlag && pFlag)) {
	exitO("Must set ID, Host, Log, and Port: see proper format");
  }
  
  //Create analog and general purpose pins
  createPins();

  //Handle TLS
  tlsCreation();

  //Collect time information
  //https://en.wikipedia.org/wiki/C_date_and_time_functions
  //https://www.geeksforgeeks.org/time-function-in-c/
  time_t time_start, time_now; 
  struct tm* timeData = NULL;

/*
https://www.tutorialspoint.com/unix_system_calls/poll.htm

The field fd contains a file descriptor for an open file. 
The field events is an input parameter, 
a bitmask specifying the events the application is interested in. 
*/
  //Set the file descriptor to poll for input and to act on data to be read
  struct pollfd fds[1];
  fds[0].fd = STDIN_FILENO;
  fds[0].events = POLLIN;


  //algo from grove temperature sensor
  time(&time_start);
  temperature = tCalc();

  //
  report(output, temperature, &time_start, timeData);
  gatherFlag = 1;


  while (isRunning) { 
    time(&time_now);

    if (difftime(time_now, time_start) >= period && stop == 0) {
	temperature = tCalc();
      report(output, temperature, &time_now, timeData);
      time_start = time_now;
    }
    
    m = poll(fds, 1, 0); //poll on a loop
    if (m < 0) { exitF("Failed polling", 2); }
    if (fds[0].revents & POLLIN) { //there is input waiting from stdin
      int length = SSL_read(ssl, buf, BSIZE); //read in from stdin
      if (length < 0) {
	exitF("Failed to read from standard input", 2);
      }
      if (lFlag) //write commands to log or stdout
	fd = log_fd;
      else
	fd = 1;
      //start at beginning of buf
      for (i = 0; i < length && n < BSIZE; i++) {
	if (buf[i] == '\n') {
	  cpBuf[n] = '\n'; 
	  if (lFlag) { //write to logfile if needed
	    m = dprintf(fd, cpBuf, n+1);
	    if (m < 0) { exitF("Failed to log file", 2); }
	  }
	  cpBuf[n] = '\0'; //now make the cpBuf null terminated
	  parseCmd(cpBuf);
	  memset(cpBuf, 0, n); //make cpBuf blank
	  n = 0;
	}
	else {
	  cpBuf[n] = buf[i]; //otherwise, just copy over
	  n++; //increment j
	}
      }      
      if (n == BSIZE) { //limit on buf reached
	exitF("Failed due to buffer overflow", 2);
      }
    }
  }
	exitS(0);
	return failed;
}


void grabTime(char *str, time_t* rTime, struct tm* timeData) {
  timeData = localtime(rTime);
  char* temp = asctime(timeData);
  temp = temp+11; //make the string only be the time
  temp[8] = '\0'; //null terminate at the end of the time
  strcpy(str, temp);
}

void exitS(int status)
{
  if (status == 0)
  {
    failed = 0;   
  }
  exitChecks();
  exit(failed);
}

void exitE(const char * msg)
{
  fprintf(stderr, "\nError: %s\n", msg);
  failed = 1;
  exit(1);
}

void exitO(const char * msg)
{
  fprintf(stderr, "\nError: %s\n", msg);
  fprintf(stderr, fmsg);
  failed = 1;
  exit(1);
}


void exitF(const char * msg, int status)
{
  fprintf(stderr, "\nError: %s\n", msg);
  if (status == 1)
  {
    failed = 1;   
  }
  else
  {
    failed = 2;   
  }
  exitChecks();
  exit(failed);
}


void report(char* str, float temp, time_t* rTime, struct tm* timeData) {
  grabTime(str, rTime, timeData);
  char report_msg[100];
  int length = sprintf(report_msg, "%s %0.1f\n", str, temp);
  SSL_write(ssl, report_msg, length); //print to stdout
  if (lFlag) { dprintf(log_fd, "%s %0.1f\n", str, temp); }
  rcount = rcount + 1;
}


void parseCmd(char * str) {
  if (strcmp(str, "SCALE=F") == 0) {
    scale = 0; //set scale to F
  }
  else if (strcmp(str, "SCALE=C") == 0) {
    scale = 1; //set scale to C
  }
  else if (strcmp(str, "STOP") == 0) {
    stop = 1; 
  }
  else if (strcmp(str, "START") == 0) {
    stop = 0; 
  }
  else if (strcmp(str, "OFF") == 0) {
	exitS(0);
  }
  else if (strncmp(str, "PERIOD=", 7) == 0) {
    period = atoi(str+7);
    if (period < 0) {
	exitF("Bad argument for Period", 2);
    }
  }
  else if (strncmp(str, "LOG ", 4) == 0) {
    return; 
  }
  else { //bad command
	exitF("Bad command used", 2);
  }
}
/*
Handles pin creation for analog or gen purppose
//http://iotdk.intel.com/docs/master/mraa/aio_8h.html
*/
int createPins()
{
//init sensor and set flags
  sensor = mraa_aio_init(ALG); 
  if (sensor == NULL) {
	mraa_deinit();
	exitF("Failed sensor init", 2);
  }
  tempSensing = 1;
  /*
  btn = mraa_gpio_init(GP50);
  if (btn == NULL) {
	mraa_deinit();
	exitF("Failed sensor init", 2);
  }
  mraa_result_t status
  status = mraa_gpio_dir(btn_MRAA_GPIO_IN);
  if(status != MRAA_SUCCESS) {fprintf(stderr, "Failed setting up btn");}
  */

  return 0;
}


void exitChecks() {
  char output[9]; time_t time_now; char output_buf[100];
  struct tm* timeData = NULL;
  time(&time_now); //get time
  grabTime(output, &time_now, timeData);
  if (gatherFlag) {
    int length = sprintf(output_buf, "%s SHUTDOWN\n", output);
    SSL_write(ssl, output_buf, length);
  }
  if (lFlag)
    dprintf(log_fd, "%s SHUTDOWN\n", output);
  closeAll();

  //https://wiki.openssl.org/index.php/Simple_TLS_Server
  if (sslFlag) {
    SSL_shutdown(ssl);
    SSL_free(ssl);
  }
}


/*
Checks if all flags have been set, if so it closes them
If not then it performs multiple checks to close appropriately
*/
void closeAll()
{
  if(lFlag & log_fd & socket_flag)
  {
    close(log_fd); mraa_aio_close(sensor); close(sockfd);
  }
  else
  {
    if (lFlag)
      close(log_fd);
    if (tempSensing)
      mraa_aio_close(sensor);
    if (socket_flag)
      close(sockfd);
  }
}

//https://www.openssl.org/docs/man1.0.2/man3/SSL_write.html
//https://wiki.openssl.org/index.php/Simple_TLS_Server
void tlsCreation() {
  int checkVal; char uidString[14]; 

  if (SSL_library_init() < 0) { exitF("Failed initializing SSL Library", 2); }
  OpenSSL_add_all_algorithms();

  const SSL_METHOD *method = TLSv1_client_method();
  SSL_CTX *ctx;
  ctx = SSL_CTX_new(method);
  if (!ctx) { exitF("Failed TLS Server Creation", 2); }
  ssl = SSL_new(ctx);

  //https://www.youtube.com/watch?v=LtXEMwSG5-8&t=1315s
  //https://www.geeksforgeeks.org/socket-programming-cc/  
  //int sockfd = socket(domain, type, protocol)
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) { exitF("Failed Socket Creation", 2); }
  socket_flag = 1; //set socket flag

  //Set up a host and grab the host through name of host, then create ip
  struct hostent *server = gethostbyname(host);
  char* ip = inet_ntoa(* ((struct in_addr*) server->h_addr_list[0]));
  
  //Specify a remoteSrv socket address
  struct sockaddr_in remoteSrv = {0};

  //Set the host, protocol and port number for the socket structure
  remoteSrv.sin_addr.s_addr = inet_addr(ip); //.sin_addr contains another address
  remoteSrv.sin_port = htons(portNum);  
  remoteSrv.sin_family = AF_INET; 

  /*
    https://www.youtube.com/watch?v=LtXEMwSG5-8&t=1315s
    "Perform actual connection: Cast server address structure to a differ struct
	where we cast our socket to a socket structure with a pointer on the
	address of the remoteSrv.
	Connect checkValurns an integer to let us know if this was successful.
	so we can perform error handling with it by assigning an integer"
  */
  checkVal = connect(sockfd, (struct sockaddr *) &remoteSrv, sizeof(struct sockaddr_in)); 
  if (checkVal < 0) { //error connecting to server
    fprintf(stderr, "%d", checkVal); exitF("Failed Server Connection", 2);
  }

  //https://stackoverflow.com/questions/54116344/how-to-swap-two-open-file-descriptors
  //Handle stdin file descriptor
  if (close(0) < 0) { exitF("Failed removing standard input file des", 2); }

  //Handle stdout file descriptor
  dup(sockfd); 
  if (close(1) < 0) { exitF("Failed removing standard output file des", 2); }
  dup(sockfd);


  SSL_set_fd(ssl, sockfd); //setup ssl 
  if (SSL_connect(ssl) < 1) { exitF("Failed SSL Connection", 2); }
  else { sslFlag = 1; }

  //Perform Buffer and Server Writes
  //int SSL_write(SSL *ssl, const void *buf, int num)
  sprintf(uidString, "ID=%s\n", id);
  SSL_write(ssl, uidString, 13);

  return;
}
