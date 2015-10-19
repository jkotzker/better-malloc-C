/************************************************************************/
/* mallocme.c                                                           */
/* Test program for implementation of malloc and free found in memlib.h */
/************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include "memlib.h"

#define malloc( x )     my_malloc( x, __FILE__, __LINE__ )
#define free( x )       my_free( x, __FILE__, __LINE__ )

void my_free(void *ap, const char file, const int line);
void* my_malloc (size_t nbytes, const char file, const int line);

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
        s mynew = (s *) malloc(sizeof(s));
        s * sptr = &mynew;
        if(mynew)
        {
            printf("Malloc succeeded, added data...\n");
            sptr->i = curr;
            sptr->c = *b;
            printf("Data added.\n");
        }
        printf("Freeing struct...\n");
        my_free(sptr);
        printf("Exiting...\n");
    }
}
