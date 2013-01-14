#ifndef REL_INDEX_H
#define REL_INDEX_H

void output_pedigree_index( FILE *ofile, ged_type *rt , int depth);
void output_descendants_index( FILE *ofile, ged_type *rt, int depth);
void create_pedigree_index
(
	btree_node_p *root,
	ged_type *rt,
	int depth,
	int current_depth
);
void create_descendants_index
(
	btree_node_p *root,
	ged_type *rt,
	int depth,
	int current_depth
);

#endif

