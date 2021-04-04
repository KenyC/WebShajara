#include <stdio.h>

#include "draw.h"
#include "canvas.h"
#include "cairo_canvas.h"
#include "tree.h"

void draw_to_png() {
	WITH_TREE(tree,({

		printf("Sprouting: %d\n", sprout(&tree, 0));
		printf("Sprouting: %d\n", sprout(&tree, 1));
		printf("Sprouting: %d\n", sprout(&tree, 3));
		printf("Sprouting: %d\n", sprout(&tree, 6));
		printf("Sprouting: %d\n", sprout(&tree, 2));

		// sprout(&tree, 0);
		// sprout(&tree, 1);
		// sprout(&tree, 3);
		// sprout(&tree, 6);
		// sprout(&tree, 2);

		//display(&tree, &dummy_display);

		// printf("Drawing\n");
		init_context();
		translate(300, 0);
		point_t test = {0,0};
		compute_positions(&tree, test);
		draw_tree(&tree);
		render();
		destroy_context();
	}))

}

