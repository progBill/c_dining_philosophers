#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include "dining_list.h"
#include <unistd.h>

/**
  * Bill Ebeling
  * An attempt to model a solution to the dining philosophers problem.
  */

#define N 6 //number of philosophers
enum {FALSE, TRUE};
enum {THINKING, HUNGRY, EATING};

pthread_mutex_t chops[N];
pthread_mutex_t singlepass;
pthread_cond_t eaters[N];

// running is universal so that when a thread gets to numRounds, they all stop
int running=TRUE, numRounds=200000000;
int nextUp;

LIST waitingList; 
ELEMENT NULLELEM;
struct {ELEMENT firstEater; 
        ELEMENT secondEater;
} diners;

int isFull(){
  // % chance
  if (rand()%1000 < 5) return TRUE;
  else return FALSE;
}

int isHungry(){
  // % chance
  if (rand()%100 < 10) return TRUE;
  else return FALSE;
}

void removeDiner(ELEMENT elem){
  if(diners.firstEater.myID == elem.myID) diners.firstEater = NULLELEM;
  else if (diners.secondEater.myID == elem.myID) diners.secondEater = NULLELEM;
}

void reserveUtensils(int myID){
    pthread_mutex_lock(&singlepass);
    printf("\033[%d;%dH %d first mutex!--cond: %d", 20 , 30*myID, myID, (myID != nextUp ));
    while (myID != nextUp ) pthread_cond_wait(&eaters[myID], &singlepass);
    //while (1 != 1) pthread_cond_wait(&eaters[myID], &singlepass);
    printf("\033[%d;%dH %d second mutex!", 21, 30*myID, myID);
    pthread_mutex_unlock(&singlepass);
    printf("\033[%d;%dH %d in crit region!", 22, 30*myID, myID);    
}

/** 
  * The Philosophers
  */

void *philosopher(void * id){
	int state = THINKING;
	int thinkUnits=0, hungerUnits=0, eatingUnits=0;
	int runCount=0, myID = *(int*) id, waiting=0;
	char* msg;

	ELEMENT me;
	me.right = ( myID + N - 1 ) % N;
	me.left = (myID + 1 ) % N;
	me.myID = myID;

	while(running){
	  usleep(100000);
	  switch(state){

	    case THINKING:
	      thinkUnits++;
	      if (isHungry() == TRUE){ 
		state= HUNGRY; 
		msg = "has become hungry!";
	      }else{
		msg="cogitating";
	      }
	      break;

	    case HUNGRY:
	      hungerUnits++;
        listAddElem(waitingList, me);
	  	  reserveUtensils(me.myID);
        if(diners.firstEater.myID == 9) diners.firstEater = me;
        else if (diners.secondEater.myID == 9) diners.secondEater = me;
	      msg = "has begun eating!";
	      state=EATING;
	      break;

	    case EATING:
	      eatingUnits++;
	      if (isFull() == TRUE){
		      //releaseUtensils(me.myID);
          nextUp = listGetNext(waitingList, me);
          listRemoveElem(waitingList, me);
    		  state=THINKING;
          if (diners.firstEater.myID == myID) diners.firstEater = NULLELEM;
          else if (diners.secondEater.myID == myID) diners.secondEater = NULLELEM;

          pthread_cond_broadcast(&eaters[(myID + 1) % N]);

	      	msg="has had their full!";
	      } else {
      		msg="currently eating";
	      }
	      break;

	    default:
	      printf("Philosopher without a state.");
	      break;
	  }

	  // this funny line moves the cursor around so the screen updates without scrolling  
	  printf("\033[%d;%dH%-15d%-15d%-15d%-15d%-15d%-15d%-30s\033[20;5H", 
		  (myID+3), 0, myID, runCount, state, thinkUnits, hungerUnits, eatingUnits, msg);
	  listPrint(waitingList);

	  msg="";
	  if(runCount % 10 == 0) fflush(stdout);  
    //TODO: put line below in critical section
	  if (runCount++ == numRounds) running=FALSE;
	}

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// if  EATING when it stops, release utensils so others can unblock and exit their loops
	//if (EATING) releaseUtensils(me.myID);
  if (EATING){
    if (diners.firstEater.myID == myID) diners.firstEater = NULLELEM;
    else if (diners.secondEater.myID == myID) diners.secondEater = NULLELEM;
    nextUp = listGetNext(waitingList, me);
  }
}

// MAIN
int main(){
  pthread_t thr[N];
  int ids[N];
  nextUp = 0;

  srand((long int) time(NULL));

  printf("\033[2J\033[H");//clear screen, leave cursor in upper left corner
  printf("%-15s%-15s%-15s%-15s%-15s%-15s%-15s",
	     "id","TRound","state","ThinkUnits","hungerUnits","EatingUnits","message");

  waitingList = listCreate(N);
  ELEMENT NULLELEM = {9,9,9};
  diners.firstEater = NULLELEM;
  diners.secondEater= NULLELEM;

  pthread_mutex_init( &singlepass, NULL );

  int i;
  for (i=0; i < N; i++){
    ids[i] = i;
    usleep(100000);
    pthread_mutex_init( &chops[ids[i]], NULL); 
    pthread_cond_init( &eaters[ids[i]], NULL );
    pthread_create(thr + i, NULL, philosopher, (void*)(ids+i));
  }
  
  for (i = 0; i < N; i++){
    pthread_join(thr[i], NULL);
  }

  for (i = 0; i < N; i++ ){
    pthread_mutex_destroy(&chops[i]);
  }

  pthread_mutex_destroy( &singlepass );

  printf("OK, bye\n");
  return 0;
}

