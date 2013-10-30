#include <stdio.h>

//pipe constants for readability
const int PIPE_READ=0;
const int PIPE_WRITE=1;

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
// PARENT CODE
//
void server(int runningTotal, int READ,int WRITE){
  STATE state;
  state=RUNNING;

  while(state != QUIT){
    read(READ, &msg, sizeof(msg));
    
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
    write(WRITE, &msg, sizeof(msg));
  } //end while
}

//
//  CHILD CODE
//
void child(int isHuman, int READ, int WRITE){
  int running=1;  
  int numDelta;

  DEF_MSG(msg, 'p', 0);
  write(WRITE, &msg, sizeof(msg));
  
  while(running){
    read(READ, &msg, sizeof(msg));
    
    printf("%d: server sent command %d with data: %d.\n", getpid(), msg.cmd, msg.data);

    if(msg.cmd != 'q'){
      printf("There are %d sticks left.\n", msg.data);
      printf("Pick up how many sticks?\n");
      scanf("%d", &numDelta);
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
  int fd_toChild[2], fd_toParent[2];
  int pid, seed, human=1;
  
  pipe(fd_toChild);
  pipe(fd_toParent);

  printf("How many opponents would you like to play against?\n");
  scanf("%d", &seed);

  pid = fork();
  printf("%d has arrived!\n", getpid());

  if (pid < 0){
    fprintf(stderr, "No fork for you!\n");
    return -1;
  }else if(pid > 0){
    for(i=0; i < seed; ++i){
      //make players here
    }
  }else{
    // make server here
  }
/*
    //server here
    close(fd_toChild[PIPE_READ]);
    close(fd_toParent[PIPE_WRITE]);
    server(seed*4, fd_toParent[PIPE_READ], fd_toChild[PIPE_WRITE]);
    wait(0);
    printf("Parent closed!\n");
  }else{
    //child here
    close(fd_toChild[PIPE_WRITE]);
    close(fd_toParent[PIPE_READ]);
    child(1, fd_toChild[PIPE_READ], fd_toParent[PIPE_WRITE]);
    printf("Child Closed!\n");
  }
*/
}// end main
