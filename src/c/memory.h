#ifndef MEMORY_H
#define MEMORY_H

#define WITH_ARRAY(TYPE, NAME, SIZE, SCOPE) { \
	TYPE* NAME = malloc(SIZE * sizeof(TYPE)); \
	SCOPE;                                    \
	free(NAME);                               \
}

#define REALLOC(POINTER, SIZE, TYPE) POINTER = realloc((POINTER), (SIZE) * sizeof(TYPE))
#define ALLOC(POINTER, SIZE, TYPE)   POINTER = malloc(SIZE * sizeof(TYPE))

#endif