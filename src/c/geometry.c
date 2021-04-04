#include "geometry.h"

transform_t invert(const transform_t* original) {
	transform_t to_return;

	to_return.translation_x = - original->translation_x;
	to_return.translation_y = - original->translation_y;

	double determiner;
	determiner = original->rotation_scale_11 * original->rotation_scale_22
	           - original->rotation_scale_21 * original->rotation_scale_12;

	to_return.rotation_scale_11 =   original->rotation_scale_22 / determiner; 
	to_return.rotation_scale_12 = - original->rotation_scale_12 / determiner; 
	to_return.rotation_scale_21 = - original->rotation_scale_21 / determiner; 
	to_return.rotation_scale_22 =   original->rotation_scale_11 / determiner;

	return to_return;
}


point_t apply(const transform_t* transform, point_t point) {
	point_t to_return;

	to_return.x = 
		transform -> rotation_scale_11 * point.x +
		transform -> rotation_scale_12 * point.y +
		transform -> translation_x
	;

	to_return.y = 
		transform -> rotation_scale_21 * point.x +
		transform -> rotation_scale_22 * point.y +
		transform -> translation_y
	;

	return to_return;
}

double quadrance(point_t pt1, point_t pt2) {
	point_t difference;
	difference.x = pt1.x - pt2.x;
	difference.y = pt1.y - pt2.y;
	return difference.x * difference.x + difference.y * difference.y;
}
