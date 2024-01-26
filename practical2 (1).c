#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "cond.c"


int pnum;  // number updated when producer runs.
int csum;  // sum computed using pnum when consumer runs.
int produceA = 0;// flag to indicate whether a number has been produced
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;// conditional variable for synchronization
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;// mutex for synchronization
int (*pred)(int); // predicate indicating if pnum is to be consumed

int produceT() {
  scanf("%d",&pnum); // read a number from stdin
  return pnum;
}

// Producer thread function
void *Produce(void *a) {
  int p;

  p=1;
  while (p) {
    pthread_mutex_lock(&mutex);
    printf("@P-READY\n"); 
  while(produceA==1){    // wait while flag is set
        pthread_cond_wait(&condition, &mutex);// wait on conditional variable

    }
    p = produceT();
    pthread_cond_signal(&condition);
    produceA = 1;   //set produceA flag
    pthread_mutex_unlock(&mutex); // release mutex lock
    printf("@PRODUCED %d\n",p);// print produced number
  }
  printf("@P-EXIT\n");
  pthread_exit(NULL);
}

// Function to check whether a number should be consumed
int consumeT() {
  if ( pred(pnum) ) { csum += pnum; }
  return pnum;
}

// Consumer thread function
void *Consume(void *a) {
  int p;

  p=1;
  while (p) {
      printf("@C-READY\n");
      pthread_mutex_lock(&mutex);//obtain mutex lock
  while(produceA==0){
        pthread_cond_wait(&condition, &mutex);
  
  }
    // consume the number
     p = consumeT();
     pthread_cond_signal(&condition);// signal producer thread
     produceA = 0;// reset produceA flag
     pthread_mutex_unlock(&mutex);// release mutex unlock
     printf("@CONSUMED %d\n",csum);//print consumed number
  }
  printf("@C-EXIT\n");// print consumer exit message
  pthread_exit(NULL);// exit thread
}


int main (int argc, const char * argv[]) {
  // the current number predicate
  static pthread_t prod,cons;
	long rc;

  pred = &cond1;
  if (argc>1) {
    if      (!strncmp(argv[1],"2",10)) { pred = &cond2; }
    else if (!strncmp(argv[1],"3",10)) { pred = &cond3; }
  }


  pnum = 999;
  csum=0;
  
  printf("@P-CREATE\n");
 	rc = pthread_create(&prod,NULL,Produce,(void *)0);
	if (rc) {
			printf("@P-ERROR %ld\n",rc);
			exit(-1);
		}
  printf("@C-CREATE\n");
 	rc = pthread_create(&cons,NULL,Consume,(void *)0);
	if (rc) {
			printf("@C-ERROR %ld\n",rc);
			exit(-1);
		}

  printf("@P-JOIN\n");
  pthread_join( prod, NULL);
  printf("@C-JOIN\n");
  pthread_join( cons, NULL);


  printf("@CSUM=%d.\n",csum);

  return 0;
}