#ifndef RELATION_H
#define RELATION_H

#include "ged.h"

#ifdef RELATION_C
static void Clear_trace_1(ged_type *rt);
static void Clear_trace_2(ged_type *rt);
static void trace_indi_1(ged_type *rt,  ged_type *srt, ged_type *child, int level);
static void trace_indi_2(ged_type *rt,  ged_type *srt, ged_type *child, int level);
static void relation_chart
(
	FILE *ofile, 
	ged_type *rt,
	int curdepth,
	unsigned int *glob_map, ged_type *rt_1, ged_type *rt_2,
	int max_depth
);

static void init_rel_list(); //the list should start empty
static void add_to_rel_list(ged_type *rt, ged_type *srt);
static void add_to_rel_list_X(rel_list_type **X, ged_type *rt, ged_type *srt, int level);
static void process_rel_list(FILE *fout, ged_type *rt_1, ged_type *rt_2, int maxdepth); //return the lists memory to the pool
static void free_rel_list(); //return the lists memory to the pool
static void free_rel_list_X(rel_list_type **X); //return the lists memory to the pool
static void remove_from_rel_list(ged_type *rt, ged_type *srt);
#endif //RELATION_C

int in_rel_list(rel_list_type *X, ged_type *rt);
rel_list_type * in_rel_list_X(rel_list_type *X, ged_type *rt, ged_type *srt, int level); //return the lists memory to the pool
void find_relationship
(
	FILE *fout,
	ged_type *rt_1,
	ged_type *rt_2,
	int	maxdepth
);

#endif // RELATION_H

