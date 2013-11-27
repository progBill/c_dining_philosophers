//#include <stdio.h>

/** INSERTION SORT
  * sorts an array in it's place inefficiently and not really intuitively
  * -main loop once per element in array
  * --in each loop, loop once each time current elem < left elem
  */
void insertion(int *incInts, int numCells){
  int i,h, holder;
  for (i=1, h=1;i<numCells;i++, h=i){
    while(*(incInts+h) < *(incInts+(h-1))){//as long as this elem is smaller than the one to it's left
      holder = *(incInts+h);
      *(incInts+h) = *(incInts+(h-1));
      *(incInts+(h-1)) = holder;
      h--;
    }
  }
}

/** SELECTION SORT 
  * finds the lowest value in array, puts it in the first position,
  * rinse, repeat
  *
  */
void selection(int *incInts, int numCells){
  int i, holder, curCell=0;
  while(curCell <= numCells){
    for(i=curCell; i>0; i--){
      if (*(incInts+i) < *(incInts+(i-1))){
        holder= *(incInts+i);
        *(incInts+i) = *(incInts+(i-1));
        *(incInts+(i-1)) = holder;
      }
    }
  curCell++;
  }  
}

/*

void main(){
  int numElems = 10;
  int intArr[10] = {1,5,4,7,3,9,2,6,0,8};

  int i;
  printf("Baseline: ");
  for (i = 0; i<10; i++) printf("%d ", *(intArr+i));
  printf("\n");

  //insertion(intArr, numElems);
  selection(intArr, numElems);

  printf("After sort: ");
  for (i = 0; i<10; i++) printf("%d ", *(intArr+i));
  printf("\n");

}

*/


