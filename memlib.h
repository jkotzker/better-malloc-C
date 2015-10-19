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

static Header *morecore(unsigned nu);    /* declare morecore now, morecore gets more dynamic memory from the system */
void my_free(void *ap);                  /* declare my_free now */

/* my_malloc: general-purpose storage allocator, with built-in error handling */
void* my_malloc (size_t nbytes)
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
            //TODO: add a function here that adds more memory to the heap
            p = morecore(nunits);
            if (p == NULL)
            {
                result = NULL;                   /* none left */
                is_allocating = false;
            }
        }
        prevptr = p;
    } /* for */

    return result;
}

/* This definition of NALLOC was recommended from K&R C book */
#define NALLOC 1024 /* minimum #units to request */

/* morecore: ask system for more memory */
static Header *morecore(unsigned nunits)
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
        //TODO: throw an error here
        return NULL;
    }

    up = (Header *) cp;                   /* makes a Header of the requested size in the new memory and returns it */
    up->s.size = nunits;
    my_free((void *)(up+1));

    return up;
}

/* my_free, based off of K&R C */
/* my_free: put block ap in free list */
void my_free(void *ap) {
    Header *bp;                                                          /* bp will point to Header of block being freed */
    Header *p;
    bp = (Header *)ap - 1;                                               /* make bp point to block header */
    p = freeptr;

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
}
