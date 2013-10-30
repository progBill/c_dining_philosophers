#include <stdio.h>
#include <stdlib.h>

void parent(int forkNum){
  printf("Poppy says: %d :: %d\n", getpid(), forkNum);
}

void child(int forkNum){
  printf("\tNew kid: %d :: %d\n", getpid(), forkNum);
}

int main() {
    int i, isParent=1, numOpps;

    printf("How many opponents?");
    scanf("%1d", &numOpps);

    int pid[numOpps], parPlay = fork();

    // make the parent as server, first child as human player
    if (parPlay > 0) {
      parent(parPlay);
      for(i=0; i<numOpps; ++i){ // number of opponents
        printf("i=%d\n",i); 
        if((pid[i]=fork())==-1) exit(1);
        else if(pid[i] ==0) child(pid[i]);
        else if(pid[i] > 0) break;
      }
    }else child(parPlay);// the human
    return 0;
}

