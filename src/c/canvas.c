#include <stdio.h>
#include <string.h>
#include <emscripten.h>
#define LOG(...) emscripten_log(EM_LOG_CONSOLE, __VA_ARGS__) 

#include "canvas.h"

/**************************************
LOW LEVEL API
***************************************/


void init_context() {
	EM_ASM({
		canvas   = document.getElementById("main_canvas");
		context  = canvas.getContext("2d");
		context.lineJoin = "round"; 
		context.font = "#FONT_SIZEpx Arial";
	});
}

void destroy_context() {}

void begin_path() {
	EM_ASM({
		context.beginPath();
	});
}

void move_to(double x, double y){
	EM_ASM({context.moveTo($0, $1);},
		x, y
	);
}

void line_to(double x, double y){
	EM_ASM({context.lineTo($0, $1);},
		x, y
	);
}

void arc(
	double x, 
	double y, 
	double r, 
	double start_angle, 
	double end_angle
) {
	EM_ASM(
		{context.arc($0, $1, $2, $3, $4)},
		x,
		y,
		r,
		start_angle,
		end_angle
	);
}

void rectangle(
	double x, 
	double y, 
	double width, 
	double height
) {
	EM_ASM(
		{context.rect($0, $1, $2, $3)},
		x,
		y,
		width,
		height
	);
}


void fill_text(const char* string, point_t position) {
	EM_ASM({
			var string = UTF8ToString($0);
			label_extent = context.measureText(string);

			context.fillText(string, $1, $2)
		}, 
		string, 
		// strlen(string),
		position.x,
		position.y
	);

}


void stroke(){
	EM_ASM({context.stroke();});
}


void stroke_width(double width) {
	EM_ASM(
		{context.lineWidth = $0;},
		width
	);
}


void stroke_color(Color color) {
	
	EM_ASM(
		{context.strokeStyle = "rgb(" + String($0) +", " + String($1) + ", " + String($2) + ")";},
		color.red, color.green, color.blue
	);

}











void clear(){
	EM_ASM({
		// Store the current transformation matrix
		context.save();
		context.setTransform(1, 0, 0, 1, 0, 0);
		context.clearRect(0, 0, canvas.width, canvas.height);
		// Restore the transform
		context.restore();
	});
}

void translate(double x, double y){
	EM_ASM({context.translate($0, $1);},
		x, y
	);
}



void set_transform(const transform_t* transform) {
	// LOG("Set Transform!");
	// LOG("%f %f",
	// 	transform->translation_x, 
	// 	100.+transform->translation_y
	// );

	EM_ASM({
		context.setTransform(
			$0, $1, 
			$2, $3, 
			/*****/
			$4, $5
		);
	},
		transform->rotation_scale_11, transform->rotation_scale_12,
		transform->rotation_scale_21, transform->rotation_scale_22,
		/*****/
		transform->translation_x, transform->translation_y
	);

	// EM_ASM({
	// 	context.setTransform(
	// 		1, 1, 
	// 		0, 1, 
	// 		/*****/
	// 		0, -100
	// 	);
	// });

}

rect_t get_canvas_rect() {
	rect_t to_return;
	EM_ASM({
		bounding_rect = canvas.getBoundingClientRect();
	});
	to_return.position.x = EM_ASM_INT({return bounding_rect.x});
	to_return.position.y = EM_ASM_INT({return bounding_rect.y});
	to_return.size.x     = EM_ASM_INT({return bounding_rect.width});
	to_return.size.y     = EM_ASM_INT({return bounding_rect.height});

	return to_return;
}


/**************************************
HIGH LEVEL API
***************************************/


void draw_line(double x1, double y1, double x2, double y2) {
	EM_ASM({
		context.moveTo($0, $1);
		context.lineTo($2, $3);
		context.stroke();
	},
		x1, y1,
		x2, y2
	);
}

point_t get_extent(const char* string) {
	point_t to_return;
	EM_ASM({
			label_extent = context.measureText(UTF8ToString($0));
		}, 
		string
	);

	to_return.x = EM_ASM_DOUBLE({return label_extent.width; });
	to_return.y = FONT_SIZE;

	return to_return;
}