#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "memory.h"
#include "canvas.h"
#include "tree.h"
#include "utils.h"
#include "log.h"

/**************************************
OPTIONS
***************************************/

Options defaults = {
	.space_between_sisters     =  20,
	.height_node_to_daughter   =  50,

	.padding_labels            =  {8, 8},
	.padding_inner_node_labels =  5,
	.min_width_node            =  50,


	.width_edge                =  2,

	.color_edge                =  {0, 0, 0},
	.color_label               =  {255, 0, 0},
};
	


/**************************************
MEMORY METHODS
***************************************/
/*
Memory policy: 

   1. labels' memory is entirely managed by this module
   2. all labels pointer i < n_nodes are pointers different from each other or NULL (they can be freed independently from each other)
   3. all labels pointer i >= n_nodes are either NULL or equal one of the pointers for i < n_nodes (they do not have to be freed if the first ones are freed)

*/

void init_tree(Tree* tree) {
	tree -> size_memory_nodes    = INITIAL_NUMBER_NODES;

	tree -> n_children           = malloc(INITIAL_NUMBER_NODES * sizeof(int)  );
	tree -> children             = malloc(INITIAL_NUMBER_NODES * sizeof(int*) );
	tree -> labels               = malloc(INITIAL_NUMBER_NODES * sizeof(char*));
	tree -> size_memory_children = malloc(INITIAL_NUMBER_NODES * sizeof(void*));

	tree -> node_bounding_boxes  = malloc(INITIAL_NUMBER_NODES * sizeof(point_t));
	tree -> positions            = malloc(INITIAL_NUMBER_NODES * sizeof(point_t));


	int i;
	for (i = 0; i < INITIAL_NUMBER_NODES; ++i)
	{
		tree -> size_memory_children[i] = INITIAL_NUMBER_CHILDREN;
		tree -> children[i]             = malloc(INITIAL_NUMBER_CHILDREN * sizeof(int));
		tree -> labels[i]               = NULL;

		INIT_POINT_ZERO(tree -> node_bounding_boxes[i])
		INIT_POINT_ZERO(tree -> positions[i])
	}

	tree -> n_nodes       = 1;
	tree -> n_children[0] = 0;

}


void destroy_tree(Tree* tree) {

	int i;
	for (i = 0; i < tree -> size_memory_nodes; ++i)
	{
		free(tree -> children[i]);
	}
	FOR_EACH_NODE(i, *tree) {
		free_content(*tree, i);
	}
	free(tree -> children);

	free(tree -> n_children);
	free(tree -> labels);
	free(tree -> size_memory_children);
	free(tree -> node_bounding_boxes);
	free(tree -> positions);

}

void make_more_space_nodes(Tree* tree) {
	printf("tree.c (l. 72) : Making more spaces\n");
	int new_size_memory_nodes = tree -> size_memory_nodes + SIZE_NODES_INCREMENT;

	REALLOC(tree -> size_memory_children, new_size_memory_nodes, int* );
	REALLOC(tree -> children,             new_size_memory_nodes, int* );
	REALLOC(tree -> n_children,           new_size_memory_nodes, int  );
	REALLOC(tree -> labels,               new_size_memory_nodes, char*);
	REALLOC(tree -> node_bounding_boxes,  new_size_memory_nodes, point_t);
	REALLOC(tree -> positions,            new_size_memory_nodes, point_t);



	for(; tree -> size_memory_nodes != new_size_memory_nodes ; tree -> size_memory_nodes++) {
		tree -> size_memory_children [tree -> size_memory_nodes] = INITIAL_NUMBER_CHILDREN;
		tree -> children             [tree -> size_memory_nodes] = malloc(INITIAL_NUMBER_CHILDREN * sizeof(int));
		tree -> labels               [tree -> size_memory_nodes] = NULL;

		INIT_POINT_ZERO(tree -> node_bounding_boxes [tree -> size_memory_nodes]);
		INIT_POINT_ZERO(tree -> positions           [tree -> size_memory_nodes]);

	} 

}


void make_more_space_children(Tree* tree, int node) {
	printf("tree.c (l. 72) : Making more spaces\n");
	REALLOC(
		tree -> children[node], 
		tree -> size_memory_children[node] += SIZE_CHILDREN_INCREMENT,
		int
	);
}







/**************************************
TREE MANIPULATION METHODS
***************************************/

int create_node(Tree* tree) {
	int new_node_index = tree -> n_nodes;
	if(new_node_index >= tree -> size_memory_nodes) {
		make_more_space_nodes(tree);
	}
	tree -> n_children[new_node_index] = 0;
	tree -> n_nodes++;
	return new_node_index;
}

#define COPY_ATTRIBUTE(TREE, ATTRIBUTE, DESTINATION, SOURCE) \
	(TREE).ATTRIBUTE[DESTINATION] = (TREE).ATTRIBUTE[SOURCE];
int create_node_at(Tree* tree, int index) {
	// All nodes after "index" will have their index incremented ; we need to change references
	int i, j;
	FOR_EACH_NODE(i, *tree) {
		FOR_EACH_CHILD(j, *tree, i) {
			if(tree -> children[i][j] >= index) {
				tree -> children[i][j]++;
			}
		}
	}

	tree -> n_nodes++;
	if(tree -> n_nodes >= tree -> size_memory_nodes) {
		make_more_space_nodes(tree);
	}

	// We now move all nodes one index down
	// Before that, save the allocated space for the last node ; otherwise, it will be lost in copying
	int *children_memory_space = tree -> children[tree -> n_nodes - 1];
	for(i = tree -> n_nodes - 1; i != index; i--) {
		COPY_ATTRIBUTE(*tree, size_memory_children, i, i - 1);
		COPY_ATTRIBUTE(*tree, node_bounding_boxes,  i, i - 1);
		COPY_ATTRIBUTE(*tree, positions,            i, i - 1);
		COPY_ATTRIBUTE(*tree, n_children,           i, i - 1);

		// Some care need to be taken when copying dynamically allocated data (we don't want two pointers to the same object, b/c we may create double free'ing)
		// We're shifting everything so double pointers is not an issue
		// Memory loss may be an issue so we must make sure that the new node carries the reference to memory
		COPY_ATTRIBUTE(*tree, labels,   i, i - 1);
		COPY_ATTRIBUTE(*tree, children, i, i - 1);

	}

	tree -> n_children[index] = 0;
	tree -> children  [index] = children_memory_space;
	tree -> labels    [index] = NULL;
	/*
	*/

	return index;
}


int delete_node(Tree* tree, int node) {
	tree -> n_children[node] = 0;
	trim(tree);
	return 0;
}






void trim(Tree* tree) {
	int i;
	int n_reachable_nodes;

	WITH_ARRAY(int, map_old_positions_to_new, tree->n_nodes, {

		// FIND REACHABLE NODES
		/*
		map_old_positions_to_new[i] = -1 if node is not reachable
		map_old_positions_to_new[i] = j  if j is the position that i should appear in
		*/
		FOR_EACH_NODE(i, *tree)
		{
			map_old_positions_to_new[i] = -1;
		}
		map_old_positions_to_new[0] = 0;

		/*
		For each node, if it is reachable, 
			- mark its children as reachable.
			- assign its position to the currently available position
		If not reachable, free its content and children
		*/
		int child_index;
		n_reachable_nodes = 0;
		FOR_EACH_NODE(i, *tree) {
			if(map_old_positions_to_new[i] >= 0) {
				for(
					child_index = 0; 
					child_index < tree -> n_children[i]; 
					child_index++
				) {
					map_old_positions_to_new[tree->children[i][child_index]] = 0;
				}
				map_old_positions_to_new[i] = n_reachable_nodes++;
			}
			else {
				// free(tree -> children[i]);
				free_content(*tree, i);
				tree -> labels[i] = NULL;
			}
		}

		int *swap_children_buffer;
		int new_position;
		int j;
		FOR_EACH_NODE(i, *tree) {
			new_position = map_old_positions_to_new[i]; 

			if(new_position >= 0) {
				if(new_position != i) {
					tree -> labels [new_position] = tree -> labels[i];
					tree -> labels [i]            = NULL;

					swap_children_buffer = tree -> children[new_position]; 
					tree -> children[new_position] = tree -> children[i];
					tree -> children[i]            = swap_children_buffer;

					tree -> n_children          [new_position] = tree -> n_children[i];
					tree -> node_bounding_boxes [new_position] = tree -> node_bounding_boxes[i];
					tree -> positions           [new_position] = tree -> positions[i];

				}

				// Indices are changing, rename children accordingly
				FOR_EACH_CHILD(j, *tree, new_position) {
					tree -> children[new_position][j] = map_old_positions_to_new[tree -> children[new_position][j]];
				}
			}
		}
		tree -> n_nodes = n_reachable_nodes;
		/*
	*/

	}) // end alloc "map_old_positions_to_new"
}

int sprout(Tree *tree, int node) {
	if(!in_tree(*tree, node)) {
		return -1;
	}

	if(is_leaf(*tree, node)) {
		int node1 = create_node(tree);
		int node2 = create_node(tree);
		tree -> children[node][0] = node1;  
		tree -> children[node][1] = node2;  
		tree -> n_children[node]  = 2;  
		return 0;
	}
	else {
		create_node_at(tree, node);


		// Find parent of "node" and rewire it to new_node
		int i, j;
		FOR_EACH_NODE(i, *tree) {
			FOR_EACH_CHILD(j, *tree, i) {
				if(tree -> children[i][j] == node + 1) {
					tree -> children[i][j]--;
					goto end_for;
				}
			}
		}
		end_for:

		tree -> children[node][0] = create_node(tree);
		tree -> children[node][1] = node + 1;
		tree -> n_children[node]  = 2;
		tree -> node_bounding_boxes[node].x = 0;
		tree -> node_bounding_boxes[node].y = 0;

		return 0;
	}
}

int add_child(Tree *tree, int node) {
	if(!in_tree(*tree, node)) {
		return -1;
	}
	int node_child = create_node(tree);
	// increment number of children, if more space needed, make more space
	if((++(tree -> n_children[node])) > tree -> size_memory_children[node]) {
		make_more_space_children(tree, node);
	}

	tree -> children[node][tree -> n_children[node] - 1] = node_child;
	return node_child;
}


void set_label(Tree* tree, int node_index, char* label) {
	if(label[0] != '\0') {
		REALLOC(
			tree -> labels[node_index], 
			MAX(strlen(label) + 1, MIN_LABEL_LENGTH), 
			char
		);
		strcpy(tree -> labels[node_index], label);
	}
	else if(tree -> labels[node_index] != NULL) {
		free(tree -> labels[node_index]);
		tree -> labels[node_index] = NULL;
	}
}

int find_nodes_near(Tree* tree, point_t point, double radius) {
	radius *= radius; // sq radius to avoid computing square root in distances
	int i;
	double quadrance_dist;
	FOR_EACH_NODE(i, *tree) {
		quadrance_dist = quadrance(point, tree -> positions[i]);
		if(quadrance_dist < radius) {
			printf("%f\n", quadrance_dist);
			return i;
		}
	}
	return -1;
}

void move_tree(Tree* destination, Tree* source) {
	// STEP 1: clear residual data from destination tree
	destroy_tree(destination);

	// STEP 2: move all the pointers from one structure to the other
	destination -> size_memory_children = source -> size_memory_children;
	destination -> size_memory_nodes    = source -> size_memory_nodes;

	destination -> n_nodes    = source -> n_nodes;
	destination -> n_children = source -> n_children;
	destination -> children   = source -> children;

	destination -> positions             = source -> positions;
	destination -> node_bounding_boxes   = source -> node_bounding_boxes;

	destination -> labels   = source -> labels;

	// STEP 2: nullify all the pointers from source (so that it can be destroyed safely)
	source -> size_memory_children = NULL;
	source -> n_children           = NULL;
	source -> children             = NULL;
	source -> node_bounding_boxes  = NULL;
	source -> positions            = NULL;
	source -> labels               = NULL;
	// and nullify size_memory attributes so that "destroy_tree" is vacuous
	source -> size_memory_nodes = 0;
	source -> n_nodes           = 0;
}



/**************************************
DISPLAY METHODS
***************************************/

/************ GRAPHICAL DISPLAY METHODS ***********************************/
void compute_bounding_boxes_from_label_extents(Tree* tree) {
	int i;
	FOR_EACH_NODE(i, *tree)
	{
		if(tree -> labels[i] != NULL && tree -> labels[i][0] != '\0') {
			tree -> node_bounding_boxes[i] = get_extent(tree -> labels[i]);
			tree -> node_bounding_boxes[i].x += defaults.padding_labels[0];
			tree -> node_bounding_boxes[i].y += defaults.padding_labels[1];
		}
		else {
			tree -> node_bounding_boxes[i].x = 0;
			tree -> node_bounding_boxes[i].y = 0;
		}		
	}
}

void compute_positions(Tree* tree, point_t root_position) {
	int n_nodes = tree -> n_nodes; 
	int i, j;

	WITH_ARRAY(point_t*, position_to_mother, tree -> n_nodes, ({
	WITH_ARRAY(float,    width_nodes,        tree -> n_nodes, ({

	FOR_EACH_NODE(i, *tree)
	{
		ALLOC(position_to_mother[i], tree -> n_children[i], point_t);
	}


		/************* BACKWARD PASS ************************************/
		/*
		Compute positions relative to mother
		*/
		float sum_width, running_width_sum;
		float running;
		for (i = n_nodes - 1; i >= 0; i--)
		{
			if(is_leaf(*tree, i)) {
				width_nodes[i] = MAX(tree->node_bounding_boxes[i].x, defaults.min_width_node);
			}
			else {
				// Compute sums of width of all daughters
				sum_width = 0;
				FOR_EACH_CHILD(j, *tree, i)
				{
					sum_width += width_nodes[tree -> children[i][j]];
				}
				width_nodes[i] = MAX(sum_width, defaults.min_width_node);
				width_nodes[i] = MAX(tree->node_bounding_boxes[i].x, width_nodes[i]);

				running_width_sum = 0;
				sum_width = 0;
				FOR_EACH_CHILD(j, *tree, i)
				{
					position_to_mother[i][j].x = running_width_sum + width_nodes[tree -> children[i][j]] / 2;
					position_to_mother[i][j].y = defaults.height_node_to_daughter + tree->node_bounding_boxes[i].y;
					running_width_sum += width_nodes[tree -> children[i][j]];
					sum_width         += position_to_mother[i][j].x;
				}

				FOR_EACH_CHILD(j, *tree, i)
				{
					position_to_mother[i][j].x -= sum_width / 2;
				}

			}
		}

		/************* FORWARD PASS ************************************/

		tree->positions[0] = root_position;
		point_t mother_position;
		FOR_EACH_NODE(i, *tree)
		{
			mother_position = tree->positions[i];
			FOR_EACH_CHILD(j, *tree, i)
			{
				tree->positions[tree -> children[i][j]].x = mother_position.x + position_to_mother[i][j].x;
				tree->positions[tree -> children[i][j]].y = mother_position.y + position_to_mother[i][j].y;
			}
		}


	FOR_EACH_NODE(i, *tree)
	{
		free(position_to_mother[i]);
	}

	})); // end allocation "width_nodes"
	})); // end allocation "position relative to mother"
}


void draw_tree(const Tree* tree) {
	int i, j;

	begin_path();

    /* DRAW EDGES */
	stroke_color(defaults.color_edge);
	stroke_width(defaults.width_edge);
	FOR_EACH_NODE(i, *tree)
	{
		FOR_EACH_CHILD(j, *tree, i)
		{
			draw_line(
				tree->positions[i].x,
				tree->positions[i].y + tree->node_bounding_boxes[i].y,
				tree->positions[tree -> children[i][j]].x,
				tree->positions[tree -> children[i][j]].y
			);
		}
	}
	stroke();

    /* DRAW LABELS */
    point_t text_position;
	FOR_EACH_NODE(i, *tree)
	{
		if(tree -> labels[i] != NULL && tree -> labels[i][0] != '\0') {
			text_position.x = tree->positions[i].x - (tree->node_bounding_boxes[i].x/2) + (defaults.padding_labels[0] / 2);
			text_position.y = tree->positions[i].y + tree->node_bounding_boxes[i].y     - (defaults.padding_labels[1] / 2);
			fill_text(
				tree -> labels[i], 
				text_position
			);
		}
	}
}


/************ TEXT DISPLAY METHODS ***********************************/

void display_aux(Tree* tree, int node, int tabulation) {
	int i;
	// char* content;
	// LOG("efezfez\n");
	for (i = 0; i < tabulation; ++i)
	{
		LOG("\t");
	}
	if(tree -> labels[node] != NULL) {
		LOG("%d[%s]\n", node, tree -> labels[node]);
	}
	else {
		LOG("%d\n", node);
	}
	// content = (tree -> labels[node] != NULL) ? tree -> labels[node] : "*";

	FOR_EACH_CHILD(i, *tree, node)
	{
		display_aux(
			tree, 
			tree -> children[node][i], 
			tabulation + 1
		);
	}
}

void display(Tree* tree) {
	display_aux(tree, 0, 0);
}