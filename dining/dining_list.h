#ifndef dining_list_h
#define dining_list_h


typedef struct _list * LIST;

// creates and initializes a new list
LIST listCreate(int maxElems);

//removes an element with given id
void listRemove(LIST L, int id);

//adds element to rear of list
void listAddElem(LIST L, int id);

//returns id of front elem, or -1 if no elems..  I hope
int listPeek(LIST L);

//returns the size of the list
int listSze(LIST L);

// destroys a list
void listDestroy(LIST L);

#endif

