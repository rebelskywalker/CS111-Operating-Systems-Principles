#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <getopt.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sched.h>
#include <pthread.h>

#define FACTOR 2

/*
Globals
Pthread Mutexes Youtube
*/
long long counter;
long threads;
long iterations;
int opt_yield;

char fmsg[] =
  "\tProgram Proper Format Message\n"
  "./lab2_add [--threads=NUMTHRDS] [--iterations=NUMITERS]\n"
  "C program to implement and test shared vairable\n"
  "Options: \n"
  "--threads=NUMTHRDS: Takes parameter for number of parallel threads"
  "--iterations=NUMITERS: takes parameter for number of iterations"
  "\tEnd Proper Format Message";

char opt_sync;
int lock = 0;
pthread_mutex_t m_lock; 
char * sync_type = NULL;
pthread_t* thread_ids;

/*
Protos
*/
void printcsv(struct timespec* begin, struct timespec* end);
void exitE(const char * rsn);
void exitO(const char * msg);
void add(long long *pointer, long long value);
void* sharedAdder(void* arg);
void threadCreate(long threads);
void addMutex(long long *pointer, long long value);
void addSpin(long long *pointer, long long value);
void addCSwap(long long *pointer, long long value);



int main(int argc, char* argv[]) {
  int i, opt; // numN, numS;
  threads = 1;
  iterations = 1;
  opt_yield = 0;
  opt_sync = 0;
  counter = 0; // initialize the long long counter to zero
  struct option options[] = {
			     {"threads", required_argument, NULL, 't'},
			     {"iterations", required_argument, NULL, 'i'},
			     {"yield", no_argument, NULL, 'y'},
			     {"sync", required_argument, NULL, 's'},
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
      opt_yield = 1;
      break;
    case 's':
      opt_sync = optarg[0];				
      //Start addition
      sync_type = optarg;
      if(strlen(optarg) > 1) {exitE("Wrong arg for --sync");}
      break;
    default:
      exitO("Improper use of program arguments");
    }
  }

  // start system clock elements of start and finish
  struct timespec begin, end;
  /*
	  struct timespec contains seconds (time_t tv_sec) and
	  nanoseconds (tv_nsec)
	  clock_gettime(clockid, *timespecpointer)
	  clock_gettime returns 0 for success, clock Monotonic on linux
	  represents number of seconds sice boot time. Does not count time
	  during system suspension. 
	 */
  clock_gettime(CLOCK_MONOTONIC, &begin);

  //Create threads
  threadCreate(threads);
  
  for (i = 0; i < threads; i++) {
    pthread_join(thread_ids[i], NULL);
  }
  //Grab finishing time for clock
  if(clock_gettime(CLOCK_MONOTONIC, &end) == -1)
    {
      fprintf(stderr, "Error: %s", strerror(errno));
      //Perform various errno checks
      switch(errno)
	{
	case EFAULT:
	  fprintf(stderr, "Failed endtime capture due to ptr outside bounds");
	  break;
	case EINVAL:
	  fprintf(stderr, "Failed endtime capture; Missing Monotonic Support");
	  break;
	default:
	  fprintf(stderr, "Misc Error in Recording clock finish time");
	  break;
	}
      exitE("Clock endtime error caught");
    }
  //print to csv files
  printcsv(&begin, &end);  
  	if (opt_sync == 'm')
	    pthread_mutex_destroy(&m_lock);
	free(thread_ids);
	exit(0);
}


/*
Handle general errors providing reason for exit
*/
void exitE(const char * rsn){
  fprintf(stderr, "\nError: %s\n", rsn);
  exit(1);  
}


/*
Lab 2A Manual add function
"Basic add routine" -> "Extended basic routine with yield"
*/

void add(long long *pointer, long long value) {
  long long sum = *pointer + value;
  if (opt_yield) {
    sched_yield();
  }
  *pointer = sum;
}

/*
Add pthread mutex around the critical addiing section
*/
void addMutex(long long *pointer, long long value) {
  pthread_mutex_lock(&m_lock);
  add(pointer, value);
  pthread_mutex_unlock(&m_lock);
}

void addSpin(long long *pointer, long long value) {
//(pointer of var to set, value to set var that p points to)
  while (__sync_lock_test_and_set(&lock, 1));
  add(pointer, value);
  __sync_lock_release(&lock);
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
Implement a compare and swap add function
GCC 5.44 Built in Functions: sync val compare and swap
*/
void addCSwap(long long *pointer, long long value) {
  long long old;
  do {
    old = counter;
    if (opt_yield) {
      sched_yield();
    }
  } while (__sync_val_compare_and_swap(pointer, old, old+value) != old);
}



void printcsv(struct timespec* begin, struct timespec* end)
{
  int numS, numN;
  /* data to csv strings */
  long operations = threads * iterations * FACTOR;
  numS = 1000000000L * (end->tv_sec - begin->tv_sec);
  numN = end->tv_nsec - begin->tv_nsec;
  long timeRunning = numN + numS;
  long opTime = timeRunning / operations;

  /* print the data */
  fprintf(stdout, "add-");
  if (opt_yield == 1 && opt_sync == 'm')
    fprintf(stdout, "yield-m,%ld,%ld,%ld,%ld,%ld,%lld\n", threads, iterations, operations, timeRunning, opTime, counter);
  else if (opt_yield == 1 && opt_sync == 's')
	    fprintf(stdout, "yield-s,%ld,%ld,%ld,%ld,%ld,%lld\n", threads, iterations, operations, timeRunning, opTime, counter);
	  else if (opt_yield == 1 && opt_sync == 'c')
	    fprintf(stdout, "yield-c,%ld,%ld,%ld,%ld,%ld,%lld\n", threads, iterations, operations, timeRunning, opTime, counter);
	  else if (opt_yield == 1 && opt_sync == 0)
	    fprintf(stdout, "yield-none,%ld,%ld,%ld,%ld,%ld,%lld\n", threads, iterations, operations, timeRunning, opTime, counter);
	  else if (opt_yield == 0 && opt_sync == 'm')
	    fprintf(stdout, "m,%ld,%ld,%ld,%ld,%ld,%lld\n", threads, iterations, operations, timeRunning, opTime, counter);
	  else if (opt_yield == 0 && opt_sync == 's')
	    fprintf(stdout, "s,%ld,%ld,%ld,%ld,%ld,%lld\n", threads, iterations, operations, timeRunning, opTime, counter);
	  else if (opt_yield == 0 && opt_sync == 'c')
	    fprintf(stdout, "c,%ld,%ld,%ld,%ld,%ld,%lld\n", threads, iterations, operations, timeRunning, opTime, counter);
	  else
	    fprintf(stdout, "none,%ld,%ld,%ld,%ld,%ld,%lld\n", threads, iterations, operations, timeRunning, opTime, counter);

}

/*
"Add 1 to counter specified number times
Add -1 to counter specified number times"
*/

void* sharedAdder(void* arg) {
	long numTimes;	
	for (numTimes = 0; numTimes < iterations; numTimes++) {
	  switch(opt_sync)
	    {
	    case 'm':
	      addMutex(&counter, 1); addMutex(&counter, -1);
	      break;
	    case 's':
	      addSpin(&counter, 1); addSpin(&counter, -1);
	      break;
	    case 'c':
	      addCSwap(&counter, 1); addCSwap(&counter, -1);
	      break;
	    default:
	      add(&counter, 1); add(&counter, -1);
	      break;
	    }
	}
	return arg;
}

void threadCreate(long threads)
{
  int i;
  int checkCreate = 0;
  thread_ids = malloc(threads * sizeof(pthread_t));
  if (thread_ids == NULL) {
    exitE("Failed allocating heap memory for threads");
  }
  if (opt_sync == 'm') {
    if (pthread_mutex_init(&m_lock, NULL) != 0){exitE("Failed init mutex");}	
  }
  /* create threads to add 1 to counter and add -1 to counter */
  for (i = 0; i < threads; i++) {
    checkCreate = pthread_create(&thread_ids[i], NULL, &sharedAdder, NULL);
      if(checkCreate != 0){
	fprintf(stderr, "Failed thread creation %d", i+1);
	switch(checkCreate)
	  {
	  case EAGAIN:
	    fprintf(stderr, "System is unable to supply another thread");
	    break;
	  case EPERM:
	    fprintf(stderr, "Missing permission for scheduling thread");
	    break;
	  default:
	    fprintf(stderr, "Misc fail in thread creation");
	}
	exitE("Failed during thread creation");
      }
  }
}

