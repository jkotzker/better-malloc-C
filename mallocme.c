/************************************************************************/
/* mallocme.c                                                           */
/* Test program for implementation of malloc and free found in memlib.h */
/************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include "memlib.h"

#define malloc( x )     my_malloc( x, __FILE__, __LINE__ )
#define free( x )       my_free( x, 0, __FILE__, __LINE__ )

typedef struct mystruct {
    int i1;
    int i2;
} s;

int main(int argc, char **argv) {
    if(argc != 3)
    {
        printf("ERROR: Expected input in form of 2 integers.\n");
        return 1;
    }
    else
    {
        char * a = argv[1];
        char * b = argv[2];
        int curr1 = *a - '0';
        int curr2 = *b - '0';
        printf("Allocating memory for a new struct...\n");
        s * mynew = (s *) malloc(sizeof(s));
        if( mynew != NULL )
        {
            printf("Malloc succeeded, adding data...\n");
            mynew->i1 = curr1;
            mynew->i2 = curr2;
            printf("Data added. Trying to access...\n");
            printf("First datum: %i\n", mynew->i1);
            printf("Second datum: %i\n", mynew->i2);
        }
        printf("Freeing struct...\n");
        free(mynew);

        printf("Now, let's test some errors...\n");

        printf("First error test...\n");
        free(mynew);

        printf("Second error test...\n");
        s * mynon;
        free(mynon);

        char c = 'd';
        char * cptr = &c;
        free(cptr);


        printf("Exiting...\n");
        return 0;
    }
}
