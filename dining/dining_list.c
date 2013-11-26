#include <stdio.h>
#include <stdlib.h>
#include "dining_list.h"

//trailing is the elem in front, leading is the elem in back
typedef struct {
	int trailingID;
	int leadingID;
	int myID;}
ELEMENT;

typedef struct _list{
	int capacity; 
	int size; 
	int front; 
	int rear; 
	ELEMENT * elems;
} LIST_HEAD;

//initializes the list
LIST listCreate(int maxElems){
	LIST L; //pointer to self

	//initing 
	L = (LIST)malloc(sizeof(LIST_HEAD)); 
	if (!L){
		fprintf(stderr, "Memory allocation error - LIST\n");
		exit(0);
	}
	/*
	L->elems = (ELEMENT*) calloc(maxElems, sizeof(ELEMENT));
	if (!L->elems){
		fprintf(stderr, "Memory allocation error - LIST\n");
		exit(0);
	}
	*/
	L->elems = ELEMENT[maxElems];
	L->size = 0;
	L->capacity = maxElems;
	L->front = 0;
	L->rear = -1;//will incr when first Elem is added

	return L;
}

//removes an element with given id
//and assigns it's head to it's tail's head

void listRemoveElem(LIST L, int id){

	/***********************
	
		You'll need to find id in the L->elems array.
		Then do what you need to do.
		You are using L->elems as a circular Q, so you just can't remove from 
		the middle without moving items to fill the "hole"
		
	****************************/

}

//adds element to rear
void listAddElem(LIST L, int id){
	if( L->size == L->capacity){//no vacancies
		fprintf(stderr, "No room in the list\n");
		return;
	}
	
	L->size = L->size + 1;
	L->rear = (L->rear + 1) % (L->capacity);
	
	//add id to back of line
	//L->ELEMS is an array.  Access fields with "."
	 
	/********************************
	
	Not sure what to do with the other fields in the L->elems ELEMENT array
	
	********************************/
	
	L->elems[L->rear].myID= id;
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


