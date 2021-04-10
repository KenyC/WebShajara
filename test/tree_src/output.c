#include <stdio.h>
#include <stdlib.h>

#include "tree.h"
#include "tree_output.h"
#include "tree_parse.h"

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
		char* string = output_qtree(&tree, NULL);
		printf("%s\n", string);

		printf("Forest output\n");
		string = output_forest(&tree, string);
		printf("%s\n", string);

		free(string);

	}))

}

char string_qtree[] = 
#include "data/qtree_tree.data.txt"
;
void parse_qtree_test() {
	WITH_TREE(tree,({

		int error = !parse_qtree(string_qtree, &tree); 
		if(error) {
			printf("Error\n");
		}
		printf("output.c (l. 55) : displaying tree\n");
		display(&tree);


	}))

}

char string_forest[] = 
#include "data/forest_tree.data.txt"
;
void parse_forest_test() {
	WITH_TREE(tree,({

		int error = !parse_forest(string_forest, &tree); 
		if(error) {
			printf("Error\n");
		}

		printf("output.c (l. 55) : displaying tree\n");
		display(&tree);

	}))

}