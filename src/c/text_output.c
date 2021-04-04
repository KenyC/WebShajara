#include <emscripten.h>
#include <string.h>

#include "text_output.h"

void init_text_output() {
	EM_ASM({
		text_output = document.getElementById("text_output");
	});	
}

void set_text_output_content(const char* string) {
	EM_ASM({
		var view =  Module.HEAPU8.subarray($0, $0 + $1);
		var string = '';
		for (let i = 0; i < $1; i++) {
		  string += String.fromCharCode(view[i]);
		}
		text_output.value = string;
	}, 
		string, 
		strlen(string)
	);	
}

void text_output_select_all() {
	EM_ASM({
		text_output.select();
	});
}