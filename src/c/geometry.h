#ifndef GEOMETRY_H
#define GEOMETRY_H

typedef struct
{
	double rotation_scale_11, 
	       rotation_scale_12, 
	       rotation_scale_21, 
	       rotation_scale_22;
	       
	double translation_x, 
	       translation_y;
} transform_t;


transform_t invert(const transform_t*);

typedef struct
{
	double x, 
	       y;
} point_t;

point_t apply(const transform_t*, point_t);
double  quadrance(point_t pt1, point_t pt2);

typedef struct
{
	point_t position, 
	        size;
} rect_t;

#endif