#include <emscripten.h>
#include <stdlib.h>

#include "canvas_mouse_event.h"
#include "main.h"
#include "canvas.h"
#include "log.h"
#include "utils.h"

/**************************************
GLOBAL VARIABLES
***************************************/

// Variables interfacing with UI
static transform_t* current_transform;
static Tree*        tree;
static int*         selected_node;
static point_t      tree_root;

// Pan constants
static int pan_view = 0;
static transform_t  last_invert_transform;
static point_t      last_screen_position;
static point_t      last_translate;

/**************************************
INITIALIZATION
***************************************/


void init_canvas_mouse_events(
	transform_t* current_transform_,
	Tree*        tree_,
	point_t      tree_root_,
	int*         selected_node_
) {
	current_transform = current_transform_;
	tree              = tree_;
	selected_node     = selected_node_;
	tree_root         = tree_root_;

	CATCH_CALLBACK(
		emscripten_set_mousedown_callback("#main_canvas", NULL, 0, &canvas_mousedown_callback),
		"Couldn't set canvas mouse down callback\n"
	) 
	CATCH_CALLBACK(
		emscripten_set_mouseup_callback("#main_canvas", NULL, 0, &canvas_mouseup_callback),
		"Couldn't set canvas mouse up callback\n"
	) 
	CATCH_CALLBACK(
		emscripten_set_mousemove_callback("#main_canvas", NULL, 0, &canvas_mousemove_callback),
		"Couldn't set canvas mouse move callback\n"
	) 
	CATCH_CALLBACK(
		emscripten_set_mouseleave_callback("#main_canvas", NULL, 0, &canvas_mouseleave_callback),
		"Couldn't set canvas mouse leave callback\n"
	) 


	// ZOOM BUTTONS
	CATCH_CALLBACK(
		emscripten_set_mousedown_callback("#zoom_in", NULL, 0, &zoom_in_callback),
		"Couldn't set zoom in mouse down callback\n"
	) 

	CATCH_CALLBACK(
		emscripten_set_mousedown_callback("#zoom_out", NULL, 0, &zoom_out_callback),
		"Couldn't set zoom in mouse down callback\n"
	) 

}

/**************************************
CANVAS CALLBACKS
***************************************/


EM_BOOL canvas_mousedown_callback(int event_type, const EmscriptenMouseEvent *mouse_event, void *user_data) {
	LOG("mouse down callback");
	last_invert_transform = invert(current_transform);
	point_t touch_position = {
		.x = mouse_event -> targetX,
		.y = mouse_event -> targetY,
	};
	touch_position = apply(&last_invert_transform, touch_position);
	int maybe_selected_node = find_nodes_near(tree, touch_position, RADIUS_SENSITIVITY); 
	if(mouse_event -> button == 0) {
		if(maybe_selected_node >= 0) {
			if(mouse_event -> ctrlKey) {
				LOG("select node");
				*selected_node = maybe_selected_node;
				EM_ASM({
					var label = document.getElementById("label");
					label.select();
					label.focus();
				});
				update_canvas();
			}
			else if(mouse_event -> altKey) {
				LOG("delete node");
				delete_node(tree, maybe_selected_node);
				if(*selected_node >= tree -> n_nodes) {
					*selected_node = 0;
				}
				compute_positions(tree, tree_root);
				UPDATE_ALL();
			}
			else {
				LOG("sprout");
				
				//-- Node indices will be changed, selected node must also be changed if it follow the insertion site
				if(!is_leaf(*tree, maybe_selected_node) && (*selected_node > maybe_selected_node)) {
					(*selected_node)++;
				}

				display(tree);
				sprout(tree, maybe_selected_node);
				display(tree);

				compute_positions(tree, tree_root);
				UPDATE_ALL();
			}
		}
		else {
			LOG("pan");
			pan_view = 1;

			last_screen_position.x = mouse_event->targetX;
			last_screen_position.y = mouse_event->targetY;

			last_translate.x = current_transform->translation_x;
			last_translate.y = current_transform->translation_y;
		}
	}

	return 1;
}



EM_BOOL canvas_mousemove_callback(int event_type, const EmscriptenMouseEvent *mouse_event, void *user_data) {
	// LOG("mouse move callback");
	if (pan_view)
	{
		point_t delta_mouse;
		delta_mouse.x = last_screen_position.x - mouse_event -> targetX;
		delta_mouse.y = -(last_screen_position.y - mouse_event -> targetY);

		// transform.translation_x += 10;
		current_transform -> translation_x = 
			last_translate.x -
			/* last_invert_transform.rotation_scale_11 * */ delta_mouse.x  /* +
			 last_invert_transform.rotation_scale_12 *  delta_mouse.y */
		;
		current_transform -> translation_y = 
			last_translate.y /* -
			 last_invert_transform.rotation_scale_21 *  delta_mouse.x */ +
			/* last_invert_transform.rotation_scale_22 * */ delta_mouse.y
		;

		update_canvas();

	}

	return 1;
}

EM_BOOL canvas_mouseleave_callback (int event_type, const EmscriptenMouseEvent *mouse_event, void *user_data) {
	pan_view = 0;
	return 1;
}

EM_BOOL canvas_mouseup_callback(int event_type, const EmscriptenMouseEvent *mouse_event, void *user_data) {
	pan_view = 0;
	return 1;
}


/**************************************
ZOOM CALLBACKS
***************************************/
#define ZOOM_FACTOR 1.2
EM_BOOL zoom_in_callback(int event_type, const EmscriptenMouseEvent *mouse_event, void *user_data) {
	current_transform -> rotation_scale_22 *= ZOOM_FACTOR;
	current_transform -> rotation_scale_12 *= ZOOM_FACTOR;
	current_transform -> rotation_scale_21 *= ZOOM_FACTOR;
	current_transform -> rotation_scale_11 *= ZOOM_FACTOR;

	update_canvas();
	return 1;

}

EM_BOOL zoom_out_callback(int event_type, const EmscriptenMouseEvent *mouse_event, void *user_data) {
	current_transform -> rotation_scale_22 /= ZOOM_FACTOR;
	current_transform -> rotation_scale_12 /= ZOOM_FACTOR;
	current_transform -> rotation_scale_21 /= ZOOM_FACTOR;
	current_transform -> rotation_scale_11 /= ZOOM_FACTOR;

	update_canvas();
	return 1;

}