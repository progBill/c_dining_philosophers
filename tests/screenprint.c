#include <stdio.h>

#define cls() printf("\033[H\033[J");
#define position(row,col) printf( "\033[%d;%dH", (row), (col) )


int main(){
cls();

int i;
for(i = 0; i < 15; i++){
  position(5, 5);
  printf("i: %d\n", i); 
  sleep(1);
}

}
