#include <stdio.h> //this provides all the basic input / output stuff

//these constants exist to make later code more readable
const int PIPE_READ=0; 
const int PIPE_WRITE=1;

//a construct to store data for the processes to read between them
struct _msg {
  char cmd; //used in proc's branching logic (switch, if/then, what-have-you)
  int data; //the content of the message, for performing operations upon, here it's an int
}; //don't forget to terminate after the closing curly bracket on struct

typedef struct _msg MESSAGE; // sets MESSAGE to type _msg
//below: macro to easily define message to pass
#define DEF_MSG(m,s,d) {m.cmd=s;m.data=d;}
MESSAGE msg; // msg buffer

void parent(int fingerRead, int fingerWrite, int totalRead, int totalWrite){
  int running = 1, incFingers, incTotal, myFingers;

  while (running){
    myFingers = 3;

    read(fingerRead, &msg, sizeof(msg));
    if (msg.cmd == 'f') {
      incFingers = msg.data; 
      if (msg.data == '0') running = 0;
    }

    read(totalRead, &msg, sizeof(msg));
    if (msg.cmd == 't') incTotal = msg.data;

    printf("incFingers: %d total Guess: %d\n", incFingers, incTotal);

    DEF_MSG(msg, 'r', myFingers + incFingers);
    write(fingerWrite, &msg, sizeof(msg));

    DEF_MSG(msg, 't', 6);
    write(totalWrite, &msg,sizeof(msg));

    }
}

void child(int fingerRead,  int fingerWrite, int totalRead, int totalWrite){
  int running=1, numFingers, total, realTot, srvGuess;

  while(running){
    // get players throw, write it to pipe
    printf("How many fingers to throw?  > ");
    scanf("%d\n", &numFingers);
    DEF_MSG(msg, 'f', numFingers); // f is for fingers
    write(fingerWrite, &msg, sizeof(msg));

    // get players guess, write it to pipe
    printf("What do you think the total will be?  > ");
    scanf("%d\n", &total);
    DEF_MSG(msg, 't', total); //t = total
    write(totalWrite, &msg, sizeof(msg));

    // get actual total of both throws
    read(fingerRead, &msg, sizeof(msg));
    if (msg.cmd == 'q') running = 0;
    else realTot = msg.data;

    // get parent's guess
    read(totalRead, &msg, sizeof(msg));
    srvGuess = msg.data;

    if (total < srvGuess && srvGuess <= realTot){
      printf("Way to go, Loser.");
    }else if(total <= realTot) {
      printf("Yay, you're a winner!");
    }else if (total > realTot && srvGuess > realTot ){
      printf("There are no winners.");
    }else if(total < realTot && total == srvGuess){
      printf("You've tied!");
    }else{
      printf("I.. don't think it's even possible to be here.");
    }
    
  }

}

int main(){

// laying some pipe -- note they are arrays
int fd_pipeFinCntToChild[2];
int fd_pipeFinCntToParent[2];
int fd_pipeTotGuessToChild[2];
int fd_pipeTotGuessToParent[2];


int pid; // for differentiating parent from child

// turn those pretty arrays into actual virtual pipes
pipe(fd_pipeFinCntToChild);
pipe(fd_pipeFinCntToParent);
pipe(fd_pipeTotGuessToChild);
pipe(fd_pipeTotGuessToParent);

printf("Morra is ancient game played by Greek and Romans for much the same reason\n\
 we play rocks, paper, scissors. The rules are simple, at the same time \n\
all players throw their hand on the middle with any number of fingers extended \n\
(but not none). At the same time, the players announce the number they think \n\
all fingers will add up to. In our variation, closest without exceeding will win.  Have Fun!\n");

pid = fork(); // create kiddie proc -- ::::NOTE::::remember the code from here on is run by both procs

if (pid < 0){// danger will robinson! -- fork failed
  printf("We've a problem with a series of tubes!!\n");
}
else if(pid > 0){ // parent code here
  // close the part of the pipe the parent won't use
  // parent won't read from toChild, won't write toParent
  close(fd_pipeFinCntToChild[PIPE_READ]); // this is why we made constants at the beginning
  close(fd_pipeFinCntToParent[PIPE_WRITE]);
  close(fd_pipeTotGuessToChild[PIPE_READ]);
  close(fd_pipeTotGuessToParent[PIPE_WRITE]);

  //create the parent, give it the ends of the pipe it will be using
  parent(fd_pipeFinCntToParent[PIPE_READ],
         fd_pipeFinCntToChild[PIPE_WRITE],
         fd_pipeTotGuessToParent[PIPE_READ],
         fd_pipeTotGuessToChild[PIPE_WRITE]);
}
else{ //child here.. do the same stuff as above, only, you know..  child safe

  //close what the child won't use now
  close(fd_pipeFinCntToChild[PIPE_WRITE]);
  close(fd_pipeFinCntToParent[PIPE_READ]);
  close(fd_pipeTotGuessToChild[PIPE_WRITE]);
  close(fd_pipeTotGuessToParent[PIPE_READ]);


  //note the params below match the params in the func def above.
  child(fd_pipeFinCntToChild[PIPE_READ],
        fd_pipeFinCntToParent[PIPE_WRITE],
        fd_pipeTotGuessToChild[PIPE_READ], 
        fd_pipeTotGuessToParent[PIPE_WRITE]);
}

return 0; //always return 0 at the end of main, it's a signal to other procs that yours ended successfully
}

