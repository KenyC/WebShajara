#include <cairo.h>
#include "geometry.h"
#include "types.h"


/**************************************
LOW LEVEL API
***************************************/


static cairo_surface_t *surface;
static cairo_t         *context;

#define WIDTH   500
#define HEIGHT  500

void init_context() {
	surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, WIDTH, HEIGHT);
	context = cairo_create (surface);
}
void destroy_context() {
	cairo_destroy(context);
	cairo_surface_destroy(surface);
}
void clear() {}
void begin_path() {}


static rect_t canvas_rect = {
	.position = {.x = 0,     .y = 0},
	.size     = {.x = WIDTH, .y = HEIGHT},
};
rect_t get_canvas_rect() {
	return canvas_rect;
}

void translate(double x, double y) {
	cairo_translate(context, x, y);
}
void set_transform(const transform_t* transform) {}

void move_to(double x, double y) {
	cairo_move_to(context, x, y);
}
void line_to(double x, double y) {
	cairo_line_to(context, x, y);
}
void stroke() {
	cairo_stroke(context);
}


void stroke_width(double width) {
	cairo_set_line_width (context, width);

}
void stroke_color(Color color) {
	cairo_set_source_rgb(
		context, 
		color.red, 
		color.green, 
		color.blue
	);
}

void fill_text(const char* string, point_t origin) {
	cairo_move_to(context, origin.x, origin.y);
	cairo_show_text(context, string);
}


point_t get_extent(const char* string) {
	point_t to_return;
	cairo_text_extents_t extents;

	cairo_text_extents(
		context,
		string,
		&extents
	);
	to_return.x = extents.width;
	to_return.x = extents.height;
	return to_return;
}


void render() {
	cairo_surface_write_to_png(surface, "output/tree.png");

}

/**************************************
HIGH LEVEL API
***************************************/

void draw_line(double x1, double y1, double x2, double y2) {
	move_to(x1, y1);
	line_to(x2, y2);
	stroke();
}


