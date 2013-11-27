#include <stdio.h>
#include <stdlib.h>
#include "dining_list.h"

//trailing is the elem in front, leading is the elem in back
typedef struct {
//	int trailingID;
//	int leadingID;
	int myID;}
ELEMENT;

typedef struct _list{
	int capacity; 
	int size; 
	int front; 
	int rear; 
	ELEMENT * elems;
} LIST_HEAD, *LIST;

//initializes the list
LIST listCreate(int maxElems){
	LIST L; //pointer to self

	//initing 
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
	L->rear = -1;//will incr when first Elem is added

	return L;
}

//removes an element with given id
//and assigns prior elem to following elem

int listRemoveElem(LIST L, int findID){
  if(L->size == 0){
    fprintf(stderr, "There's no Elements left in your list!");
  }else{
    L->size--;//we'll just ignore the final element
    int i,j;
    for(i=0; i<L->size; i++){
      if (L->elems[i].myID == findID){//only true once per trip through
        //move all the elems down, erasing the current
        for(j=i; j<L->size; j++){
          L->elems[j] = L->elems[j+1];
        }
        return i;
      }
    }
  }
}

//adds element to rear
void listAddElem(LIST L, int id){
	if( L->size == L->capacity){//no vacancies
		fprintf(stderr, "No room in the list\n");
		return;
	}else{
  	L->size = L->size++;
	  L->rear = L->rear++;
	  L->elems[L->rear].myID= id;
  }
}
//returns id of front elem, or -1 if no elems..  I hope
int listPeek(LIST L){
	if (L->size==0) return -1;
	return L->elems[L->front].myID;
}

//returns the size of the list
int listSize(LIST L){
	return L->size;
}

void listDestroy(LIST L){
	if ( L->elems ) free( L->elems );
	if ( L ) free( L );
}

void listPrint(LIST L){
  int i;
  for(i=0; i<L->size; i++){
    printf("%d ", L->elems[i].myID);
  }
  printf("\n");
}
/*
int main(){

  LIST myList = listCreate(5);

  listAddElem(myList, 3);
  listAddElem(myList, 1);
  listAddElem(myList, 2);
  listAddElem(myList, 4);
  listAddElem(myList, 0);

  listRemoveElem(myList, 2);  
  listPrint(myList);
  listRemoveElem(myList, 1);
  listPrint(myList);
}
*/

