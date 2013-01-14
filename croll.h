#ifndef CROLL_H
#define CROLL_H

void init_gedlists();
void add_to_all(ged_type *line);
void test_ats(char *link);
void insert_line(ged_type *line_before, ged_type *line_to_insert);
void add_list(ged_type *line, ged_type *list);
char *add_ats(char * s);
void add_hash(ged_type *line);
void check_present(char *key);
void add_NAMEhash(ged_type *line);
int type_to_num(char *type);
ged_type *attach(ged_type *insert, char *level_p, char *type_p, char *data_p);
ged_type * dup_data_and_attach(ged_type *insert, int level, char *type, char *data);
ged_type *find_hash(char *key);
ged_type *find_NAMEhash(char *key);
ged_type *Next_of_this_Name(ged_type *n);
void parse_gedcom(char *buff, int num_files);
char *enclosing_records_key(ged_type *l);
ged_type *find_type(ged_type * g, int type);
ged_type *find_next_this_type(ged_type *base, ged_type *last_one);
void Print_enclosing_record(ged_type *l);
void Print_record(ged_type *g);
ged_type *find_first_spou(ged_type *g);
void follow_child_list(ged_type *parent, ged_type *child);
ged_type *exists(ged_type *cspo, ged_type *r);
void dump_gedcom();
int filesize(int fd);
void load_gedcom(void);
void check_links();
void check_dates(void);
void check_for_in_fam(void);
char *strip_ats(char *buff, char *ref);
void generate_families();
void check_for_duplicates();
void delete_line(ged_type *line);
void delete_level(ged_type *line);
void del_NAMEhash(ged_type *line);
void display_stats(void);
int all_present_and_correct(char *indi, char *fam, ged_type *marr, ged_type *fam_marr);
void dump_notes(FILE *fp, ged_type *record, int indent);
void html_dump_indi(FILE *fp, ged_type *indi, int parent);
void Indi_html_dump(FILE *fp, ged_type *g);
void Family_html_dump(FILE *fp, ged_type *g);
char *extract_filename(char *data);
void reload(void);
void html_dump(void);
#endif

