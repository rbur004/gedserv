#ifndef PATHNAMES_H
#define PATHNAMES_H

typedef struct
{
	char *base;
	char *name;
} t_action;

typedef struct
{
	char *name;
	int length;
} t_gedfile;

#define PATH "data:Family.Tree:html:"

#define INDEX_DIR "indexes"
#define HTML_DIR  "html"

#ifdef CROLL_C

t_gedfile *Ged_File; //filled in by main from arg list
//    =
//{
// 	"data:Family.Tree:Gedcom.Files:genealog.roy92",0, 
//	"data:Family.Tree:Gedcom.Files:Pollard2.ged",0,
//	"data:Family.Tree:Gedcom.Files:burrowes.ged",0,
//	"data:Family.Tree:Gedcom.Files:Fitzherbert.ged",0,
//	"data:Family.Tree:Gedcom.Files:Forte.ged",0,
//	0,0
//};

t_action Ancestor_Tree[] =
{
//	"@IR1261@", "RoyalTA.html", 
//	"@I4576@", "Pollard2TA.html",
//	"@I5@", "burrowesTA.html",
//	"@IW1@", "crollTA.html",
//	"@IB378@", "FitzherbertTA.html",
//	"@IB1127@", "AlleyneTA.html",
//	"@IBC66@", "ForteTA.html",
//	"@IPB24@", "PollardTA.html",
	0,0
};
	
t_action Descendants_Tree[] =
{
//	"@I1027@", "Pollard2TD.html",
//	"@I7@", "richardsTD.html",
//	"@IW970@", "brassTD.html",
//	"@IW1693@", "crollTD.html",
//	"@IB1@", "FitzherbertTD.html",
//	"@IB1000@", "AlleyneTD.html",
//	"@IBC1@", "ForteTD.html",
//	"@IPB24@", "PollardTD.html",
	0,0
};

t_action Descendants_Single_Tree[] =
{
//	"@I405@", "burrowesTDN.html",
	0,0
};

#else //CROLL_C

extern t_gedfile *Ged_File;
extern t_action Ancestor_Tree[]; 
extern t_action Descendants_Tree[]; 
extern t_action Descendants_Single_Tree[];

#endif //CROLL_C


#endif

