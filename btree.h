#ifndef BTREE_H
#define BTREE_H

typedef struct
{
	ged_type *name;
	ged_type *indi;
} tree_data;

typedef struct btree_node_struct
{
	//nodes data
	tree_data *data;

	int balance; //-1, if right subtree is deeper than the left subtree
				 // 0, if the right and left sub-trees are the same height
				 // 1, if the left sub-tree is deeper than the right sub-tree
	struct btree_node_struct *left;  //left sub-tree of words alphabetically before this nodes word
	struct btree_node_struct *right; //Right sub-tree of words alphabetically after this nodes word
} btree_node;

typedef  btree_node *btree_node_p;

#ifdef BTREE_C

static btree_node_p balance_LL (btree_node_p A);
static btree_node_p balance_LR (btree_node_p A);
static btree_node_p rotate_right (btree_node_p tree);
static btree_node_p balance_RR (btree_node_p A);
static btree_node_p balance_RL (btree_node_p A);
static btree_node_p rotate_left (btree_node_p tree);
static char *extract_firstnames(tree_data *node, char *firstnames);
static int cmp_node(tree_data *d1, tree_data *d2) ;
static btree_node_p add_node( btree_node_p tree, tree_data *data);
static void dump_tree_body(FILE *fp, btree_node_p tree, char *filename, int surname_or_full, char *thesurname, int no_surname);
static int char_to_index(char c);
static int surname_char(char *name);

#endif

char *extract_surname(tree_data *node, char *surname);
void dump_by_match(FILE *fp, char *match_me);
void dump_Surname_as_html(FILE *fp, char letter, int no_surname);
void dump_index_as_html(FILE *fp, char letter, int no_surname);
void dump_t_index_as_html(FILE *fp, btree_node_p r);
void dump_shortindex_as_html(FILE *fp, char *thesurname, int no_surname);
void create_index(void);
btree_node_p add_index_name(btree_node_p root, ged_type *rt);

#endif
