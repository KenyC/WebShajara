#include <stdio.h>

#include "geometry.h"
#include "main.h"

#define FUNCTION_TO_TEST test_inverse

int main(int argc, char const *argv[])
{
	FUNCTION_TO_TEST();
	return 0;
}


void test_inverse() {
	transform_t transform = {
		.rotation_scale_11   = 4,
		.rotation_scale_12   = 1,
		.rotation_scale_21   = 2,
		.rotation_scale_22   = -3,
		.translation_x       = 4,
		.translation_y       = 5,
	};

	transform_t inverse = invert(&transform);

	printf(
    "|%f, %f, %f|\n"
    "|%f, %f, %f|\n"
    "|0,  0 , 1 |\n"
	,
	inverse.rotation_scale_11, inverse.rotation_scale_12, inverse.translation_x,
	inverse.rotation_scale_21, inverse.rotation_scale_22, inverse.translation_y
	);

	int i, j;
	point_t test1;
	point_t test2;

	for (int i = 0; i < 4; ++i)
	 {
	 	for (int j = 0; j < 4; ++j)
	 	{
	 		test1.x = i;
	 		test1.y = j;

	 		test2 = apply(&transform, test1);
	 		test2 = apply(&inverse,   test2);

	 		printf(
	 			"(%f, %f) == (%f, %f)\n", 
	 			test1.x, test1.y,
	 			test2.x, test2.y
	 		);
	 	}
	 } 

}