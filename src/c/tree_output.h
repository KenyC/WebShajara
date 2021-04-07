#ifndef TREE_OUTPUT_H
#define TREE_OUTPUT_H

#include "tree.h"

typedef char* (*output_t)(const Tree*, char* string_buffer);


/*
These output methods contain pointers to buffer with the string version of the tree
Will return "string_buffer" if enough space in "string_buffer"; otherwise reallocates
*/
char* output_qtree  (const Tree*, char *string_buffer);
char* output_forest (const Tree*, char *string_buffer);


#endif