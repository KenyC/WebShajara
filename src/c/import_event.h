#ifndef IMPORT_EVENT_H
#define IMPORT_EVENT_H

#include <emscripten/html5.h>

#include "tree.h"
#include "tree_parse.h"


void init_import_events(parse_t* parse_func_, Tree*, point_t* tree_root_, int* selected_node_);

EM_BOOL import_click_callback(int event_type, const EmscriptenMouseEvent *mouse_event, void *user_data);

#endif