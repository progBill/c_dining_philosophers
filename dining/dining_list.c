#include <stdio.h>
#include <stdlib.h>
#include "dining_list.h"

//initializes the list
LIST listCreate(int maxElems){
	LIST L; //pointer to self

	L = (LIST)malloc(sizeof(LIST_HEAD)); 
	if (!L){
		fprintf(stderr, "Memory allocation error - LIST\n");
		exit(0);
	}
	L->elems = (ELEMENT*) calloc(maxElems, sizeof(ELEMENT));
	if (!L->elems){
		fprintf(stderr, "Memory allocation error - LIST\n");
		exit(0);
	}
	L->size = 0;
	L->capacity = maxElems;
	L->front = 0;
	L->rear = -1;//will incr when zeroth Elem is added

	return L;
}

//removes an element with given id
int listRemoveElem(LIST L, ELEMENT elem){
  if(L->size == 0){
    fprintf(stderr, "There's no Elements left in your list!");
  }else{
    //L->size--;//we'll just ignore the final element
    int i,j;
    for(i=0; i<L->size; i++){
      if (L->elems[i].myID == elem.myID){//only true once per trip through
        //move all the elems down, erasing the current
        for(j=i; j<L->size-1; j++){
          L->elems[j] = L->elems[j+1];
        }
        L->size--;
        L->rear--;
        return elem.myID;
      }
    }
  }
}

//adds element to rear
void listAddElem(LIST L, ELEMENT newElem){
	if( L->size == L->capacity){//no vacancies
		fprintf(stderr, "No room in the list\n");
		return;
	}else{
  	L->size = L->size++;
	  L->rear = L->rear++;
	  L->elems[L->rear] = newElem;
  }
}

//given an elem, return the id of the next legal eating partner
int listGetNext(LIST L, ELEMENT elem ){
  int i;
  for(i=0;i<L->size;i++){// loop through every elem, starting from the front
    if (L->elems[i].myID != elem.right && L->elems[i].myID != elem.left){//current inspected elem can eat with current eater
      int j, conflict=0;//declared in scope so we don't have to reset 
      for(j=i; j>=0;j--){//check from current back to the front, to make sure current inspected doesn't conflict with prior elems
        if(L->elems[j].myID == L->elems[i].right || L->elems[j].myID == L->elems[i].left) conflict++;
      }// end j loop
      if (conflict == 0) return L->elems[i].myID;  
    }// end i i
  }// end i for
  return -1; //no legal eating partners found
}

//returns the size of the list
int listSize(LIST L){
	return L->size;
}

//free everything up
void listDestroy(LIST L){
	if ( L->elems ) free( L->elems );
	if ( L ) free( L );
}

//just prints the list in order from front-to-back
void listPrint(LIST L){
  int i, val;
  for(i=0; i<L->capacity; i++){
    if(i < L->size){
     printf("\033[%d;5Helem%d:%d", (i + 10), i, L->elems[i].myID);
    }else{
     printf("\033[%d;5Helem%d:%d", (i + 10), i, 9);

    }
  }
  printf("\n");
}

/*
int main(){

  LIST myList = listCreate(5);
  ELEMENT zeroth = {4,1,0};
  ELEMENT first= {0,2,1};
  ELEMENT second = {1,3,2};
  ELEMENT third= {2,4,3};
  ELEMENT fourth = {3,0,4};

  //listAddElem(myList, zeroth);
  listAddElem(myList, first);
  listAddElem(myList, second);
  listAddElem(myList, third);
  listAddElem(myList, fourth);
 
  listPrint(myList);
 
  printf("Next legal for 0: %d\n", listGetNext(myList, zeroth.right, zeroth.left));
  printf("Next legal for 1: %d\n", listGetNext(myList, first.right, first.left));
  printf("Next legal for 2: %d\n", listGetNext(myList, second.right, second.left));
  printf("Next legal for 3: %d\n", listGetNext(myList, third.right, third.left));
  printf("Next legal for 4: %d\n", listGetNext(myList, fourth.right, fourth.left));
}
*/

