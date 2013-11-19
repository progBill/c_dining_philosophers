#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>

/**
  * Bill Ebeling
  * An attempt to model a solution to the dining philosophers problem.
  */

#define N 5 //number of philosophers
//below: stolen from the internet
#define cls() printf("\033[H\033[J");
#define position(row,col) printf( "\033[%d;%dH", (row), (col) )

enum {FALSE, TRUE};
enum {THINKING, HUNGRY, EATING};
sem_t chops[N];
int running=1, numRounds=200;
int id[N];
/** 
  * The Philosophers
  */
void * philosopher(void * id){
int state = THINKING;
int thinkUnits=0, hungerUnits=0, eatingUnits=0;
int runCount=0, myID = *(int*) id, waiting=0;
char* msg;

while(running){
  switch(state){
    case THINKING:
      thinkUnits++;
      if (rand()%100 < 10){// % chance 
        state= HUNGRY; 
        msg = "getting hungry!";//printf("%d is getting hungry!\n", myID);
      }
      break;
    case HUNGRY:
      hungerUnits++;
      if (myID % 2 == 0){//even looks right first 
      sem_wait(&chops[(myID+1) % N]);
      sem_wait(&chops[myID]);
      }else{//odd looks left first
      sem_wait(&chops[myID]);
      sem_wait(&chops[(myID+1)%N]);
      }
      msg = "has begun eating!";
      state=EATING;
      break;
    case EATING:
        eatingUnits++;
        if (rand()%100 < 5){// % chance
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

void printer(){

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

