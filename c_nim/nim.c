#include <stdio.h>
#include <sys/wait.h>

// pipes
const int PIPE_READ = 0;
const int PIPE_WRITE= 1;
const int DEBUG= 1;


// message structs
struct _msg {
  char cmd;
  int data;
};

typedef struct _msg MESSAGE;

char * error[] = {
  "GET WITH IT, no negative numbers!\n",
  "Really?  Up to 3, not whatever you picked...\n"
};

#define DEF_MSG(m,s,d) {m.cmd=s; m.data=d; }
MESSAGE msg;
enum _state{READY, PLAY, QUIT};
typedef enum _state STATE;

//
//  PARENT CODE: PARENT ENFORCES RULES, HOLDS GAME STATE 
//
void parent(int numSticks, int READ, int WRITE){
  STATE state;  
  state=READY;

  printf("numSticks: %d\n", numSticks);

  while(state!=QUIT){//parent main loop
    //get kids messages
    read(READ, &msg, sizeof(msg));
    printf("parent: msg.cmd: %c msg.data: %d\n", msg.cmd, msg.data);

    // state behavior below
    switch(state){
      // ready but not playing
      case READY:
        if (DEBUG) printf("Parent Ready!\n");
        if (msg.cmd == 'p'){
          state=PLAY;
          DEF_MSG( msg, 'p', numSticks );
        }
        break;
      // there're sticks on the ground, good to play
      case PLAY:
        if (DEBUG) printf("Parent Playing\n");
        if (msg.cmd == 'p'){
          numSticks -= msg.data;
          DEF_MSG( msg, 'p', numSticks );
          if (numSticks < 1) state=QUIT;
        }
        else if(msg.cmd == 'o'){
          state=QUIT;
          DEF_MSG( msg , 'o', 0 );
        }
        break;
      // we're done, either someone won or player quit
      case QUIT:
        if (DEBUG) printf("Parent done with your crap.\n");
        DEF_MSG( msg, 'o', 0);
        break;
      default:
        printf("Oh god how did this get in here I am not good with computers\n");
        state=QUIT;
        break;
      }
      //leave a message for kids
      if(DEBUG) printf("Parent writing pipe!\n");
      write(WRITE, &msg, numSticks);
  }
  if(DEBUG) printf("Parent out of while!\n");
  close(READ);
  close(WRITE);
}

//
//  CHILD CODE: CHILDREN REPRESENT PLAYERS
//
void child(int isPlayer, int READ, int WRITE){
  char c[2];
  int numToTake, pick;
  read(READ, &msg, sizeof(msg));

  if(DEBUG) printf("Child active\n");

  while (msg.cmd != 'o'){//child main loop

    //get a message from our parent
    read(READ, &msg, sizeof(msg));

    if(msg.cmd=='p'){
      if (isPlayer){
        printf("There are %d sticks left.\n", msg.data);
        printf("How many sticks do you want to pick up? (0 to quit)\n");
        scanf("%d\n", &pick );
      } else {
        pick=3;
      }
    
      //write the message for the parent to read
      if(pick < 0){ DEF_MSG(msg, 'e', 0); }
      else if (pick == 0){ DEF_MSG(msg, 'o', 0); }
      else if (pick == 1){ DEF_MSG(msg, 'p', 1); }
      else if (pick == 2){ DEF_MSG(msg, 'p', 2); }
      else if (pick == 3){ DEF_MSG(msg, 'p', 3); }
      else if (pick > 3){ DEF_MSG(msg, 'e', 1); }
    }
    else if(msg.cmd== 'o'){
      DEF_MSG( msg, 'o', 0 );
    }
    //leave a message for our parent
    write(WRITE, &msg, sizeof(msg));

  }
  close(READ);
  close(WRITE);
}

//
//  MAIN METHOD
//
int main(){

  pid_t pid;
  int numPlayers;
  // pipes
  int fd_toChild[2], fd_toParent[2];
  
  pipe(fd_toChild);
  pipe(fd_toParent);

  printf("Welcome to the Game of Nim!\n\nThe game consists of a pile of sticks (virtual in this case). On each\
  \nplayer's turn, they pick up 1, 2 or 3 sticks.  The last player to \npick up sticks wins. Sweet, huh?\
  \nNow, how many opponents would you like to play against (minimum 2)?\n");
  scanf("%d", &numPlayers);//this'll be in a for loop, or some such

  pid = fork();

  if (pid < 0){ fprintf(stderr, "fork failed OMGz!\n"); }
  else if (pid > 0){ // parent code here
    if(DEBUG) printf("Making parent\n");
    close( fd_toChild[PIPE_READ] );
    close( fd_toParent[PIPE_WRITE] );
    parent(4*numPlayers, fd_toParent[PIPE_READ], fd_toChild[PIPE_WRITE]);
    wait(0);  
    printf("Parent out.\n");
  }
  else if (!pid){//child code here
    if(DEBUG) printf("Making babies!\n");
    close( fd_toChild[PIPE_WRITE] );
    close( fd_toParent[PIPE_READ] );
    child(1, fd_toChild[PIPE_READ], fd_toParent[PIPE_WRITE]);
    printf("Kid done!\n");
  }

// all done!
return 0;
}

