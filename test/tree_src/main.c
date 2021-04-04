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

#define FUNCTION_TO_TEST problematic_tree


char buffer[256];
char* display_node_label(void* content) {
	strcpy(buffer, (char*) content);
	return buffer;
}

int main(int argc, char *argv[]) {
	// draw_to_png();
	// many_sprouts();
	// delete_tree();
	FUNCTION_TO_TEST();
	// simple_tree();
	return 0;
}


/**************************************
TEST FUNCTIONS
***************************************/

