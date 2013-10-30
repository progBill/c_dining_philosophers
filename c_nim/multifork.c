#include <stdio.h>

void reporter(){
  printf("%d reporting!\n", getpid());
}

void writer(){
  printf("I'm the big kahuna!");
}

int main(){
int pid, i;

pid = fork();
if (pid>0){
  for (i=0; i < 5; ++i){
    reporter();
  }
}else{
  writer();
}

return 0;  
}
