#include <stdio.h>

#define cls() printf("\033[H\033[J");
#define position(row,col) printf( "\033[%d;%dH", (row), (col) )


int main(){
cls();

printInt(5,5,10);
printStr(6,5,'ohai');

}

void printInt(int row, int col, int num){
position(row, col);
printf("%d", num);
}

void printStr(int row, int col, char txt){
  position(row, col);
  printf("%s", txt);
}

}
