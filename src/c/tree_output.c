#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "tree_output.h"

/**************************************
CONSTANTS
***************************************/


/**************************************
OUTPUT METHODS
***************************************/
#define APPEND(STRING, CHARACTER) *(STRING++) = CHARACTER
#define APPEND_STRING(STRING1, STRING2) {\
	char *buffer_;                      \
 	for(                                \
 		buffer_ = STRING2;              \
 		*buffer_ != '\0';               \
 		APPEND(STRING1, *(buffer_++))    \
 	);}
#define PRINT_LABEL(TREE, NODE, CHARACTER) 		{ \
	char *label = tree -> labels[current_node];   \
	if(label != NULL) {                           \
		while(*label != '\0') {                   \
			APPEND(current_char, *(label++));     \
		}                                         \
	}                                             \
}

/**************************************
QTREE OUTPUT
***************************************/


// Constants
#define QTREE_HEADER_SIZE           6
#define QTREE_INNER_NODE_NUM_CHARS  7
#define QTREE_LEAF_NODE_NUM_CHARS   3
#define QTREE_TABULATION_SIZE       3
char qtree_tabulation_string[QTREE_TABULATION_SIZE+1] = "   ";
char qtree_header[QTREE_HEADER_SIZE+1] = "\\Tree\n";


char* output_qtree_aux(
	const Tree* tree, 
	int tabulation, 
	int current_node, 
	char* current_char
) {


	{ // Creating a scope so that all intermediate vars are unstacked before recursive call
		int i_tabulations = tabulation;
		while (i_tabulations --> 0) {
			APPEND_STRING(current_char, qtree_tabulation_string);
		}
	}
	if(is_leaf(*tree, current_node)) {
		APPEND(current_char, '{');
		PRINT_LABEL(tree, current_node, current_char);
		APPEND_STRING(current_char, "}\n");
	}
	else {
		APPEND_STRING(current_char, "[.{");
		PRINT_LABEL(tree, current_node, current_char);
		APPEND_STRING(current_char, "}\n");

		int i;
		FOR_EACH_CHILD(i, *tree, current_node) {
			current_char = output_qtree_aux(
				tree,
				tabulation + 1,
				tree -> children[current_node][i],
				current_char
			);
		}

		{ // Creating a scope so that all intermediate vars are unstacked before recursive call
			int i_tabulations = tabulation;
			while (i_tabulations --> 0) {
				APPEND_STRING(current_char, qtree_tabulation_string);
			}
		}
		APPEND_STRING(current_char, "]\n");
	}
	return current_char;
}




char* output_qtree(const Tree* tree, char *string_buffer) {

	// COMPUTING STRING LENGTH
	int size_string_to_return = QTREE_HEADER_SIZE;
	int i, j, depth;

	WITH_ARRAY(int, depths, tree -> n_nodes, ({
		depths[0] = 0;
		FOR_EACH_NODE(i, *tree) {
			depth = depths[i];
			if(is_leaf(*tree, i)) {
				// Leaf nodes contribute 1 string of indentation + character "{}\n"
				size_string_to_return += QTREE_TABULATION_SIZE * depth + QTREE_LEAF_NODE_NUM_CHARS;
			}
			else {
				// Inner nodes contribute 1 string of indentation + character "[.{}\n"
				size_string_to_return += 2 * QTREE_TABULATION_SIZE * depth + QTREE_INNER_NODE_NUM_CHARS;
			}

			// Adding label length
			if(tree -> labels[i] != NULL) {
				size_string_to_return += strlen(tree -> labels[i]);
			}

			FOR_EACH_CHILD(j, *tree, i) {
				depths[tree -> children[i][j]] = depth + 1;
			}
		}
	}))


	REALLOC(string_buffer, size_string_to_return + 1, char);
	char *current   = string_buffer;
	APPEND_STRING(current, qtree_header);

	current = output_qtree_aux(
		tree,
		0,
		0,
		current
	);
	APPEND(current, '\0');


	return string_buffer;
}

/**************************************
FOREST OUTPUT
***************************************/

// Constants
#define FOREST_HEADER_SIZE                 15
#define FOREST_FOOTER_SIZE                 13
#define FOREST_INNER_NODE_NUM_CHARS        6
#define FOREST_LEAF_NODE_NUM_CHARS         5
#define FOREST_EMPTY_INNER_NODE_NUM_CHARS  4
#define FOREST_EMPTY_LEAF_NODE_NUM_CHARS   3
#define FOREST_TABULATION_SIZE             3
char forest_tabulation_string[FOREST_TABULATION_SIZE+1] = "   ";
char forest_header[FOREST_HEADER_SIZE+1] = "\\begin{forest}\n";
char forest_footer[FOREST_FOOTER_SIZE+1] = "\\end{forest}\n";


char* output_forest_aux(
	const Tree* tree, 
	int tabulation, 
	int current_node, 
	char* current_char
) {


	{ // Creating a scope so that all intermediate vars are unstacked before recursive call
		int i_tabulations = tabulation;
		while (i_tabulations --> 0) {
			APPEND_STRING(current_char, forest_tabulation_string);
		}
	}
	if(is_leaf(*tree, current_node)) {
		APPEND(current_char, '[');
		if(tree -> labels[current_node] != NULL) {
			APPEND(current_char, '{');
			PRINT_LABEL(tree, current_node, current_char);
			APPEND(current_char, '}');
		} 
		APPEND_STRING(current_char, "]\n");
	}
	else {
		APPEND(current_char, '[');
		if(tree -> labels[current_node] != NULL) {
			APPEND(current_char, '{');
			PRINT_LABEL(tree, current_node, current_char);
			APPEND(current_char, '}');
		}
		APPEND(current_char, '\n');

		int i;
		FOR_EACH_CHILD(i, *tree, current_node) {
			current_char = output_forest_aux(
				tree,
				tabulation + 1,
				tree -> children[current_node][i],
				current_char
			);
		}

		{ // Creating a scope so that all intermediate vars are unstacked before recursive call
			int i_tabulations = tabulation;
			while (i_tabulations --> 0) {
				APPEND_STRING(current_char, forest_tabulation_string);
			}
		}
		APPEND_STRING(current_char, "]\n");
	}
	return current_char;
}




char* output_forest(const Tree* tree, char *string_buffer) {

	// COMPUTING STRING LENGTH
	int size_string_to_return = FOREST_HEADER_SIZE + FOREST_FOOTER_SIZE;
	int i, j, depth;

	WITH_ARRAY(int, depths, tree -> n_nodes, ({
		depths[0] = 0;
		FOR_EACH_NODE(i, *tree) {
			depth = depths[i];
			if(is_leaf(*tree, i)) {
				// Leaf nodes contribute 1 string of indentation + character "{}\n"
				size_string_to_return += FOREST_TABULATION_SIZE * depth + FOREST_LEAF_NODE_NUM_CHARS;
			}
			else {
				// Inner nodes contribute 1 string of indentation + character "[.{}\n"
				size_string_to_return += 2 * FOREST_TABULATION_SIZE * depth + FOREST_INNER_NODE_NUM_CHARS;
			}

			// Adding label length
			if(tree -> labels[i] != NULL) {
				size_string_to_return += strlen(tree -> labels[i]);
			}

			FOR_EACH_CHILD(j, *tree, i) {
				depths[tree -> children[i][j]] = depth + 1;
			}
		}
	}))


	REALLOC(string_buffer, size_string_to_return + 1, char);
	char *current   = string_buffer;
	APPEND_STRING(current, forest_header);

	current = output_forest_aux(
		tree,
		0,
		0,
		current
	);

	APPEND_STRING(current, forest_footer);
	APPEND(current, '\0');

	// printf("Real %d vs expected %d\n", strlen(string_buffer), size_string_to_return);

	return string_buffer;
}
