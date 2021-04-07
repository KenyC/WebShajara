#include <stdlib.h>
#include <emscripten.h>

#include "dropdown_event.h"
#include "main.h"
#include "log.h"

output_t* output_func = NULL;

void init_dropdown_events(output_t* output_func_) {
	output_func = output_func_;
}

void EMSCRIPTEN_KEEPALIVE dropdownlist_change_callback(int output_format_number) {
	switch(output_format_number) {
		case 1:
			*output_func = &output_qtree;
			break;
		case 2:
			*output_func = &output_forest;
			break;
		default:
			*output_func = &output_forest;
			break;
	}
	update_output();
}