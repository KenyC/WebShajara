#ifndef CANVAS_H
#define CANVAS_H

#include "geometry.h"
#include "types.h"

#define FONT_SIZE 10

/**************************************
LOW LEVEL API
***************************************/

void init_context();
void destroy_context();
void clear();
void begin_path();

rect_t get_canvas_rect();

void translate(double x, double y);
void set_transform(const transform_t*);

void move_to(double x, double y);
void line_to(double x, double y);
void arc(double x, double y, double r, double start_angle, double end_angle);
void rectangle(double x, double y, double width, double height);
void fill_text(const char*, point_t);
void stroke();



void stroke_width(double width);
void stroke_color(Color);


/**************************************
HIGH LEVEL API
***************************************/

point_t get_extent(const char*);
void draw_line(double x1, double y1, double x2, double y2);

#endif