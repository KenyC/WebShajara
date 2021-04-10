#ifndef LABEL_EVENT_H
#define LABEL_EVENT_H

#include <emscripten/html5.h>
#include "tree.h"
#include "geometry.h"


void init_label_events(Tree*, int* selected_node, point_t tree_root);
EM_BOOL label_mousedown_callback(int event_type, const EmscriptenMouseEvent *mouse_event, void *user_data);

void EMSCRIPTEN_KEEPALIVE set_label_callback();

#endif