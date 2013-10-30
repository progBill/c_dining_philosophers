#include <stdio.h>
#include <stdlib.h>

void reporter(){
  printf("%d reporting!\n", getpid());
}

void writer(){
  printf("%d the big kahuna!\n", getpid());
}

int main(){
int pid, i, pids[10];

// creates server & human interface
pid = fork();
if (pid>0){// only the server piece should do this
  for (i=0; i < 5; ++i){// create X times
    if((pids[i] = fork()) == 0){// only kids from this fork get made 
      reporter();
      exit(0);
    }
  }
}else{
  writer();
}

// the parent is really running the server 
// and players and not participating in the game
wait(0);
return 0;  
}

