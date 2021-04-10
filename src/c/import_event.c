#include <stdlib.h>
#include <emscripten.h>

#include "import_event.h"
#include "main.h"
#include "geometry.h"
#include "utils.h"
#include "log.h"


static parse_t* parse_func    = NULL;
static Tree*    tree          = NULL;
static point_t* tree_root     = NULL;
static int*     selected_node = NULL;

void init_import_events(
	parse_t*  parse_func_, 
	Tree*     tree_, 
	point_t*  tree_root_,
	int*      selected_node_
) {
	parse_func    = parse_func_;
	tree          = tree_;
	tree_root     = tree_root_;
	selected_node = selected_node_;

	CATCH_CALLBACK(
		emscripten_set_mousedown_callback("#import", NULL, 0, &import_click_callback),
		"Couldn't set import mouse down callback\n"
	);
}

EM_BOOL import_click_callback(int event_type, const EmscriptenMouseEvent *mouse_event, void *user_data) {
	char* tree_string = (char*) EM_ASM_INT({
		var ptr    = allocate(
			intArrayFromString(document.getElementById("text_output").value), 
			ALLOC_NORMAL
		);
		return ptr;
	}); 
	// LOG("%s", tree_string);
	if(!(*parse_func)(tree_string, tree)) {
		EM_ASM({alert("Can't parse provided string.");});
	}
	else {
		compute_bounding_boxes_from_label_extents(tree);
		compute_positions(tree, *tree_root);
		if(*selected_node >= tree -> n_nodes) {
			*selected_node = 0;
		}
		UPDATE_ALL();
	}
	free(tree_string);
	return 1;
}