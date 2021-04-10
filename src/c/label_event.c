#include <emscripten.h>
#include <stdlib.h>

#include "label_event.h"
#include "utils.h"
#include "log.h"
#include "main.h"

/**************************************
GLOBALS
***************************************/

static Tree*    tree;
static int*     selected_node;
static point_t  tree_root;

/**************************************
INITIALIZATION
***************************************/


void init_label_events(Tree* tree_, int* selected_node_, point_t tree_root_) {
	tree          = tree_;
	selected_node = selected_node_;
	tree_root     = tree_root_;

	CATCH_CALLBACK(
		emscripten_set_mousedown_callback("#set_label", NULL, 0, &label_mousedown_callback),
		"Couldn't set label mouse down callback\n"
	) 
}

/**************************************
CALLBACKS
***************************************/


EM_BOOL label_mousedown_callback(
	int event_type, 
	const EmscriptenMouseEvent *mouse_event, 
	void *user_data
) {
	set_label_callback();
	return 1;
}

void EMSCRIPTEN_KEEPALIVE set_label_callback() {
	char* label = (char*) EM_ASM_INT({
		var ptr    = allocate(
			intArrayFromString(document.getElementById("label").value), 
			ALLOC_NORMAL
		);
		return ptr;
	}); 	

	set_label(tree, *selected_node, label);

	compute_bounding_boxes_from_label_extents(tree);
	compute_positions(tree, tree_root);
	UPDATE_ALL();

	free(label);
}