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

//Static array for use as dynamic memory; our "heap"
//static char myblock[5000];

typedef long Align;                      /* for alignment to long boundary */

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


static Header base;                /* empty list to get started */
static Header* freeptr = NULL;           /* start of free list */

static Header *morecore(unsigned nu);    /* declare morecore now */

/* malloc: general-purpose storage allocator */
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


#define NALLOC 1024 /* minimum #units to request */
/* morecore: ask system for more memory */
static Header *morecore(unsigned nu)
{

  void *cp;
  Header *up;
  int *sbrk(size_t);

  if (nu < NALLOC)
  {
    nu = NALLOC;
  }

  cp = (void *) sbrk(nu * sizeof(Header));

  if (cp == (char *) -1){ /* no space at all */
    return NULL;
  }

  up = (Header *) cp;
  up->s.size = nu;
  free((void *)(up+1));

  return up;
}
