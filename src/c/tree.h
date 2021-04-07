#ifndef TREE_H
#define TREE_H

#include "types.h"
#include "geometry.h"

/**************************************
CONSTANTS
***************************************/


#define INITIAL_NUMBER_NODES    128
#define SIZE_NODES_INCREMENT    16
#define INITIAL_NUMBER_CHILDREN 3
#define MIN_LABEL_LENGTH        20

/**************************************
TYPES
***************************************/


typedef void    (*content_displayer_t)(char* content);
typedef point_t (*get_extent_t)(char* label);

typedef struct {
	// Memory stuff
	int*   size_memory_children;
	int    size_memory_nodes;

	// Abstract Geometry
	int    n_nodes;
	int*   n_children;
	int**  children;

	// Display Geometry
	point_t*  node_bounding_boxes;
	point_t*  positions;

	// Content
	char** labels;
} Tree;

typedef struct {
	double space_between_sisters;
	double height_node_to_daughter;
	double height_node;
	double min_width_node;

	double padding_labels[2];
	double padding_inner_node_labels[2];

	Color color_edge;
	Color color_label;
	
	double width_edge;
	
} Options;


/**************************************
MEMORY MANAGEMENT METHODS
***************************************/

void init_tree(Tree*);
void destroy_tree(Tree*);
void make_more_space_nodes(Tree* tree);
#define free_content(tree, node_index)           \
     if(((tree).labels[node_index] != NULL)) {   \
     	free((tree).labels[node_index]);         \
     }
     	// printf("Freeing %i", node_index);        \


/**************************************
TREE MANIPULATION METHODS
***************************************/

int create_node(Tree*);
int create_node_at(Tree* tree, int index);
int delete_node(Tree* tree, int node);
void set_label(Tree*, int node_index, char*);

void trim(Tree*);
int sprout(Tree *tree, int node);


#define is_leaf(tree, node_index)      ((tree).n_children[node_index] == 0)
int find_nodes_near(Tree*, point_t, double radius);



/**************************************
DISPLAY METHODS
***************************************/


void compute_bounding_boxes_from_label_extents(Tree*);
void compute_positions(Tree*, point_t root_position);
void draw_tree(const Tree*);
void display(Tree* tree);



/**************************************
CONTROL STRUCTURES FOR TREE
***************************************/


#define WITH_TREE(NAME, SCOPE) { \
	Tree NAME;                   \
	init_tree(&NAME);            \
	SCOPE;                       \
	destroy_tree(&NAME);         \
}

#define WITH_POSITIONS(NAME, TREE, ROOT, SCOPE) {    \
	point_t *NAME = compute_positions(&TREE, ROOT);  \
	SCOPE;                                            \
	free(NAME);                                      \
}

#define FOR_EACH_NODE(INDEX, TREE) for(INDEX = 0; INDEX < (TREE).n_nodes; INDEX++)

#define FOR_EACH_CHILD(INDEX, TREE, NODE) for(INDEX = 0; INDEX < (TREE).n_children[NODE]; INDEX++)

/**************************************
INTERNALS
***************************************/

#define INIT_POINT_ZERO(point) (point).x = 0;	(point).y = 0;


#endif