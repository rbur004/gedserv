void output_pedigree(FILE *ofile, ged_type *rt, int max_depth);
void output_decendants( FILE *ofile, ged_type *rt, int max_depth );
void output_decendants_of_name( FILE *ofile, ged_type *rt, int max_depth);

void output_pedigree_info
(
	FILE *ofile,
	ged_type *rt,
	int curdepth,
	int direction,
	unsigned int *glob_map,
	char *child_marker,
	int max_depth
);

int output_decendants_info
(
	FILE *ofile,
	ged_type *rt,
	int curdepth,
	int direction,
	unsigned int *glob_map,
	int rel_chart,
	int max_depth
);

int output_decendants_info_of_name
(
	FILE *ofile,
	ged_type *rt,
	int curdepth,
	int direction,
	unsigned int *glob_map,
	int max_depth
);

int output_pedigree_name
(
	FILE *ofile,
	ged_type *indiv,
	int depth,
	ged_type *famc,
	ged_type *fams,
	unsigned int *map,
	char *prefix,
	char *child_marker,
	int  blank,
	char *suffix,
	int max_depth
);

void print_bars(FILE *ofile, unsigned int *map, int depth);
