#include <stdio.h>
#include <stdlib.h>

#include "tree.h"

#include "output.h"

void print_output() {
	WITH_TREE(tree,({

		sprout(&tree, 0);
		sprout(&tree, 1);
		sprout(&tree, 3);
		sprout(&tree, 6);
		sprout(&tree, 2);

		// sprout(&tree, 0);
		// sprout(&tree, 1);
		// sprout(&tree, 3);
		// sprout(&tree, 6);
		set_label(&tree, 0, "efzioj");
		set_label(&tree, 1, "zioj");
		set_label(&tree, 2, "zfefez");
		// sprout(&tree, 2);

		printf("Structure\n");
		display(&tree);

		printf("QTree output\n");
		char *string = output_qtree(&tree, NULL);
		printf("%s\n", string);

		free(string);

	}))

}