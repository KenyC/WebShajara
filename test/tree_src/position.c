#include <stdio.h>

#include "position.h"
#include "tree.h"

void simple_position() {
	WITH_TREE(tree,({

		printf("Sprouting: %d\n", sprout(&tree, 0));
		printf("Sprouting: %d\n", sprout(&tree, 1));

		set_label(&tree, 0, "A");
		set_label(&tree, 1, "B");
		set_label(&tree, 2, "C");
		set_label(&tree, 3, "D");
		set_label(&tree, 4, "E");
		display(&tree);

		printf("Computing positions\n");
		point_t test = {0,0};
		compute_positions(&tree, test);
		int i;
		for (i = 0; i < tree.n_nodes; ++i)
		{
			printf(
				"%i %s (%f,%f)\n", 
				i, 
				tree.labels[i], 
				tree.positions[i].x, 
				tree.positions[i].y
			);
		}

	}))
}
