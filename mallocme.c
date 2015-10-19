/************************************************************************/
/* mallocme.c                                                           */
/* Test program for implementation of malloc and free found in memlib.h */
/************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include "memlib.h"

#define malloc( x )     my_malloc( x, __FILE__, __LINE__ )
#define free( x )       my_free( x, __FILE__, __LINE__ )

typedef struct mystruct {
    int i;
    char c;
} s;

int main(int argc, char **argv) {
    if(argc != 3)
    {
        printf("ERROR: Expected input in form of 2 numbers.\n");
        return 1;
    }
    else
    {
        char * a = argv[1];
        char * b = argv[2];
        int curr = *a - '0';
        printf("Allocating memory for a new struct...\n");
        s * mynew = (s *) malloc(sizeof(s));
        if( mynew != NULL )
        {
            printf("Malloc succeeded, added data...\n");
            mynew->i = curr;
            mynew->c = *b;
            printf("Data added.\n");
        }
        printf("Freeing struct...\n");
        free(mynew);
        printf("Exiting...\n");
    }
}
