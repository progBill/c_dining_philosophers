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

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//  If running is univeral, it should be in a critical region!!

// running is universal so that when a thread gets to numRounds, they all stop
int running=TRUE, numRounds=200000000;

LIST waitingList;

// CHANGE BELOW TO SWITCH FEEDING STRATEGY
const int NORMAL = TRUE; 

// ********************** 
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
  if (myID % 2 == 0){//even looks right first 
    pthread_mutex_lock(&chops[(myID+1) % N]);
    pthread_mutex_lock(&chops[myID]);
  }else{//odd looks left first
    pthread_mutex_lock(&chops[myID]);
    pthread_mutex_lock(&chops[(myID+1)%N]);
  }
}

void releaseUtensils(int myID){
  pthread_mutex_unlock(&chops[myID]);
  pthread_mutex_unlock(&chops[(myID+1) % N]);
}

void unlock(ELEMENT elem){
  listAddElem(waitingList, elem);
  int nextID;
  // nextID elem needs to be an eating elem
  if(diners.firstEater.myID != 9){
    nextID = listGetNext(waitingList, diners.firstEater);
  }else if(diners.secondEater.myID != 9){
    nextID = listGetNext(waitingList, diners.secondEater);
  }else{
    nextID = elem.myID;
  }

  if (nextID != -1){
    pthread_mutex_unlock(&chops[nextID]);
    pthread_mutex_unlock(&chops[(nextID+1) % N]);
    if (diners.firstEater.myID == 9) diners.firstEater = elem;
    else diners.secondEater = elem;
  }
}

void relock(ELEMENT elem){
  listRemoveElem(waitingList, elem);
  //removeDiner(elem);
  pthread_mutex_lock(&chops[elem.myID]);
  pthread_mutex_lock(&chops[(elem.myID+1) % N]);
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
	  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	  usleep(300000);
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
	      if(NORMAL){
		listAddElem(waitingList, me);
		reserveUtensils(me.myID);
		listRemoveElem(waitingList, me);
	      }else{
		unlock(me);
		relock(me);
	      }
	      msg = "has begun eating!";
	      state=EATING;
	      break;

	    case EATING:
	      eatingUnits++;
	      if (isFull() == TRUE){
		if(NORMAL){
		  releaseUtensils(me.myID);
		}else{
		  //relock(me);
		  removeDiner(me);
		}
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
	  listPrint(waitingList);

	  //sleep(1); 
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

  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  waitingList = listCreate(N);
  ELEMENT NULLELEM = {9,9,9};
  diners.firstEater = NULLELEM;
  diners.secondEater= NULLELEM;

  int i;
  for (i=0; i < N; i++){
    pthread_mutex_init( &chops[i], NULL);  // this is a one item semaphore
    // make sure they do not link to same local variable

    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    ids[i] = i;
    pthread_create(thr + i, NULL, philosopher, (void*)(ids+i));
    sleep(1);
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

