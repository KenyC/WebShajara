#ifndef TREE_PARSE_H
#define TREE_PARSE_H

#include "tree.h"

typedef int (*parse_t)(const char* string_tree, Tree*	);

int parse_qtree (const char* string_tree, Tree*);
int parse_forest(const char* string_tree, Tree*);
#endif