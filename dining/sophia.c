#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>

/**
  * Bill Ebeling
  * An attempt to model a solution to the dining philosophers problem.
  */

#define N 5 //number of philosophers
enum {FALSE, TRUE};
enum {THINKING, HUNGRY, EATING};
sem_t chops[N];
int running=1, numRounds=1000;

/** 
  * The Philosophers
  */
void * philosopher(void * id){
int state = THINKING;
int thinkUnits=0, hungerUnits=0, eatingUnits=0;
int runCount=0, myID = *(int*) id, waiting=0;
char* msg;

//printf("New Philosopher %d!\n", myID); 

while(running){
  switch(state){
    case THINKING:
      thinkUnits++;
      if (thinkUnits%100 > 1+rand()%100){// chance to get hungry grows with time
        state= HUNGRY; 
        msg = "getting hungry!";//printf("%d is getting hungry!\n", myID);
      }
      break;
    case HUNGRY:
      hungerUnits++;
      sem_wait(&chops[(myID+1) % N]);//right first-- must be 1 higher than myID except when myID == N, then it should be 0
      sem_wait(&chops[myID]);//left -- must equal myID
      msg = "has begun eating!";
      state=EATING;
      break;
    case EATING:
        eatingUnits++;
        if (eatingUnits % 50 > 1+rand()%50){
          sem_post(&chops[myID]);
          sem_post(&chops[(myID+1) % N]);
          state=THINKING;
          msg="has had their full!";
        }
      break;
    default:
      printf("Philosopher without a state.");
      break;
  }
    printf("%d\t%d\t%d\t%d\t%d\t%d\t%s\n", myID, runCount, state, thinkUnits, hungerUnits, eatingUnits, msg);
//  printf("%d\treport round %d:\tstate:%d\tTU:%d\tHU:%d\tEU:%d\t%s\n", myID, runCount, state, thinkUnits, hungerUnits, eatingUnits, msg);
  msg="";
  sleep(1+rand()%2);
if(runCount % 10 == 0) fflush(stdout);  
if (runCount++ == numRounds) running=FALSE;
}
}

// MAIN
int main(){
  pthread_t thr[N];
  srand((long int) time(NULL));
  //printf("Seating Philosophers now.\n");
  
  printf("id\tTRound\tstate\tThinkUnits\thungerUnits\tEatingUnits\tmessage\n");

  int i;
  for (i=0; i < N; i++){
    sem_init( &chops[i], FALSE, 1);
  }
  //printf("making threads\n");
  for (i = 0; i < N; i++){
    pthread_create(thr + i, NULL, philosopher, (void*)(&i));
    //I think i is incrementing before the thread is done initializing
    sleep(1);
  }
  //printf("joining threads\n");
  for (i = 0; i < N; i++){
    pthread_join(thr[i], NULL);
  }
  //printf("destroying semaphores\n");
  for (i = 0; i < N; i++ ){
    sem_destroy(&chops[i]);
  }

  printf("OK, bye\n");
  return 0;
}

