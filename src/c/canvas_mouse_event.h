#ifndef CANVAS_MOUSE_EVENT_H
#define CANVAS_MOUSE_EVENT_H

#include <emscripten/html5.h>
#include "geometry.h"
#include "tree.h"

#define RADIUS_SENSITIVITY 20

void init_canvas_mouse_events(
	transform_t*,
	Tree*,
	point_t tree_root,
	int* selected_node_
);

EM_BOOL canvas_mousedown_callback(int event_type, const EmscriptenMouseEvent *mouse_event, void *user_data);
EM_BOOL canvas_mouseup_callback(int event_type, const EmscriptenMouseEvent *mouse_event, void *user_data);
EM_BOOL canvas_mousemove_callback(int event_type, const EmscriptenMouseEvent *mouse_event, void *user_data);
EM_BOOL canvas_mouseleave_callback (int event_type, const EmscriptenMouseEvent *mouse_event, void *user_data);


#endif
