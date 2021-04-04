#include <stdio.h>

#include "sprouts_and_deletes.h"
#include "tree.h"


void many_sprouts() {
	WITH_TREE(tree, {
		int i;
		for (i = 0; i < 1000; ++i)
		{
			sprout(&tree, 2 * i);
		}
		printf("%i\n", tree.n_nodes);
	})
}

void many_sprouts_and_a_delete() {
	WITH_TREE(tree, ({
		int i, j;
		int n_nodes;
		for (i = 0; i < 10; ++i)
		{
			n_nodes = tree.n_nodes;
			for(j = 0; j < n_nodes; j++) {
				if(is_leaf(tree, j)) {
					sprout(&tree, j);
				}
			}
		}
		FOR_EACH_NODE(i, tree) {
			set_label(&tree, i, "*");
		}
		printf("%i\n", tree.n_nodes);
		delete_node(&tree, 15);
		delete_node(&tree, 148);
		printf("%i\n", tree.n_nodes);
	}))
}

void simple_tree() {
	WITH_TREE(tree,{

		printf("Sprouting: %d\n", sprout(&tree, 0));
		printf("Sprouting: %d\n", sprout(&tree, 1));

		set_label(&tree, 0, "A");
		set_label(&tree, 1, "B");
		set_label(&tree, 2, "C");
		set_label(&tree, 3, "D");
		set_label(&tree, 4, "E");
		display(&tree);

		printf("Removing one node\n");
		tree.n_children[1] = 0;
		display(&tree);

		printf("NUM NODES: %d\n", tree.n_nodes);
		trim(&tree);
		display(&tree);

		printf("NUM NODES: %d\n", tree.n_nodes);
		trim(&tree);
		display(&tree);

		printf("Hello!\n");

	})
}

void delete_tree() {
	WITH_TREE(tree,({
	
		// tree.content_freer = &free;
	
		sprout(&tree, 0);
		sprout(&tree, 1);
		sprout(&tree, 2);
		sprout(&tree, 3);
	
		delete_node(&tree, 1);
		point_t root = {0,0};

		compute_positions(&tree, root);
	}))
}

void test_create_node_at() {
	WITH_TREE(tree,({
	
		// tree.content_freer = &free;
	
		sprout(&tree, 0);
		sprout(&tree, 1);
		sprout(&tree, 2);
		sprout(&tree, 3);

		int i;
		FOR_EACH_NODE(i, tree) {
			set_label(&tree, i, "*");
		}
	
		create_node_at(&tree, 2);

	}))
}

void test_inner_sprout() {
	WITH_TREE(tree,({
	
		// tree.content_freer = &free;
	
		sprout(&tree, 0);
		sprout(&tree, 1);
		sprout(&tree, 2);
		sprout(&tree, 3);

		int i;
		char buffer[3];
		FOR_EACH_NODE(i, tree) {
			sprintf(buffer, "%d", i);
			set_label(&tree, i, buffer);
		}
	

		display(&tree);
		sprout(&tree, 2);
		display(&tree);

	}))
}

void problematic_tree() {
	WITH_TREE(tree,({
	
		// tree.content_freer = &free;
		sprout(&tree, 0);
		sprout(&tree, 1);
		sprout(&tree, 2);
		sprout(&tree, 3);

		int i;
		char label_buffer[3];
		FOR_EACH_NODE(i, tree) {
			sprintf(label_buffer, "%d", i);
			set_label(&tree, i, label_buffer);
		}

		printf("############################\n");
		display(&tree);

		printf("############################\n");
		delete_node(&tree, 1);
		display(&tree);

		printf("############################\n");
		sprout(&tree, 1);
		display(&tree);

	}))
}
