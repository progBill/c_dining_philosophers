#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>
#include "dining_list.h"


/**
  * Bill Ebeling
  * An attempt to model a solution to the dining philosophers problem.
  */

#define N 6 //number of philosophers
enum {FALSE, TRUE};
enum {THINKING, HUNGRY, EATING};

pthread_mutex_t chops[N];

// running is universal so that when a thread gets to numRounds, they all stop
int running=TRUE, numRounds=200;

int isFull(){
  // % chance
  if (rand()%1000 < 5) return TRUE;
  else return FALSE;
}

int isHungry(){
  // % chance
  if (rand()%100 < 5) return TRUE;
  else return FALSE;
}

void reserveUtensils(int myID){
  if (myID % 2 == 0){//even looks right first 
    pthread_mutex_lock(&chops[(myID+1) % N]);
    pthread_mutex_lock(&chops[myID]);
  }else{//odd looks left first
    pthread_mutex_lock(&chops[myID]);
    pthread_mutex_lock(&chops[(myID+1) % N]);
  }
}

void releaseUtensils(int myID){
  pthread_mutex_unlock(&chops[myID]);
  pthread_mutex_unlock(&chops[(myID+1) % N]);
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
	  	  reserveUtensils(me.myID);
	      msg = "has begun eating!";
	      state=EATING;
	      break;

	    case EATING:
	      eatingUnits++;
	      if (isFull() == TRUE){
		      releaseUtensils(me.myID);
    		  state=THINKING;
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
	  // MAY NOT BE WINDOWS COMPLIANT
	  printf("\033[%d;%dH%-15d%-15d%-15d%-15d%-15d%-15d%-30s\033[20;5H", 
		  (myID+3), 0, myID, runCount, state, thinkUnits, hungerUnits, eatingUnits, msg);

	  msg="";
	  if(runCount % 10 == 0) fflush(stdout);  
    //TODO: put line below in critical section
	  if (runCount++ == numRounds) running=FALSE;
	}

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// if  EATING when it stops, release utensils so others can unblock and exit their loops
	if (EATING) releaseUtensils(me.myID);
}

// MAIN
int main(){
  pthread_t thr[N];
  int ids[N];

  srand((long int) time(NULL));

  printf("\033[2J\033[H");//clear screen, leave cursor in upper left corner
  printf("%-15s%-15s%-15s%-15s%-15s%-15s%-15s",
	     "id","TRound","state","ThinkUnits","hungerUnits","EatingUnits","message");

  int i;
  for (i=0; i < N; i++){
    pthread_mutex_init( &chops[i], NULL); 
    ids[i] = i;
    usleep(100000);
    pthread_mutex_init( &chops[ids[i]], NULL); 
    pthread_create(thr + i, NULL, philosopher, (void*)(ids+i));
  }
  
  for (i = 0; i < N; i++){
    pthread_join(thr[i], NULL);
  }

  for (i = 0; i < N; i++ ){
    pthread_mutex_destroy(&chops[i]);
  }

  printf("OK, bye\n");
  return 0;
}

