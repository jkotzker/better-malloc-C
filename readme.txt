The file memlib.h contains the function my_malloc() and my_free(), standings for the memory allocation and freeing programs included in the standard C library.
Also included is the function morecore(), which requests more memory from the system in the case where the user attempts to allocate memory on a full heap.

The functions themselves are based heavily on the functions written in K&R C, Second Edition, on pages 185-189.
I also made use of the stackoverflow question here: http://stackoverflow.com/questions/13159564/explain-this-implementation-of-malloc-from-the-kr-book.
This was not my question, but the code and explanation provided by stackoverflow.com/users/584518/lundin there were extremely helpful in building off of and better understanding the code included in K&R C.

The function my_malloc allocates memory for the user when called, allocates a block of the requested size, and returns a pointer to it.
Like the regular malloc function, it will return a NULL pointer when it fails.
It will fail in the case where there is no more dynamic memory available to be allocated
(i.e., where morecore determines that that there is less memory available than the minimum size for a block.)

my_free is passed a pointer to a block of memory. It first checks to see if the block is currently allocated, or has been freed already, using recognition patters the library functions write into the block at different points.
If my_free detects that it has neither been allocated nor freed, it will refuse to free it and assume that it was allocated or freed by another program.
If it detects that it has already been freed by my_free, it will refuse to free it again.
If it detects that it is currently allocated, it will free it by placing the block in the free list.

The ErrorHandler function is used to catch errors thrown in the other library functions and output them to the user with useful information.
The function catches the above errors in my_free, as well as the error when there is no more dynamic memory available to be allocated.

 A test program mallocme.c is included.
