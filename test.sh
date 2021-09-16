#!/bin/bash

#force make re
make re
clang -g3 main.c debug_lib/libft_malloc_printf.a

export DYLD_LIBRARY_PATH=.
export DYLD_FORCE_FLAT_NAMESPACE=1
export DYLD_INSERT_LIBRARIES="libft_malloc.so"
export LD_PRELOAD=`pwd`/$DYLD_INSERT_LIBRARIES

$@
