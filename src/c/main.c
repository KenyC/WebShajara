#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <emscripten.h>

#include "main.h"
//
#include "canvas.h"
#include "text_output.h"
//
#include "tree.h"
//
#include "geometry.h"
#include "types.h"
#include "memory.h"
#include "log.h"
//
#include "canvas_mouse_event.h"
#include "label_event.h"

/**************************************
GLOBAL VARIABLES
***************************************/

static Color default_color = {0, 0, 0};

transform_t transform = {
	.rotation_scale_11   = 1,
	.rotation_scale_12   = 0,
	.rotation_scale_21   = 0,
	.rotation_scale_22   = 1,
	.translation_x       = 0,
	.translation_y       = 0,
};

Tree tree;
point_t tree_root = {ROOT_X, ROOT_Y};
int selected_node = 0;

#define TEXT_OUTPUT_BUFFER_INITIAL_SIZE 200
char *text_output_buffer; // Store the string form content of the tree

void dummy_log(char* string) {
	LOG("%s\n", string);
}


/**************************************
MAIN
***************************************/


int main() {

	LOG("Initialized\n");

	init_context();     // Init canvas
	init_text_output(); // Init the area where the string representation of the tree is displayed
	
	init_main_tree();

	// Not free'd at the end of progrem
	ALLOC(text_output_buffer, TEXT_OUTPUT_BUFFER_INITIAL_SIZE, char); 

	init_canvas_mouse_events(&transform, &tree, tree_root, &selected_node);
	init_label_events(&tree, &selected_node, tree_root);


	UPDATE_ALL();

}


/**************************************
DRAW
***************************************/
void update_output() {
	text_output_buffer = output_qtree(&tree, text_output_buffer);
	set_text_output_content(text_output_buffer);
}


void update_canvas() {

	// Drawing screen;
	clear();
	set_transform(&transform);

	// draw_axes();

	begin_path();
	draw_selected_node();

	begin_path();
	draw_tree(&tree);

}

#define SCALE    30
void draw_axes() {
	// LOG("Printing axesmak");
	// translate(CENTER_X, CENTER_Y);
	begin_path();
	move_to(SCALE, 0);
	line_to(0, 0);
	line_to(0, SCALE);
	stroke();
	// translate(-CENTER_X, -CENTER_Y);
}

#define PADDING 3
void draw_selected_node() {
	// LOG("Printing axesmak");
	begin_path();
	stroke_color(default_color);
	if((tree.node_bounding_boxes[selected_node].x + tree.node_bounding_boxes[selected_node].y) < 1e-5) {
		arc(
			tree.positions[selected_node].x, 
			tree.positions[selected_node].y,
			SELECTED_NODE_RADIUS,
			0,
			2 * PI
		);
	}
	else {
		rectangle(
			tree.positions[selected_node].x - tree.node_bounding_boxes[selected_node].x / 2 - PADDING, 
			tree.positions[selected_node].y - PADDING,
			tree.node_bounding_boxes[selected_node].x + 2 * PADDING,
			tree.node_bounding_boxes[selected_node].y + 2 * PADDING
		);
	}
	stroke();
}

transform_t* get_transform() {
	return &transform;
}


/**************************************
TREE
***************************************/
char label_buffer[4];
void init_main_tree() {
	init_tree(&tree); // Never freed ; is this a problem? I don't think so
	// tree.content_freer = &free;

	sprout(&tree, 0);
	sprout(&tree, 1);
	sprout(&tree, 2);
	sprout(&tree, 3);

	int i;
	FOR_EACH_NODE(i, tree) {
		sprintf(label_buffer, "%d", i);
		set_label(&tree, i, label_buffer);
	}

	compute_bounding_boxes_from_label_extents(&tree);
	compute_positions(&tree, tree_root);
}
