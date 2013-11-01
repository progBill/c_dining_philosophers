#include <stdio.h>
#include <stdlib.h>

//pipe constants for readability
const int PIPE_READ=0;
const int PIPE_WRITE=1;
const int DEBUG = 1; //used to print wall of text to screen for debugging purposes

//message to pass around
struct _msg {
  char cmd;
  int data;
};

typedef struct _msg MESSAGE;
#define DEF_MSG(m, s, d) {m.cmd=s; m.data=d;}
MESSAGE msg;

enum _state{RUNNING, QUIT};
typedef enum _state STATE;

//
// SERVER CODE
//
void server(int numPlayers, int readFD[3][2],int writeFD[3][2]){
  STATE state;
  state=RUNNING;
  int curPlayer=0, runningTotal=numPlayers*4;
  if (DEBUG) printf("new server with pid: %d\n", getpid());
  
  while(state != QUIT){

    if (DEBUG) printf("%d says there are %d sticks.\n", getpid(), runningTotal);
    
    read(readFD[curPlayer][0], &msg, sizeof(msg));
      
    switch(state){
      case RUNNING:
        if (msg.cmd == 'p'){// p for pickup
          runningTotal -= msg.data;
        } 
        if(runningTotal > 0){
          DEF_MSG(msg, 't', runningTotal);// t for total
        }else{
          DEF_MSG(msg, 'q', runningTotal);// q for quit
        }
        break;
      case QUIT:
        DEF_MSG(msg, 'q', 0);// q for quit
        break;
    } //end switch
    write(writeFD[curPlayer][1], &msg, sizeof(msg));
    curPlayer = curPlayer + 1 % numPlayers;    
  } //end while
}

//
//  PLAYER CODE
//
void player(int isHuman, int READ, int WRITE){
  int running=1;  
  int numDelta;

  if(DEBUG) printf("new player with pid %d is child of %d\n", getpid(), getppid());

  DEF_MSG(msg, 'p', 0);
  write(WRITE, &msg, sizeof(msg));
  
  while(running){
    if (DEBUG) printf("%d reading", getpid());
    read(READ, &msg, sizeof(msg));
    if (DEBUG) printf("%d: read command %d with data: %d.\n", getpid(), msg.cmd, msg.data);

    if(msg.cmd != 'q'){
      if (isHuman){
        printf("There are %d sticks left.\n", msg.data);
        printf("Pick up how many sticks?\n");
        scanf("%d", &numDelta);
      } else{
        numDelta= 3;
      }
    }

    if (msg.data > 0){// still playing
      if (msg.cmd == 't'){// server thinks game is still going
          DEF_MSG(msg, 'p', numDelta);
        }
        write(WRITE, &msg, sizeof(msg));
    }else{// all sticks are gone
      running=0;
    }
  } //end while
}

//
// MAIN
//
int main(){
  int i, pid, seed;
  
  printf("How many opponents would you like to play against?\n");
  scanf("%d", &seed);

  int fd_toChild[seed][2], fd_toParent[seed][2];
  pipe(fd_toChild);
  pipe(fd_toParent);
  int pids[seed];
  // creates server & human player 
  pid = fork();
  if (pid>0){// create computer players, 0 through seed-1
    for (i=0; i < seed; ++i){// create X times
      printf("preparing for baby making!\n");
      if((pids[i] = fork()) == 0){// only kids from this fork get made 
        if (DEBUG) printf("Making babies..\n");
        player(0, fd_toChild[i][PIPE_READ],fd_toParent[i][PIPE_WRITE]);
        exit(0);
      }else if (pids[i] < 0){
        printf("Big problems, boss.\n");
      }else{
        printf("I'm a daddy!\n");
      }
     server(seed, fd_toParent, fd_toChild);
    }
}else{
  player(1, fd_toChild[seed][PIPE_READ], fd_toParent[seed][PIPE_WRITE]);
}
if (DEBUG) printf("Out of proc making labrynth!\n");

// the parent is really running the server 
// and players and not participating in the game
wait(0);
return 0;
}// end main

