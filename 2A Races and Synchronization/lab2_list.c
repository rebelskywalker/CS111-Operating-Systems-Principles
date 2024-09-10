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
/*
Globals
*/


SortedList_t head;
SortedListElement_t* elements;
pthread_mutex_t aMutex;
int sync_flag = 0;
int lock;
char opt_sync;

char fmsg[] =
  "\tProgram Proper Format Message\n"
  "./lab2_add [--threads=NUMTHRDS] [--iterations=NUMITERS]\n"
  "C program to implement and test shared vairable\n"
  "Options: \n"
  "--threads=NUMTHRDS: Takes parameter for number of parallel threads, df:1\n"
  "--iterations=NUMITERS: takes parameter for number of iterations, df:1\n"
  "--yield=[idl]: Causes threads to yield\n"
  "--sync=[sm]: Imposes Synchronization on threads\n"
  "\tEnd Proper Format Message";

pthread_t* tid;
int opt_yield;
unsigned long threads;
long iterations;


/*
Protos
*/
void printcsv(struct timespec* begin, struct timespec* end);
void exitT(const char * msg);
void parseOpts(int argc, char** argv);
void exitE(const char * rsn);
void threadCreate(long threads);
void exitO(const char * msg);
void* threadHandler(void *stuff);
void segHandler();


int main(int argc, char* argv[]) {
	unsigned long i;
	unsigned long j;

	//Handle Seg Faults
	signal(SIGSEGV, segHandler);

	//Run parse opts to set up longopts
	parseOpts(argc, argv);

	//Begin creating number of elements and allocations
	unsigned long eCount = threads * iterations;
	elements = malloc(sizeof(SortedListElement_t) * eCount);
	if (elements == NULL) {exitE("Failed element allocation");}
	char** keys = malloc(iterations * threads * sizeof(char*));
	if (keys == NULL) {exitE("Failed allocating keys");}
	for (i = 0; i < eCount; i++) {
		keys[i] = malloc(sizeof(char) * 256);
		if (keys[i] == NULL) { exitE("Failed key allocation");}
		for (j = 0; j < (BSIZE-1); j++) {
			keys[i][j] = rand() % 0x5E + 0x21;
		}
		keys[i][(BSIZE-1)] = '\0';
		(elements + i)->key = keys[i];
	}
	//Check if m, also lock
	if (opt_sync == 'm' && pthread_mutex_init(&aMutex, NULL) != 0) {
	  exitE("Failed mutex init");
  	}

	//StartTime High Resolution time
  	struct timespec startTime, finishTime;
  	clock_gettime(CLOCK_MONOTONIC, &startTime);

	//Create threads
	threadCreate(threads);

	//Join them
  	for (i = 0; i < threads; i++) {
  		pthread_join(tid[i], NULL);
  	}

	//Grab finishing time for clock
	if(clock_gettime(CLOCK_MONOTONIC, &finishTime) == -1)
	  {
	    fprintf(stderr, "Error: %s", strerror(errno));
	    switch(errno)
	      {
	      case EFAULT:
		fprintf(stderr, "Failed finishTimetime capture due to ptr outside bounds");
		break;
	      case EINVAL:
		fprintf(stderr, "Failed finishTimetime capture; Missing Monotonic Support");
		break;
	      default:
	  fprintf(stderr, "Misc Error in Recording clock finish time");
	  break;
	      }
	    exitE("Clock finishTimetime error caught");
	  }

	//Check Sorted List, and start print
  	if (SortedList_length(&head) != 0) {exitT("List FinishTime not 0");}
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
	//Print to csv
	printcsv(&startTime, &finishTime);
	free(elements);
	free(tid);
	if (opt_sync == 'm') {
		pthread_mutex_destroy(&aMutex);
	}
	exit(0);
}

/*
Handle Other Exits
*/
void exitT(const char * msg){
  fprintf(stderr, "\nError: %s\n", msg);
  exit(2);
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
Handles Sorted List
Locks, Inserts, Lookup
*/
void* threadHandler(void *items) {
	SortedListElement_t* array = items;
    	if (opt_sync == 'm') {
        	pthread_mutex_lock(&aMutex);
    	} else if (opt_sync == 's') {
        	while (__sync_lock_test_and_set(&lock, 1));
    	}
	
	long i;
	for (i = 0; i < iterations; i++) {
		SortedList_insert(&head, (SortedListElement_t *) (array+i));
	}
	long list_length = SortedList_length(&head);
	if (list_length < iterations) {exitT("Unable to insert all items");}
	char *keyPos = malloc(sizeof(char)*BSIZE);
	SortedListElement_t *ptr;
	for (i = 0; i < iterations; i++) {
		strcpy(keyPos, (array+i)->key);
        	ptr = SortedList_lookup(&head, keyPos);
        	if (ptr == NULL) { exitT("Failed Lookup");}
		int n = SortedList_delete(ptr);
        	if (n != 0) {exitT("Failed deleting");}
	}
    	// release mutex or lock based on sync options
    	if (opt_sync == 'm') { // unlock the mutex
        	pthread_mutex_unlock(&aMutex);
	} else if (opt_sync == 's') { // release lock
       		__sync_lock_release(&lock);
    	}
    	return NULL;
}


/*
Handles Segfault
*/
void segHandler() {
  exitE( "Segmentation fault caught and terminating program");
}

/*
Creates threads and performs several checks on errno
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


/*
Prints data to csv
*/
void printcsv(struct timespec* startTime, struct timespec* finishTime)
{
  int numS, numN;
  numS = numN = 0x0;
  long num_lists = 0x1;
  long operations = threads * iterations * FACTORT;
  numS = 1000000000L * (finishTime->tv_sec - startTime->tv_sec);
  numN = finishTime->tv_nsec - startTime->tv_nsec;
  long run_time = numN + numS;
  long time_per_op = run_time / operations;


  fprintf(stdout, ",%ld,%ld,%ld,%ld,%ld,%ld\n", threads, iterations, num_lists, operations, run_time, time_per_op);
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
	struct option options[] = {
		{"threads", required_argument, NULL, 't'},
		{"iterations", required_argument, NULL, 'i'},
		{"yield", required_argument, NULL, 'y'},
		{"sync", required_argument, NULL, 's'},
		{0, 0, 0, 0}
	};
	int opt;

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
			default:
			  exitO("Incorrect use of options");
		}
	}

}

