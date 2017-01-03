#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include "cache.h"


/* Here is an our naive implementation */
char rotate_descr[] = "Naive Row-wise Traversal of src";
void rotate(int dim, pixel *src, pixel *dst) {
	int i, j;

	for(i=0; i < dim; i++) {
		for(j=0; j < dim; j++) {
			COPY(&dst[PIXEL(dim-1-j,i,dim)], &src[PIXEL(i,j,dim)]);
		}
	}

	return;
}


/* Add additional functions to test here */
char optimized_descr[] = "Optimized rotation";
void optimized_rotate(int dim, pixel *src, pixel *dst) {
    int block_size = 4;
    int i, j, k, l;
    for (l = 0; l < dim; l += block_size)
        for (k = 0; k < dim; k += block_size)
            for (j = l + block_size - 1; j >= l; j--)
                for (i = k; i < k + block_size; i++)
                    COPY(&dst[PIXEL(dim-1-j,i,dim)], &src[PIXEL(i,j,dim)]);
}

void register_rotate_functions() {
	add_rotate_function(&rotate, rotate_descr);
    add_rotate_function(&optimized_rotate, optimized_descr);	
}

