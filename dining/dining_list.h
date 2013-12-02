#ifndef dining_list_h
#define dining_list_h

typedef struct {
  int right;
  int left;
  int myID;
}ELEMENT;

typedef struct _list{
  int capacity;
  int size;
  int front;
  int rear;
  ELEMENT * elems;
} LIST_HEAD, *LIST;


//typedef struct _elem * ELEMENT;
//typedef struct _list * LIST;

// creates and initializes a new list
LIST listCreate(int maxElems);

//removes an element with given id
void listRemove(LIST L, ELEMENT elem);

//adds element to rear of list
void listAddElem(LIST L, ELEMENT newElem);

//given an elem, find next elem in line that can fairly eat
int listGetNext(LIST L, ELEMENT elem);

//returns the size of the list
int listSze(LIST L);

// destroys a list
void listDestroy(LIST L);

#endif

