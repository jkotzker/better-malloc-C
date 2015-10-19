/******************************************************/
/* Dynamic Memory Management Implementation for C     */
/*                                                    */
/* Contains functions mymalloc() and myfree() for use */
/* managing memory from the heap.                     */
/*                                                    */
/* Brought to you by Joseph Kotzker                   */
/******************************************************/

/* Code from http://stackoverflow.com/questions/13159564/explain-this-implementation-of-malloc-from-the-kr-book */
/* was used as a starting point for this implementation */
/* Much thanks to http://stackoverflow.com/users/584518/lundin for his explanations there */
/* Code from K&R C was also used to implement this library */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>


typedef long Align;                      /* for alignment to long boundary */
typedef int ErrorCode;                   /* used for managing errors in the included functions */

/* Header is the type for a block of memory */

typedef union header                     /* block header */
{
    struct
    {
        union header *ptr;                   /* next block if on free list */
        size_t size;                         /* size of this block */
        unsigned int recognize;              /* recognition pattern to prevent freeing unallocated memory */
    } s;

    Align x;                               /* force alignment of blocks */

} Header;


static Header base;                      /* empty list to get started */
static Header* freeptr = NULL;           /* start of free list */

static Header *morecore(unsigned nu, const char file, const int line);    /* declare morecore now, morecore gets more dynamic memory from the system */
void my_free(void *ap, const char file, const int line);                  /* declare my_free now */


/* ErrorHandler function allows all included functions to throw errors to the handler, which provides useful feedback to the programmer */
void ErrorHandler (ErrorCode e, const char file, const char line)
{
    if(e == 1)
    {
        printf("Error at file:%c, line:%c\n", file, line);
        printf("ERROR: System has fewer than 1024 units of dynamic memory remaining! This is a problem; for your safety, you cannot be allocated any more space.\n");
        printf("Try checking your program for memory leaks.\n");
    }

    if(e == 2)
    {
        printf("Error at file:%c, line:%c\n", file, line);
        printf("ERROR: You have tried to free memory that was either never allocated, or allocated with a memory allocation program not provided by memlib.h.\n");
        printf("You can only use the included my_free() function to free memory allocated with the included my_malloc().\n");
    }
    if(e == 3)
    {
        printf("Error at file:%c, line:%c\n", file, line);
        printf("ERROR: You have already freed this block, and it has not been reallocated since.\n");
    }

    return;
}

/* my_malloc: general-purpose storage allocator, with built-in error handling */
void* my_malloc (size_t nbytes, const char file, const int line)
{
    Header*  p;
    Header*  prevptr;
    size_t   nunits;
    void*    result;
    bool     is_allocating;

    nunits = (nbytes + sizeof(Header) - 1) / sizeof(Header) + 1;

    prevptr = freeptr;
    if (prevptr == NULL)                   /* no free list yet */
    {
        base.s.ptr  = &base;
        freeptr     = &base;
        prevptr     = &base;
        base.s.size = 0;
        base.s.recognize = 0xAAAAAAAA;       /* set recognize to 0xAAAAAAAA every time allocated */
    }

    is_allocating = true;
    for (p = prevptr->s.ptr; is_allocating; p = p->s.ptr)
    {
        if (p->s.size >= nunits)             /* big enough */
        {
            if (p->s.size == nunits)           /* exactly */
            {
                prevptr->s.ptr = p->s.ptr;
            }
            else                               /* allocate tail end */
            {
                p->s.size -= nunits;
                p += p->s.size;
                p->s.size = nunits;
                p->s.recognize = 0xAAAAAAAA;     /* set recognize to 0xAAAAAAAA every time allocated */
            }

            freeptr = prevptr;
            result = p+1;
            is_allocating = false;             /* we are done */
        }

        if (p == freeptr)                    /* wrapped around free list */
        {
            p = morecore(nunits, file, line);
            if (p == NULL)
            {
                result = NULL;                   /* none left */
                is_allocating = false;
                ErrorHandler(1, file, line);
            }
        }
        prevptr = p;
    } /* for */

    return result;
}


#define NALLOC sizeof(Header) /* minimum #units to request is size of a single block */

/* morecore: ask system for more memory */
static Header *morecore(unsigned nunits, const char file, const int line)
{

    void *cp;                             /* pointer for new free memory added to the heap */
    Header *up;                           /* pointer to a Header that will be returned as the new space */
    int *sbrk(size_t);                    /* Declare function sbrk included from unistd.h */

    if (nunits < NALLOC)                  /* On the recommendation of K&R C, allocated at minimum 1024 units to the heap */
    {
        nunits = NALLOC;
    }

    cp = (void *) sbrk(nunits * sizeof(Header));      /* gets more memory from the system */

    if (cp == (char *) -1) {                           /* checks if there was no additional memory in the system */
        return NULL;
    }

    up = (Header *) cp;                   /* makes a Header of the requested size in the new memory and returns it */
    up->s.size = nunits;
    my_free((void *)(up+1), file, line);

    return up;
}

/* my_free, based off of K&R C */
/* my_free: put block ap in free list */
void my_free(void *ap, const char file, const int line) {
    Header *bp;                                                          /* bp will point to Header of block being freed */
    Header *p;
    bp = (Header *)ap - 1;                                               /* make bp point to block header */
    p = freeptr;

    unsigned int recog = bp->s.recognize;
    if(!(recog == 0xAAAAAAAA || recog == 0xBBBBBBBB))
    {
        ErrorHandler(2, file, line);
        return;
    }
    if(recog == 0xBBBBBBBB)
    {
        ErrorHandler(3, file, line);
        return;
    }
    else {
        recog = 0xBBBBBBBB;
    }

    while(!(bp > p && bp < p->s.ptr)) {
        if (p >= p->s.ptr && (bp > p || bp < p->s.ptr)) {
            break;                                                       /* freed block at start or end of arena */
        }
        p = p->s.ptr;
    }

    if (bp + bp->s.size == p->s.ptr) {
        bp->s.size += p->s.ptr->s.size;
        bp->s.ptr = p->s.ptr->s.ptr;
    }
    else {
        bp->s.ptr = p->s.ptr;
    }

    if (p + p->s.size == bp) {
        p->s.size += bp->s.size;
        p->s.ptr = bp->s.ptr;
    }
    else {
        p->s.ptr = bp;
    }
    freeptr = p;
    return;
}
