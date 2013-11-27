#include <stdio.h>
#include "sortof.h"

void main(){
  int myArr[5] = {3,65,3,2,6};

  insertion(myArr, 5);

  int i;
  for(i=0; i<5; i++) printf("%d ", *(myArr + i));
}

