#ifndef GED_DUMP_H
#define GED_DUMP_H

void Family_GED_dump(FILE *fp, ged_type *g);

#ifdef GED_DUMP_C

char *link_types[] =
{
	"FAMC", "FAMS", "HUSB", "WIFE", "CHIL", "REPO", "SOUR", "NOTE"
};

#define NLINK_TYPES 8

#endif //GED_DUMP_C

#endif //GED_DUMP_H
