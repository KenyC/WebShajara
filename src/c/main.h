#ifndef MAIN_H
#define MAIN_H


#define SELECTED_NODE_RADIUS 4
#define PI 3.14159265358979
#define ROOT_X 250
#define ROOT_Y 20

void draw_axes();
void update_output();
void update_canvas();
#define UPDATE_ALL() \
	update_output(); \
	update_canvas();
void draw_selected_node();
void init_main_tree();


#endif