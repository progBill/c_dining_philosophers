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
void parent(int seed, int READ,int WRITE){
  STATE state;
  state=RUNNING;
  int runningTotal=seed;

  printf("runningTotal: %d\n", runningTotal);

  while(state != QUIT){
    read(READ, &msg, sizeof(msg));
    
    switch(state){
      case RUNNING:
        if (msg.cmd == 'a'){
          runningTotal += msg.data;
        }else if (msg.cmd == 's'){
          runningTotal -= msg.data;
        } 
        DEF_MSG(msg, 'd', runningTotal);
        break;
      case QUIT:
        DEF_MSG(msg, 'q', 0);
        break;
    } //end switch
    write(WRITE, &msg, sizeof(msg));
  } //end while
}

//
//  CHILD CODE
//
void child(int READ, int WRITE){
  int running=1;  
  int numDelta;

  DEF_MSG(msg, 'a', 0);
  write(WRITE, &msg, sizeof(msg));
  
  while(running){
    read(READ, &msg, sizeof(msg));
    printf("curNum: %d\n", msg.data);
    printf("By much should the number changed?\n");
    scanf("%d", &numDelta);
// testing sign
    if (numDelta > 0){
      printf("Positive, I'll add %d\n", numDelta);
    }else if (numDelta < 0){
      printf("Negative, we'll add %d\n", numDelta);
    }else{
      printf("No change? alright, I'll just waste these flops, then.\n", numDelta);
    }
//
    if (msg.data > 0 && msg.data < 1000){
      if (msg.cmd == 'd'){
        if(numDelta > 0){
          DEF_MSG(msg, 'a', numDelta);
        }else{     
          DEF_MSG(msg, 's', numDelta);  
        }
        write(WRITE, &msg, sizeof(msg));
     }else{//got some weird ass command
        running=0;
      }
    }else{// number is too high
      running=0;
    }
  } //end while
}

//
// MAIN
//
int main(){
  int fd_toChild[2], fd_toParent[2];
  int pid, seed;

  pipe(fd_toChild);
  pipe(fd_toParent);

  printf("What would you like the seed number to be?\n");
  scanf("%d", &seed);

  pid = fork();

  if (pid < 0){
    fprintf(stderr, "No fork for you!\n");
    return -1;
  }else if(pid > 0){
    //parent here
    close(fd_toChild[PIPE_READ]);
    close(fd_toParent[PIPE_WRITE]);
    parent(seed, fd_toParent[PIPE_READ], fd_toChild[PIPE_WRITE]);
    wait(0);
    printf("Parent closed!\n");
  }else{
    //child here
    close(fd_toChild[PIPE_WRITE]);
    close(fd_toParent[PIPE_READ]);
    child(fd_toChild[PIPE_READ], fd_toParent[PIPE_WRITE]);
    printf("Child Closed!\n");
}
}

