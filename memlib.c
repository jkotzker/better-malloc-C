/******************************************************/
/* Dynamic Memory Management Implementation for C     */
/*                                                    */
/* Contains functions mymalloc() and myfree() for use */
/* managing memory from the heap.                     */
/*                                                    */
/* Brought to you by Joseph Kotzker                   */
/******************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

//Static array for use as dynamic memory; our "heap"
static char myblock[5000];

struct mementryT_ {
    int size;
    void * pattern;
    char * start;
};
typedef struct mementryT_ mementryT;

//Malloc Function
void * mymalloc(unsigned int size);
void * free( char * p);
