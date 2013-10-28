#include <stdio.h> //this provides all the basic input / output stuff
#include <unistd.h>
#include <stdlib.h>

//these constants exist to make later code more readable
const int PIPE_READ=0; 
const int PIPE_WRITE=1;

//a construct to store data for the processes to read between them
struct _msg {
  char cmd; //used in proc's branching logic (switch, if/then, what-have-you)
  char * data; //the content of the message, for performing operations upon, here it's an int
}; //don't forget to terminate after the closing curly bracket on struct

typedef struct _msg MESSAGE; // sets MESSAGE to type _msg
//below: macro to easily define message to pass
#define DEF_MSG(m,s,d) {m.cmd=s;m.data=d;}
MESSAGE msg; // msg buffer

void parent(int READ, int WRITE){
  
  //read is blocking, so when this line is fired, the proc will sit and wait for a message in the pipe
  //whatever you do, don't have 2 procs trying to read..  you'll just sit there staring at nothing happening
  //I paid for this knowledge in blood and sleepless nights
  read(READ, &msg, sizeof(msg));

  //the recieved message can be accessed via dot notations.  
  //the field to access is named in the struct def above.
  printf("Ooohh..  user picked: %s.  It's like magic, I know right!\n", msg.data);

}

void child(int READ, int WRITE){
  char * dirList;
  execl("/bin/ls", "/bin/ls","-l","-s", "-a", (char*) 0);

  DEF_MSG(msg, 'n', dirList); 
  write(WRITE, &msg, sizeof(msg));

}

int main(){

// laying some pipe -- note they are arrays
int fd_pipeToChild[2];
int fd_pipeToParent[2];
int pid; // for differentiating parent from child

// turn those pretty arrays into actual virtual pipes
pipe(fd_pipeToChild);
pipe(fd_pipeToParent);

pid = fork(); // create kiddie proc -- ::::NOTE::::remember the code from here on is run by both procs

if (pid < 0){// danger will robinson! -- fork failed
  printf("We've a problem with a series of tubes!!\n");
}
else if(pid > 0){ // parent code here
  // close the part of the pipe the parent won't use
  // parent won't read from toChild, won't write toParent
  close(fd_pipeToChild[PIPE_READ]); // this is why we made constants at the beginning
  close(fd_pipeToParent[PIPE_WRITE]);
  //create the parent, give it the ends of the pipe it will be using
  parent(fd_pipeToParent[PIPE_READ], fd_pipeToChild[PIPE_WRITE]);
}
else{ //child here.. do the same stuff as above, only, you know..  child safe

  //close what the child won't use now
  close(fd_pipeToChild[PIPE_WRITE]);
  close(fd_pipeToParent[PIPE_READ]);
  //note the params below match the params in the func def above.
  child(fd_pipeToChild[PIPE_READ], fd_pipeToParent[PIPE_WRITE]);
  exit(0);
}

return 0; //always return 0 at the end of main, it's a signal to other procs that yours ended successfully
}

