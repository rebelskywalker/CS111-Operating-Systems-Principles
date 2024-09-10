#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include "SortedList.h"

#define FACTORT 3
#define BSIZE 256
#define HSIZE 31
#define AVAL 1

/*
Globals
*/

SortedList_t* head;
SortedListElement_t* elements;
int sync_flag = 0;
int* lock;
char opt_sync;

char fmsg[] =
  "\tProgram Proper Format Message\n"
  "./lab2_add [--threads=NUMTHRDS] [--iterations=NUMITERATIONS]\n"
  "C program to implement and test shared vairable\n"
  "Options: \n"
  "--threads=NUMTHRDS: Takes parameter for number of parallel threads, df:1\n"
  "--iterations=NUMITERATIONS: takes parameter for number of iterations, df:1\n"
  "--yield=[idl]: Causes threads to yield\n"
  "--sync=[sm]: Imposes Synchronization on threads\n"
  "\t Proper Format Message";

pthread_t * tid;
pthread_mutex_t* m_lock;
unsigned long lists;
unsigned long threads;
unsigned long iterations;

/*
Protos
*/
void printcsv(struct timespec* startTime, struct timespec* finishTime, long tWait);
void exitT(const char * msg);
void parseOpts(int argc, char** argv);
void exitE(const char * rsn);
void threadCreate(long threads);
void exitO(const char * msg);
void* threadHandler(void *items);
void segHandler();

int main(int argc, char* argv[]) {
  unsigned long elemCount, i, j, k;
  long listSize = 0;
  i = j = k = 0;
  long tWaiting = 0;

  //Handle Seg Faults
  signal(SIGSEGV, segHandler);

  //Run Options Parser
  parseOpts(argc, argv);

  //Begin creation of lists
  head = malloc(sizeof(SortedList_t) * lists);
  if (head == NULL) { exitE("Error allocating head"); }
  for (i = 0; i < lists; i++) {
    head[i].next = NULL;
    head[i].prev = NULL;
  }

  elemCount = threads * iterations;
  elements = malloc(sizeof(SortedListElement_t) * elemCount);
  if (elements == NULL) {exitE("Failed element allocation");}  
  char** keys = malloc(iterations * threads * sizeof(char*));
  if (keys == NULL) {exitE("Failed allocating keys");}
  for (i = 0; i < elemCount; i++) {
    keys[i] = malloc(sizeof(char) * BSIZE);
    if (keys[i] == NULL) { exitE("Failed allocating keys");}
        
    for (j = 0; j < (BSIZE-1); j++) {
      keys[i][j] = rand() % 0x5E + 0x21;
    }
    keys[i][(BSIZE-1)] = '\0';
    (elements + i)->key = keys[i];
  }
  if (opt_sync == 'm') {
    m_lock = malloc(sizeof(pthread_mutex_t) * lists);
    if (m_lock == NULL) { exitE("Failed mutex memory allocation"); }
    for (i = 0; i < lists; i++) {
      if (pthread_mutex_init((m_lock + i), NULL) != 0) {
	exitE("Failed mutex init");
      }
    }		
  } else if (opt_sync == 's') {
    lock = malloc(sizeof(int) * lists);
    if (lock == NULL) {
      exitE("Failed allocating spin lock mem");
    }
    for (k = 0; k < lists; k++) {
      lock[k] = 0;
   }	

  }
  //StartTime High Resolution time
  struct timespec startTime, finishTime;

  //Use Clock Monotonic
  //  clock_gettime(CLOCK_MONOTONIC, &startTime);

//Grab start time for clock
  if(clock_gettime(CLOCK_MONOTONIC, &startTime) == -1)
    {
      fprintf(stderr, "Error: %s", strerror(errno));
      switch(errno)
	{
	case EFAULT:
	  fprintf(stderr, "Failed startTime capture due to ptr outside bounds");
	  break;
	case EINVAL:
	  fprintf(stderr, "Failed startTime capture; Missing Monotonic Support");
	  break;
	default:
	  fprintf(stderr, "Misc Error in Recording clock start time");
	  break;
	}
      exitE("Clock startTime error caught");
    }

  
  //Create threads
  threadCreate(threads);

  void** isWait = (void *) malloc(sizeof(void**));

  //Join Threads
  for (k = 0; k < threads; k++){
    pthread_join(tid[k], isWait);
    tWaiting += (long) *isWait;
  }

  //Grab finishing time for clock
  if(clock_gettime(CLOCK_MONOTONIC, &finishTime) == -1)
    {
      fprintf(stderr, "Error: %s", strerror(errno));
      switch(errno)
	{
	case EFAULT:
	  fprintf(stderr, "Failed finishTime capture due to ptr outside bounds");
	  break;
	case EINVAL:
	  fprintf(stderr, "Failed finishTime capture; Missing Monotonic Support");
	  break;
	default:
	  fprintf(stderr, "Misc Error in Recording clock finish time");
	  break;
	}
      exitE("Clock finishTime error caught");
    }

  
  //  clock_gettime(CLOCK_MONOTONIC, &finishTime);

  for (i = 0; i < lists; i++) {
    listSize += SortedList_length(head + i);
  }
  
  if (listSize != 0) { exitT("List did not end with 0"); }

  //StartTime print
  	fprintf(stdout, "list");
	if(opt_yield == 0x0)
	  {
	    fprintf(stdout, "-none");
	  }
	else if(opt_yield == 0x1)
	  {
	    fprintf(stdout, "-i");
	  }
	else if(opt_yield == 0x2)
	  {
	    fprintf(stdout, "-d");
	  }
	else if(opt_yield == 0x3)
	  {
	    fprintf(stdout, "-id");
	  }
	else if(opt_yield == 0x4)
	  {
	    fprintf(stdout, "-l");
	  }
	else if(opt_yield == 0x5)
	  {

	    fprintf(stdout, "-il");
	  }
	else if(opt_yield == 0x6)
	  {
	    fprintf(stdout, "-dl");
	  }
	else if(opt_yield == 0x7)
	  {
	    fprintf(stdout, "-idl");
	  }

	//Check values for opt_sync
	switch(opt_sync) {
	    case 0:
	        fprintf(stdout, "-none");
	        break;
	    case 's':
	        fprintf(stdout, "-s");
	        break;
	    case 'm':
	        fprintf(stdout, "-m");
	        break;
	    default:
	        break;
	}

	printcsv(&startTime, &finishTime, tWaiting);
	free(elements);
	free(tid);
  	if (opt_sync == 'm') {
    		for (i = 0; i < lists; i++){
    	  		pthread_mutex_destroy(m_lock + i);
    		}
    		free(m_lock);
 	} else if (opt_sync == 's') {
 		free (lock);
 	}
 	free(isWait);
	exit(0);
}

/*
Handle Other Exits
*/
void exitT(const char * msg){
  fprintf(stderr, "\nError: %s\n", msg);
  exit(2);
}


unsigned int hfunc(const char* key) {
 	int value;
 	while (*key) {
	  value = (7 << 2) * HSIZE ^ *key;
    		key++;
  	}
  	return value % lists;
}


/*
Handle general errors providing reason for exit
*/
void exitE(const char * rsn){
  fprintf(stderr, "\nError: %s\n", rsn);
  exit(1); 
}

/*
Handle option argument errors and display proper format message
*/
void exitO(const char * msg){
  fprintf(stderr, "\nError: %s\n", msg);
  fprintf(stderr, fmsg);
  exit(1);
}

/*
Prints data to csv
*/
void printcsv(struct timespec* startTime, struct timespec* finishTime, long tWait)
{
  int numS, numN;
  numS = numN = 0x0;
  long listCount = 0x1;
  long operations = threads * iterations * FACTORT;
  numS = 1000000000L * (finishTime->tv_sec - startTime->tv_sec);
  numN = finishTime->tv_nsec - startTime->tv_nsec;
  long timeRunning = numN + numS;
  long timepop = timeRunning / operations;
  listCount = lists;
  long tWasted = tWait;
  long lockwaiting = tWasted/operations;

  fprintf(stdout, ",%ld,%ld,%ld,%ld,%ld,%ld,%ld\n", threads, iterations, listCount, operations, timeRunning, timepop, lockwaiting);
}


/*
Handles Creation of options
*/
void parseOpts(int argc, char** argv)
{

  unsigned long i;
  	threads = 1;
	iterations = 1;
	opt_yield = 0;
	opt_sync = 0;
	lists = 1;
	int opt;
	struct option options[] = {
		{"threads", required_argument, NULL, 't'},
		{"iterations", required_argument, NULL, 'i'},
		{"yield", required_argument, NULL, 'y'},
		{"sync", required_argument, NULL, 's'},
		{"lists", required_argument, NULL, 'l'},
		{0, 0, 0, 0}
	};
	while ((opt = getopt_long(argc, argv, "", options, NULL)) != -1) {
	  switch (opt) {
	  case 't':
	    threads = atoi(optarg);
	    break;
	  case 'i':
	    iterations = atoi(optarg);
	    break;
	  case 'y':
	    for (i = 0; i < strlen(optarg); i++) {
	      if (optarg[i] == 'i') {
		opt_yield |= INSERT_YIELD;
	      } else if (optarg[i] == 'd') {
		opt_yield |= DELETE_YIELD;
	      } else if (optarg[i] == 'l') {
		opt_yield |= LOOKUP_YIELD;
	      }
	    }
	    break;
	  case 's':
	    opt_sync = optarg[0];
	    break;
	  case 'l':
	    lists = atoi(optarg);
	    break;				
	  default:
	    exitO("Incorrect use of options");
		}
	}
}


/*
Handles Segfault
*/
void segHandler() {
  exitE( "Segmentation fault caught and terminating program");
}

/*
Creates threads and performs errno checks
*/
void threadCreate(long threads)
{
  int i;
	int checkCreate = 0;
	tid = malloc(sizeof(pthread_t) * threads);
    	if (tid == NULL) {
	  exitE("Failed allocating heap memory for threads");
    	}
	for (i = 0; i < threads; i++) {
	  checkCreate = pthread_create(&tid[i], NULL, &threadHandler, (void *) (elements + iterations * i));
	    if(checkCreate != 0)
	      {
	       fprintf(stderr, "Failed thread creation %d\n", i+1);
	       switch(checkCreate)
		 {
		 case EAGAIN:
		   fprintf(stderr, "System is unable to supply another thread\n");
		   break;
		 case EPERM:
		   fprintf(stderr, "Missing permission for scheduling thread\n");
		   break;
		 default:
		   fprintf(stderr, "Misc fail in thread creation\n");
		 }
	       exitE("Failed during thread creation");
	      }
  	}
}

void* threadHandler(void *items) {
	long time = 0;
	unsigned long i;
	struct timespec startTime, finishTime;
	SortedListElement_t* array = items;
	for (i = 0; i < iterations; i++) {
		unsigned int hval = hfunc( (array+i) -> key);
		clock_gettime(CLOCK_MONOTONIC, &startTime);
		if (opt_sync == 'm') {
	        	pthread_mutex_lock(m_lock + hval);
	    	} else if (opt_sync == 's') {
	    		while (__sync_lock_test_and_set(lock + hval, 1));
	    	}
	   	clock_gettime(CLOCK_MONOTONIC, &finishTime);
		int numS = 1000000000L * (finishTime.tv_sec - startTime.tv_sec);
		int numN =  finishTime.tv_nsec - startTime.tv_nsec;
		time +=  numS + numN;
		SortedList_insert(head + hval, (SortedListElement_t *) (array+i));
    		if (opt_sync == 'm') {
        		pthread_mutex_unlock(m_lock + hval);
   		} else if (opt_sync == 's') {
        		__sync_lock_release(lock + hval);
    		}
	}

    	// get length of all heads for total list lenght
    	unsigned long listSize = 0;
    	for (i = 0; i < lists; i++) {
     		if (opt_sync == 'm') {
		  clock_gettime(CLOCK_MONOTONIC, &startTime);
		  pthread_mutex_lock(m_lock + i);
		  clock_gettime(CLOCK_MONOTONIC, &finishTime);
		  int numS1 = 1000000000L *(finishTime.tv_sec - startTime.tv_sec);
		  int numN1 =  finishTime.tv_nsec - startTime.tv_nsec;
		  time += numS1+ numN1;
      		} else if (opt_sync == 's') {
         		while (__sync_lock_test_and_set(lock + i, 1));
      		}
    	}
   	for (i = 0; i < lists; i++) {
    		listSize += SortedList_length(head + i);
	}
	if (listSize < iterations) { exitT("Failed to insert all items");}
	for (i = 0; i < lists; i++) {
      		if (opt_sync == 'm') {
          		pthread_mutex_unlock(m_lock + i);
      		} else if (opt_sync == 's') {
          		__sync_lock_release(lock + i);
      		}
    	}
	char *curr_key = malloc(sizeof(char)*BSIZE);
	SortedListElement_t *ptr;
	for (i = 0; i < iterations; i++) {

	  unsigned int hval = hfunc( (array+i)->key );
	  strcpy(curr_key, (array+i)->key);
	  //clock_gettime(CLOCK_MONOTONIC, &startTime);
	  if(clock_gettime(CLOCK_MONOTONIC, &finishTime) == -1)
	    {
	      fprintf(stderr, "Error: %s", strerror(errno));
	      switch(errno)
		{
		case EFAULT:
		  fprintf(stderr, "Failed startTime capture due to ptr outside bounds");
		  break;
		case EINVAL:
		  fprintf(stderr, "Failed startTime capture; Missing Monotonic Support");
		  break;
		default:
		  fprintf(stderr, "Misc Error in Recording clock start time");
		  break;
		}
	      exitE("Clock startTime error caught");
	    }

	  if (opt_sync == 'm') {
	    pthread_mutex_lock(m_lock + hval);
	  } else if (opt_sync == 's') {

	    while (__sync_lock_test_and_set(lock + hval, 1));

	  }

	  //	    	clock_gettime(CLOCK_MONOTONIC, &finishTime);
	  //Grab finishing time for clock
	  if(clock_gettime(CLOCK_MONOTONIC, &finishTime) == -1)
	    {
	      fprintf(stderr, "Error: %s", strerror(errno));
	      switch(errno)
		{
		case EFAULT:
		  fprintf(stderr, "Failed finishTime capture due to ptr outside bounds");
		  break;
		case EINVAL:
		  fprintf(stderr, "Failed finishTime capture; Missing Monotonic Support");
		  break;
		default:
		  fprintf(stderr, "Misc Error in Recording clock finish time");
		  break;
		}
	      exitE("Clock finishTime error caught");
	    }
		
	  int numN2,numS2;
	  numN2 = 1000000000L * (finishTime.tv_sec - startTime.tv_sec);
	  numS2 = finishTime.tv_nsec - startTime.tv_nsec;
	  time += numN2 + numS2;
        	ptr = SortedList_lookup(head + hval, curr_key);
        	if (ptr == NULL) { exitT("Failed to lookup elements"); }
		int n = SortedList_delete(ptr);
        	if (n != 0) { exitT("Failed to delete element\n"); }
		if (opt_sync == 'm') {
			pthread_mutex_unlock(m_lock + hval);
		} else if (opt_sync == 's') {
			__sync_lock_release(lock + hval);
		}
	}
    	return (void *) time;
}

