#ifndef GED_H
#define GED_H

//A GEDCOM file has line based records containing level type and data.
//The zero level records are 0 index type.
//Each record can have sub records of the same form.


typedef struct rel_list_struct
{
	struct rel_list_struct *next;
	struct ged_record *rt;
	struct ged_record *srt;
	int level;  //when used as a link to the previous child this has the generation 
				//of the current node through this child
	int direct;
} rel_list_type;

struct ged_record
{
	int level;
	char *type;
	char *data; //Also used as the index in 0 length fields
	int processed;

	struct ged_record *next;		//All records linked together
	struct ged_record *last;		//All records linked together

	struct ged_record *hash_link; 	//Hash collision resolution by chaining
	struct ged_record *NAMEhash_link; 	//Hash collision resolution by chaining

	struct ged_record *next_this_type; //Next Level 0 record of this type
	struct ged_record *last_this_type; //Last Level 0 record of this type

	struct ged_record *next_sorted; //Next Level 0 record of this type
	struct ged_record *last_sorted; //Last Level 0 record of this type
	
	//The next two are used to help find how two individuals are related.
	rel_list_type *rel_child_1; //Keeps track of child we came to this node through.
	rel_list_type *rel_child_2; //We need to also track path from second start point.
};

typedef  struct ged_record ged_type;

#ifdef GED_C

ged_type *hash_table[1024];
ged_type *NAMEhash_table[1024];

ged_type head_INDI;		//Head of list of individuals records
ged_type head_FAM;	 	//Head of list of Family records
ged_type head_EVEN; 	//Head of list of Event records
ged_type head_NOTE; 	//Head of list of Note records
ged_type head_SOUR; 	//Head of list of Source records
ged_type head_REPO; 	//Head of list of Repository records
ged_type all;   		//Head of all level 0 data entered

#else

extern ged_type *hash_table[1024];
extern ged_type *NAMEhash_table[1024];

extern ged_type head_INDI;		//Head of list of individuals records
extern ged_type head_FAM;	 	//Head of list of Family records
extern ged_type head_EVEN; 	//Head of list of Event records
extern ged_type head_NOTE; 	//Head of list of Note records
extern ged_type head_SOUR; 	//Head of list of Source records
extern ged_type head_REPO; 	//Head of list of Repository records
extern ged_type all;   		//Head of all level 0 data entered

#endif

typedef struct 
{
	char *type;
	int	 Value;
} typemap_t;

#define INDI 	1
#define FAM 	2
#define EVEN 	3
#define NAME	4
#define CSPO	5
#define FCHI	6
#define FATH	7
#define MOTH	8
#define OSIB	9
#define YSIB	10
#define MARR	11
#define LSPO	13
#define ESPO	14
#define SPOU	15
#define SEX		16
#define BIRT	17
#define FAMS	18
#define FAMC	19
#define HUSB	20
#define WIFE	21
#define CHIL	22
#define HEAD	23
#define DATE	24
#define PLAC	25
#define QUAY	26
#define NOTE	27
#define DEAT	28
#define DIVO	29
#define OCCU	30
#define PICT	31
#define TITL	32
#define BURI	33
#define EDUC	34
#define SOUR	35
#define REPO	36
#define CONT	37
#define RESN	38
#define CONC	39
#define CHR	    40
#define WILL	41
#define ADOP	42
#define CREM  43

#define PRIVACY "privacy"

#ifdef GED_C

typemap_t typemap[] =
{
	 "INDI", 	INDI 	,
	 "FAM", 	FAM 	,
	 "EVEN", 	EVEN 	,
	 "CSPO",	CSPO	,
	 "NAME",	NAME	,
	 "FATH",	FATH	,
	 "MOTH",	MOTH	,
	 "FCHI",	FCHI	,
	 "YSIB",	YSIB	,
	 "OSIB",	OSIB	,
	"MARR",		MARR	,
	"LSPO",		LSPO	,
	"ESPO",		ESPO	,
	"SPOU",		SPOU	,
	 "SEX",		SEX		,
	 "BIRT",	BIRT	,
	"FAMS",		FAMS	,
	"FAMC",		FAMC	,
	"HUSB",		HUSB	,
	"WIFE",		WIFE	,
	"CHIL",		CHIL	,
	"HEAD",		HEAD	,
	"DATE",		DATE	,
	"PLAC",		PLAC	,
	"QUAY",		QUAY	,
	"NOTE", 	NOTE	,
	"DEAT",		DEAT	,
	"DIVO",		DIVO	,
	"OCCU",		OCCU	,
	"_PICT",	PICT	,
	"TITL",		TITL	,
	"BURI",		BURI	,
	"EDUC",		EDUC	,
	"SOUR",		SOUR	,
	"REPO",		REPO	,
	"CONT",		CONT	,
	"RESN",		RESN	,
	"CONC",		CONC	,
	"CHR",		CHR	    ,
	"WILL",	    WILL    ,
	"ADOP",     ADOP    ,
	"CREM",     CREM    ,
	 (char *)0, 0 	
};


#else

extern typemap_t typemap[];

#endif


#endif

