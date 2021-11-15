# Malloc
Malloc is a simple project that allow you to....malloc !

## Subject

The subject is available in French [here](./fr.subject.pdf)

## How it works ?

Malloc use a function called ```mmap()``` to pre-allocate 4096 bytes, then split it into small parts.

This malloc use 2 linked lists, one for handling 'blocks', and one for allocations

If you are trying to understand how this malloc works, there is a branch called [with_debug_print](https://github.com/lubenard/malloc/tree/with_debug_print) who might help you in this adventure !

## Test it ! 

First, you need to compile it by taping ```make```

Then, you can execute ```./test.sh [A PROGRAM]```

This syntax is also valid in Linux ```LD_PRELOAD=./libft_malloc.so [A PROGRAM]```

# Enjoy !
