#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tree.h"
#include "canvas.h"

#include "main.h"
#include "cairo_canvas.h"
#include "position.h"
#include "output.h"
#include "sprouts_and_deletes.h"

// #define FUNCTION_TO_TEST add_child_test
// #define FUNCTION_TO_TEST problematic_tree
#define FUNCTION_TO_TEST parse_forest_test


char buffer[256];
char* display_node_label(void* content) {
	strcpy(buffer, (char*) content);
	return buffer;
}

int main(int argc, char *argv[]) {
	// draw_to_png();
	// delete_tree();
#ifdef FUNCTION_TO_TEST
	FUNCTION_TO_TEST();
#else
	// many_sprouts();
	many_sprouts();
	many_sprouts_and_a_delete();
	simple_tree();
	delete_tree();
	test_create_node_at();
	test_inner_sprout();
	problematic_tree();
	add_child_test();
	simple_position();
	print_output();
#endif
	// simple_tree();
	return 0;
}


/**************************************
TEST FUNCTIONS
***************************************/

