#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "tree_parse.h"

#include "log.h"

#define LABEL_BUFFER 256


#define CURLY_PUT_CHAR(CHAR) *current_label_char = (CHAR); current_label_char++;
#define CURLY_MOVE_ONE (*(++(*current_char)))
int parse_curly_brackets(const char** current_char, char** label_buffer) {
	char *current_label_char = *label_buffer;
	while((**current_char != '{') && (**current_char != '\0')) {
		CURLY_MOVE_ONE;
	}
	if(**current_char == '\0') {return 0;}
	CURLY_MOVE_ONE;

	int open_brackets = 1;


	while(1) {
		if(**current_char == '\0') {return 0;}
		if(**current_char == '{')  {open_brackets++;}
		if(**current_char == '}')  {
			if ((--open_brackets) == 0)	{break;}
		}

		CURLY_PUT_CHAR(**current_char);
		CURLY_MOVE_ONE;
	}
	CURLY_PUT_CHAR('\0');
	// (*current_char)++;
	return 1;
}


/**************************************
QTREE
***************************************/


#define PARSE_MOVE_ONE       (++current_char)
#define CLEAN_UP()           free(label_buffer); free(stack); destroy_tree(&intermediate_tree);
#define FAIL_ON(CONDITION)   if((CONDITION)) {FAIL}
#define FAIL                 CLEAN_UP(); return 0;
#define INITIAL_SIZE_STACK   10
int parse_qtree(const char* string_tree, Tree *tree) {
	Tree        intermediate_tree;
	const char* current_char = string_tree;
	char*       label_buffer;
	int*        stack;
	int*        current_stack_pointer;
	int size_stack = INITIAL_SIZE_STACK;

	ALLOC(label_buffer, LABEL_BUFFER,       char);
	ALLOC(stack,        INITIAL_SIZE_STACK, int);
	init_tree(&intermediate_tree);


	// Move to first opening bracket
	while((*current_char != '[') && (*current_char != '\0')) {
		PARSE_MOVE_ONE;
	}
	// Fail if we can't find opening bracket
	FAIL_ON(*current_char != '[')

	// If root has label, parse
	if(*(PARSE_MOVE_ONE) == '.'){
		FAIL_ON(!parse_curly_brackets(&current_char, &label_buffer))
		set_label(&intermediate_tree, 0, label_buffer);
	}

	*stack                = 0;
	current_stack_pointer = stack;
	int not_done = 1;
	int child;

	while(not_done) {
		//-- Advance to next '[', '{' or ']' 
		while(
			(*current_char != '[') && // Beginning inner node
			(*current_char != ']') && // End inner node
			(*current_char != '{') && // Beginning leaf
			(*current_char != '\0')
		) {
			PARSE_MOVE_ONE;
		}

		switch(*current_char) {
			case '[':
				LOG("tree_parse.c (l. 85) : Begin [\n");
				//-- Adding inner node & push node onto stack
				child = add_child(&intermediate_tree, *current_stack_pointer);
				*(++current_stack_pointer) = child; 
				// LOG("Pushing %d\n", *current_stack_pointer);

				//-- If label present, parse label
				if(*(PARSE_MOVE_ONE)=='.') {
					FAIL_ON(!parse_curly_brackets(&current_char, &label_buffer))
					set_label(&intermediate_tree, *current_stack_pointer, label_buffer);
				}

				//-- Moving on
				PARSE_MOVE_ONE;

				break;

			case '{':
				LOG("tree_parse.c (l. 101) : Leaf\n");
				//-- Creating leaf node
				child = add_child(&intermediate_tree, *current_stack_pointer);

				//-- Parsing label
				FAIL_ON(!parse_curly_brackets(&current_char, &label_buffer))
				set_label(&intermediate_tree, child, label_buffer);

				//-- Moving on
				PARSE_MOVE_ONE;
				break;

			case ']':
				LOG("tree_parse.c (l. 114) : End ]\n");

				//-- If no node to pop, we've completed parsing
				// LOG("Popping %d\n", *current_stack_pointer);
				if(current_stack_pointer == stack) {
					not_done = 0;
				}
				else {
					//-- Pop node from stack
					current_stack_pointer--;
					PARSE_MOVE_ONE;
				}
				break;

			default:
				LOG("tree_parse.c (l. 114) : FAIL on %c\n", *current_char);
				FAIL;
		}

	}
	move_tree(tree, &intermediate_tree);
	CLEAN_UP();
	return 1;

}


/**************************************
FOREST
***************************************/


#define PARSE_MOVE_ONE         (++current_char)
#define PARSE_PUT_CHAR(CHAR)   *(current_label_char++) = (CHAR);
#define CLEAN_UP()           free(label_buffer); free(stack); destroy_tree(&intermediate_tree);
#define FAIL_ON(CONDITION)   if((CONDITION)) {FAIL}
#define FAIL                 CLEAN_UP(); return 0;



int parse_forest(const char* string_tree, Tree *tree) {
	Tree        intermediate_tree;
	const char* current_char = string_tree;
	char*       label_buffer;
	int*        stack;
	int*        current_stack_pointer;
	int size_stack = INITIAL_SIZE_STACK;

	ALLOC(label_buffer, LABEL_BUFFER,       char);
	ALLOC(stack,        INITIAL_SIZE_STACK, int);
	init_tree(&intermediate_tree);



	// Move to first opening bracket
	while((*current_char != '[') && (*current_char != '\0')) {
		PARSE_MOVE_ONE;
	}
	// Fail if we can't find opening bracket
	FAIL_ON(*current_char != '[')
	PARSE_MOVE_ONE;


	*stack                = 0;
	current_stack_pointer = stack;
	label_buffer[0]       = '\0';
	char* current_label_char = label_buffer;
	int   not_done = 1;
	int   child;

	int   previous_char  = 0;
	int   previous_space = 0;
	int   label_to_flush = 0;

	while(not_done) {
		//-- Advance to next '[', '{' or ']' 

		switch(*current_char) {

			case '[':
				LOG("tree_parse.c (l. 85) : Begin [\n");


				//-- Add label to parent
				PARSE_PUT_CHAR('\0');
				LOG("Label @ %d: \"%s\"\n", *current_stack_pointer, label_buffer);
				if(label_to_flush) {
					current_label_char--;
					current_label_char--;
					while(*current_label_char == '\n' 
					   || *current_label_char == ' ' 
					   || *current_label_char == '\t') {
						*(current_label_char--) = '\0';
					}
					set_label(&intermediate_tree, *current_stack_pointer, label_buffer);
					label_to_flush = 0;
				}
				current_label_char = label_buffer;

				//-- Adding inner node & push node onto stack
				child = add_child(&intermediate_tree, *current_stack_pointer);
				*(++current_stack_pointer) = child; 



				break;

			case '{':
				//-- Adding content of curly brace to label
				FAIL_ON(!parse_curly_brackets(&current_char, &current_label_char))
				// Parse curly braces does not advance "current_label_char"; manual advancing
				while(*(++current_label_char) != '\0') {}
				LOG("Current label: <<%s>>\n", label_buffer);
				label_to_flush = 1;

				break;

			case ']':
				LOG("tree_parse.c (l. 114) : End ] <<%s>>\n", current_char);

				//-- If a label (i.e. at a leaf node) is available add it to node
				if(label_to_flush) {
					PARSE_PUT_CHAR('\0');
					//--Removing trailing space
					current_label_char--;
					while(*current_label_char == '\n' 
					   || *current_label_char == ' ' 
					   || *current_label_char == '\t') {
						*(current_label_char--) = '\0';
					}
					set_label(&intermediate_tree, *current_stack_pointer, label_buffer);
					current_label_char = label_buffer;				
				}

				//-- If no node to pop, we've completed parsing
				if(current_stack_pointer == stack) {
					not_done = 0;
				}
				else {

					//-- Pop node from stack
					current_stack_pointer--;
					// PARSE_MOVE_ONE;


					label_to_flush = 0;
				}
				break;

			case '\\':
				PARSE_PUT_CHAR(*PARSE_MOVE_ONE);
				label_to_flush = 1;
				break;

			case ' ':
				// LOG("+"); 
				PARSE_PUT_CHAR(*current_char);
				break;

			case '\n':
				// LOG("-"); 
				PARSE_PUT_CHAR(*current_char);
				break;

			default:
				// LOG("%c", *current_char); 
				// LOG("tree_parse.c (l. 114) : FAIL on %c\n", *current_char);
				PARSE_PUT_CHAR(*current_char);
				label_to_flush = 1;

		}
		PARSE_MOVE_ONE;
	}

	move_tree(tree, &intermediate_tree);
	CLEAN_UP();
	return 1;

}
