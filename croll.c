#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
 #include <ctype.h>

#define GED_C
#define CROLL_C

#include "ged.h"
#include "hash.h"
#include "croll.h"
#include "stringfunc.h"
#include "date.h"
#include "btree.h"
#include "chart.h"
#include "pathnames.h"

void init_gedlists()
{
int i;

  for(i= 0; i < 1024; i++) //ensure hashtables are zero'd
  {
    hash_table[i] = (ged_type *) 0;
    NAMEhash_table[i] = (ged_type *) 0;
  }

  head_INDI.next_this_type = &head_INDI;  //Head of list of individuals records
  head_INDI.last_this_type = &head_INDI;  //Head of list of individuals records
  head_INDI.next_sorted = &head_INDI;   //Keep a sorted name list
  head_INDI.last_sorted = &head_INDI;
  head_INDI.level = -1;         //Makes tests for same level work.

  head_FAM.next_this_type = &head_FAM;  // Head of list of Family records
  head_FAM.last_this_type = &head_FAM;  // Head of list of Family records
  head_FAM.level = -1;          //Makes tests for same level work.

  head_EVEN.next_this_type = &head_EVEN;  //Head of list of Event records
  head_EVEN.last_this_type = &head_EVEN;  //Head of list of Event records
  head_EVEN.level = -1;         //Makes tests for same level work.

  head_NOTE.next_this_type = &head_NOTE;  //Head of list of Note records
  head_NOTE.last_this_type = &head_NOTE;  //Head of list of Note records
  head_NOTE.level = -1;         //Makes tests for same level work.

  head_SOUR.next_this_type = &head_SOUR;  //Head of list of Source records
  head_SOUR.last_this_type = &head_SOUR;  //Head of list of Source records
  head_SOUR.level = -1;         //Makes tests for same level work.

  head_REPO.next_this_type = &head_REPO;  //Head of list of Source records
  head_REPO.last_this_type = &head_REPO;  //Head of list of Source records
  head_REPO.level = -1;         //Makes tests for same level work.

  all.next = &all;          //Head of all data entered
  all.last = &all;          //Head of all data entered

  all.next_this_type = &all;
  all.last_this_type = &all;

  all.next_this_type = &all;
  all.last_this_type = &all;

  all.next_this_type = &all;
  all.last_this_type = &all;

  all.next_sorted = &all;     //Keep a sorted name list
  all.last_sorted = &all;

  all.level = -1;         //Makes tests for same level work.
}

void test_ats(char *link)
{
int l;

  if(link)
  {
    if((l = strlen(link)) <= 1)
      printf("Link too short %s:%d\n", link, l);
    else
    {
      if(*link != '@')
        printf("No @ at the start of %s\n", link);
      if(link[l - 1] != '@')
        printf("No @ at the end of %s\n", link);
    }
  }
  else
    printf("link null\n");
}

void add_to_all(ged_type *line)
{
  if(line != (ged_type *)0)
  {
    line->next = &all;
    line->last = all.last;
    all.last->next = line;
    all.last = line;
  }
}

void delete_line(ged_type *line)
{
  if(line != (ged_type *)0) //Make sure we have something to delete
  {
    if(line->next && line->last)  //Ensure the link list doesn't have a hole in it
    {
      line->next->last = line->last;
      line->last->next = line->next;
      //Ensure we stop further references to the list
      line->next = 0;
      line->last = 0;
    }
    if(line->level == 0)  //At level 0 we have type lists to fix up
    {
      if(line->next_this_type && line->last_this_type) //validate the list
      { //generic reverse to add_list()
        line->next_this_type->last_this_type = line->last_this_type;
        line->last_this_type->next_this_type = line->next_this_type;
        //Ensure we stop further references to the list
        line->last_this_type = 0;
        line->next_this_type = 0;
      }
    }
    else if(type_to_num(line->type) == NAME) //Name are part of a name hash list
    {
      del_NAMEhash(line);   //del names from name hashtable
    }
  }
}

void delete_level(ged_type *line)
{
int current_level;
ged_type *g;
ged_type *g_next;

  if(line != (ged_type *)0)
  {
    current_level = line->level;
    //Delete the sub-levels of a level
    for(g = line->next; g && g->level > current_level; g = g_next )
    {
      g_next = g->next; //before it is deleted
      delete_line(g);
    }
    delete_line(line);  //Delete the line.
  }
}

void insert_line(ged_type *line_before, ged_type *line_to_insert)
{
  if(line_to_insert && line_before)
  {
    line_to_insert->next = line_before->next;
    line_to_insert->last = line_before;
    line_before->next->last = line_to_insert;
    line_before->next = line_to_insert;
  }
}

void add_list(ged_type *line, ged_type *list)
{
  line->next_this_type = list;
  line->last_this_type = list->last_this_type;
  list->last_this_type->next_this_type = line;
  list->last_this_type = line;
}

char *add_ats(char * s)
{
static char buff[16];

  if(*s == '@')
    return s;
  else
  {
    buff[0] = '@';
    strcpy(&buff[1], s);
    strcat(buff, "@");
    return buff;
  }
}

void check_present(char *key)
{
  if(find_hash(key))
    printf("key %s already present\n", key);

}

void add_hash(ged_type *line)
{
int hash;
  test_ats(line->data);
  check_present(line->data);
  hash = hash_string((unsigned char *) line->data, 1024);
  line->hash_link = hash_table[hash];   //Hash collision resolution by chaining
  hash_table[hash] = line;        //Add to front of list (easiest soln.)
}

void add_NAMEhash(ged_type *line)
{
int hash;
  hash = hash_string((unsigned char *)line->data, 1024);
  line->NAMEhash_link = NAMEhash_table[hash];   //Hash collision resolution by chaining
  NAMEhash_table[hash] = line;        //Add to front of list (easiest soln.)
}

void del_NAMEhash(ged_type *line)
{
int hash;
ged_type *g;

  hash = hash_string((unsigned char *)line->data, 1024);
  if(NAMEhash_table[hash] == line)
    NAMEhash_table[hash] = line->NAMEhash_link;
  else
    for(g = NAMEhash_table[hash]; g ; g = g->NAMEhash_link)
    {
      if(g->NAMEhash_link == line)
      {
        g->NAMEhash_link = line->NAMEhash_link;
        line->NAMEhash_link = 0; //just in case we use it somewhere else.
        break;
      }
    }
}

int type_to_num(char *type)
{
typemap_t *t;

  for(t = typemap; t->type; t++)
    if( strcmp(type, t->type) == 0)
      return t->Value;
  return 0;
}

ged_type *attach(ged_type *insert, char *level_p, char *type_p, char *data_p)
{
ged_type *this_line;
int level = atoi(level_p);

  if((this_line = malloc(sizeof(ged_type))) == 0)
    return (ged_type *) 0;
  this_line->level = level;
  this_line->type = type_p;
  this_line->data = data_p;
  this_line->hash_link = 0;     //Hash collision resolution by chaining
  this_line->NAMEhash_link = 0;
  this_line->next_this_type = 0; //Next record of this level
  this_line->last_this_type = 0; //Next record of this level
  this_line->rel_child_1=0;
  this_line->rel_child_2=0;
  this_line->processed = 0;
  if(insert)
    insert_line(insert, this_line);
  else
    add_to_all(this_line); //sets next and last
  if(level == 0)
  {
    //For level zero lines we need to add to hash index and to add to type chains
    add_hash(this_line);
    switch(type_to_num(this_line->type))
    {
    case INDI:
      add_list(this_line, &head_INDI); break;
    case FAM:
      add_list(this_line, &head_FAM); break;
    case EVEN:
      add_list(this_line, &head_EVEN); break;
    case SOUR:
      add_list(this_line, &head_SOUR); break;
    case NOTE:
      add_list(this_line, &head_NOTE); break;
    case REPO:
      add_list(this_line, &head_REPO); break;
    }
  }
  else
  {
    switch(type_to_num(this_line->type))
    {
    case NAME:
      add_NAMEhash(this_line); break; //add names to name hashtable
    }
  }
  return this_line;
}

ged_type * dup_data_and_attach(ged_type *insert, int level, char *type, char *data)
{
//copy all fields to new area and create record
int type_len = strlen(type) + 1;
int data_len = strlen(data) + 1;
char *buffer;
char level_buff[8];

  if((buffer = malloc(type_len + data_len)) == 0)
    return 0;
  strcpy(buffer, type);
  strcpy(&buffer[type_len], data);
  sprintf(level_buff, "%d", level);
  return attach(insert, level_buff, buffer, &buffer[type_len]);
}

ged_type *find_hash(char *key)
{
int hash;
ged_type *line;
char *new_key;


  new_key = add_ats(key);
  hash = hash_string((unsigned char *)new_key, 1024); //Hash the key
  line = hash_table[hash]; //Retrieve the first line pointer
  while(line)
  {
    //printf("Trying %s\n", line->data);
    if(strcmp(line->data,  new_key) == 0) //Try to match key
      return line;
    line = line->hash_link; //Failed, so try the next in the chain
  }
  //printf("didn't find %s\n", key);
  return (ged_type *) 0;
}

ged_type *find_NAMEhash(char *key)
{
int hash;
ged_type *line;


  hash = hash_string((unsigned char *)key, 1024); //Hash the key
  line = NAMEhash_table[hash]; //Retrieve the first line pointer
  while(line)
  {
    //printf("Trying %s\n", line->data);
    if(strcmp(line->data,  key) == 0) //Try to match key
      return line;
    line = line->NAMEhash_link; //Failed, so try the next in the chain
  }
  //printf("didn't find %s\n", key);
  return (ged_type *) 0;
}

ged_type *Next_of_this_Name(ged_type *n)
{
char *this_name = n->data;

  for(n = n->NAMEhash_link; n; n=n->NAMEhash_link)
  {
    if(strcmp(n->data,  this_name) == 0) //Try to match key
      return n;
  }
  return (ged_type *) 0;

}


void parse_gedcom(char *buff, int num_files)
{
char *l = buff; //Line pointer
char *nl;   //Next line Pointer
int level;    //This lines level as an integer
char *level_p;  //Pointer to string holding this lines level as string
char *type_p; //Pointer to line type string
char *data_p; //Pointer to lines data or a level 0 index key
int lineno = 1; //Line number for error reporting

  do
  {
    nl = skip_to_next_line(l);  //Null terminate this line
    if((level_p = skip_spaces(l)) != 0)
    {   //Line contains something
      if(*level_p == '0')
      { //This is a level 0 line (We assume no leading 0's)
        if((data_p = next_field(level_p)) == 0)
        {
          printf("Skipping: No INDEX, line %d: %s\n", lineno, l);
        }
        else if((type_p = next_field(data_p)) == 0)
        {
          if(strcmp(data_p, "TRLR") == 0)
          {
            if(--num_files <= 0) //finished if parsed all files
            {
              break;  //End of valid data
            }
          }
          else if(strcmp(data_p, "HEAD") == 0)
          {
            attach(0, level_p, data_p, "");
          }
          else
            printf("Skipping: No TYPE, line %d: %s %s\n", lineno, l, data_p);
        }
        else
        {
          //We have a complete level 0 line
          next_field(type_p); //null terminate type.
          attach(0, level_p, type_p, data_p);
        }
      }
      else if(isdigit(*level_p))
      { //This is a sublevel line
        if((type_p = next_field(level_p)) == 0)
        {
          printf("Skipping: No TYPE, line %d: %s \n", lineno, l);
        }
        else
        {
          if((data_p = next_field(type_p)) == 0)
            data_p = "";
          attach(0, level_p, type_p, data_p);
        }
      }
      else
      {
        printf("Skipping: No level, line %d: %s\n", lineno, l);
      }
    }
    else
      printf("Skipping: Blank Line, line %d\n", lineno);
    l = nl; //Shift to next line (or EOF)
    lineno++;
  } while (l); //drop out if out of lines.
  printf("No Lines = %d\n", lineno);
}

char *enclosing_records_key(ged_type *l)
{
ged_type *g;
  if(l->level == 0)
    return l->data;
  for(g = l->last; g != &all; g=g->last)
    if(g->level == 0)
      return g->data;
  return "@@";
}

ged_type *find_type(ged_type * g, int type)
{
int level;
// FInds TYPE record within given record.
// It looks only at the level one above the level of the Base record.

  if(g == 0 || type == 0)
    return 0;
  level = g->level;
  for(g = g->next; g && g != &all && g->level > level; g=g->next)
  {
    if(g->level == level + 1 && type_to_num(g->type) == type)
      return g;
  }
  return (ged_type *) 0;
}

ged_type *find_next_this_type(ged_type *base, ged_type *last_one)
{
int level;
int type;

  if(last_one == 0 || base == 0)
    return 0;

  level = base->level;
  type = type_to_num(last_one->type);

  for(last_one = last_one->next;
    last_one && last_one != &all && last_one->level > level;
    last_one=last_one->next)
  {
    if(last_one->level == level + 1 && type_to_num(last_one->type) == type)
      return last_one;
  }
  return (ged_type *) 0;
}

ged_type * find_spouse(ged_type *fam, char * indi_reference)
{
ged_type * spouse;
char spouse_reference[32];
ged_type * family;

  family = find_hash(fam->data);
  if( (spouse = find_type(family, HUSB)) )
  {
    strip_ats(spouse_reference, spouse->data);
    if( strncmp(indi_reference, spouse_reference, 32) == 0 ) //Looking at self, not spouse.
    {
      if((spouse = find_type(family, WIFE)) == (ged_type *) 0) //Look for WIFE instead
        return (ged_type *) 0; //Didn't find a spouse.

      strip_ats(spouse_reference, spouse->data);
      if( strncmp(indi_reference, spouse_reference, 32) == 0 ) //Looking at self, not spouse, which is an error in the gedcom file.
        return (ged_type *) 0; //Didn't find a spouse.
    }
  }
  else //no HUSB or WIFE recorded, or just one is recorded, which means it must be self (or a gedcom error)
    return (ged_type *) 0; //Didn't find a spouse.

  return find_hash(spouse->data);
}

void Print_enclosing_record(ged_type *l)
{
ged_type *g = 0;
int i;

  if(l == 0)
  {
    printf("Print_enclosing_record passed a null pointer\n");
    return;
  }
  if(l->level != 0)
  {
    for(l = l->last; l && l != &all; l=l->last)
    {
      if(l->level == 0)
      {
        g = l; break;
      }
    }
    if( g == 0 )
    {
      printf("Print_enclosing_record No enclosing record\n");
      return;
    }
  }
  else
    g = l;

  printf("0 %s %s\n", g->data, g->type);
  for(g = g->next; g && g->level != 0; g = g->next)
  {
    for(i = 0; i < g->level; i++)
      putchar(' ');
    printf("%d %s %s\n", g->level, g->type, g->data);
  }
}

void Print_record(ged_type *g)
{
int i;
int current_level;

  if(g == 0)
  {
    printf("Print_record passed a null pointer\n");
    return;
  }
  if( (current_level = g->level) )
  {
    for(i = 0; i < g->level; i++)
      putchar(' ');
    printf("%d %s %s\n", g->level , g->type, g->data);
  }
  else
    printf("0 %s %s\n", g->data, g->type);
  for(g = g->next; g && g->level > current_level; g = g->next)
  {
    for(i = 0; i < g->level; i++)
      putchar(' ');
    printf("%d %s %s\n", g->level, g->type, g->data);
  }
}

ged_type *find_first_spou(ged_type *g)
{
ged_type *prev = g;

  while((prev = find_type(prev, ESPO)) && (prev = find_hash(prev->data)))
    g = prev;
  return g;
}

void follow_child_list(ged_type *parent, ged_type *child)
{
ged_type *first_child;

  if((first_child = find_type(parent, FCHI)) && (first_child = find_hash(first_child->data)))
  {
    if(first_child == child)
      return;
    if(first_child == find_first_spou(child))
        return;
    while((first_child = find_type(first_child, YSIB))
       && (first_child = find_hash(first_child->data)))
    {
      if(first_child == find_first_spou(child))
        return;
    }
  }
  printf("%s not in %s chain\n", child->data, parent->data);
}

void check_links()
{
ged_type *g;
ged_type *h;
ged_type *fam;
ged_type *fams;
ged_type *famc;
ged_type *base_record;
ged_type *this_line;
ged_type *name;
ged_type *husb;
tree_data husb_name;
ged_type *wife;
ged_type *husb_rec;
ged_type *wife_rec;
ged_type *chil;
tree_data chil_name;
ged_type *sex;
char indicated_sex[2];
int spouse;
char *atp1, *atp2;
char name1[128], name2[128];

  indicated_sex[1] = '\0';
  for(base_record = head_INDI.next_this_type;
     base_record != &head_INDI;
     base_record=base_record->next_this_type) //for each INDI record
  {
    test_ats(base_record->data);

    if((name = find_type(base_record, NAME)) == 0)
      printf("INDI %s has no name record\n", base_record->data);

    for(g = base_record->next; g->level > 0; g = g->next) //For each sub record
    {

      switch(type_to_num(g->type))
      {
      case FAMS:
        test_ats(g->data);
        if( (fam = find_hash(g->data)) )
        {
          husb = find_type(fam, HUSB);
          wife = find_type(fam, WIFE);
          if(wife &&  strcmp(wife->data, base_record->data) == 0)
            indicated_sex[0] = 'F';
          else if(husb &&  strcmp(husb->data, base_record->data) == 0)
            indicated_sex[0] = 'M';
          else
          {
            printf("FAM %s doesn't reference FAMS of INDI %s\n", g->data, base_record->data);
            indicated_sex[0] = '\0';
          }
          if((sex = find_type(base_record, SEX)) == 0)
          {
            if(indicated_sex[0] != '\0')
            {
              printf("Adding SEX %s to INDI %s \n", indicated_sex, base_record->data);
              dup_data_and_attach(base_record, 1,"SEX", indicated_sex);
            }

          }
          else if(indicated_sex[0] != '\0' && *(sex->data) != indicated_sex[0])
            printf("INDI %s SEX already specified as %s\n", base_record->data, sex->data);
        }
        else
          printf("FAM %s for FAMS of INDI %s Not found\n",  g->data, base_record->data);
        break;
      case FAMC:
        test_ats(g->data);
        if( (fam = find_hash(g->data)) )
        {
          if( (chil = find_type(fam, CHIL)) )
          {
            do
            {
              if(strcmp(chil->data, base_record->data) == 0)
                break;
            }while( (chil = find_next_this_type(fam, chil)) );
            if(chil == 0)
              printf("FAM %s doesn't reference FAMC of INDI %s\n", g->data, base_record->data);
          }
          else
            printf("FAM %s doesn't reference FAMC of INDI %s\n", g->data, base_record->data);
        }
        else
          printf("FAM %s for FAMC of INDI %s Not found\n",  g->data, base_record->data);
        break;
      }
    }
  }
  for(base_record = head_FAM.next_this_type; base_record != &head_FAM; base_record=base_record->next_this_type) //for each FAM record
  {
    spouse = 0;
    test_ats(base_record->data);
    husb_name.name = 0;

    for(g = base_record->next; g->level > 0; g = g->next) //For each sub record
    {
      switch(type_to_num(g->type))
      {
      case HUSB:
        test_ats(g->data);
        if( (husb = find_hash(g->data)) )
        {
          husb_name.name = find_type(husb, NAME);
          husb_name.indi = husb;

          if( (fams = find_type(husb,FAMS)) )
          {
            do
            {
              if(strcmp(fams->data, base_record->data) == 0)
                break;
            }while( (fams = find_next_this_type(husb, fams)) );
            if(fams == 0)
              printf("INDI %s doesn't reference HUSB of FAM %s\n", g->data, base_record->data);
          }
          else
            printf("INDI %s doesn't reference HUSB of FAM %s\n", g->data, base_record->data);
          spouse = 1;
        }
        else
          printf("INDI %s for HUSB of FAM %s Not found\n",  g->data, base_record->data);
        break;
      case WIFE:
        test_ats(g->data);
        if( (wife = find_hash(g->data)) )
        {
          if( (fams = find_type(wife,FAMS)) )
          {
            do
            {
              if(strcmp(fams->data, base_record->data) == 0)
                break;
            }while( (fams = find_next_this_type(wife, fams)) );
            if(fams == 0)
              printf("INDI %s doesn't reference WIFE of FAM %s\n", g->data, base_record->data);
          }
          else
            printf("INDI %s doesn't reference WIFE of FAM %s\n", g->data, base_record->data);
          spouse = 1;
        }
        else
          printf("INDI %s for WIFE of FAM %s Not found\n",  g->data, base_record->data);
        break;
      case CHIL:
        test_ats(g->data);
        if( (chil = find_hash(g->data)) )
        {
          if((chil_name.name = find_type(chil, NAME)) && husb_name.name)
          {
            chil_name.indi = chil;
            extract_surname(&husb_name, name1);
            extract_surname(&chil_name, name2);
          //  if(strcasecmp(name1, name2) != 0)
          //    printf("HUSB %s surname %s of FAM %s doesn't match child %s surname %s\n",
          //        husb_name.indi->data, base_record->data, name1, g->data, name2);
          }

          if( (famc = find_type(chil,FAMC)) )
          {
            do
            {
              if(strcmp(famc->data, base_record->data) == 0)
                break;
            }while( (famc = find_next_this_type(chil, famc)) );
            if(famc == 0)
              printf("INDI %s doesn't reference CHIL of FAM %s\n", g->data, base_record->data);
          }
          else
            printf("INDI %s doesn't reference CHIL of FAM %s\n", g->data, base_record->data);
        }
        else
          printf("INDI %s for CHIL of FAM %s Not found\n",  g->data, base_record->data);
        break;
      }
    }
    if(spouse == 0)
      printf("FAM %s doesn't have a spouse record\n", base_record->data);
  }
}

char *strip_ats(char *buff, char *ref)
{
int i = strlen(ref);

  if(*ref == '@')
  {
    ref++; //remove front @
    i--;   //String now 1 shorter
  }
  if(i && ref[i - 1] == '@')
  {
    strncpy(buff, ref, i-1);  //Copy all but last @
    buff[i-1] = '\0';       //Null terminate (strncpy doesn't always do this).
  }
  else
    strcpy(buff, ref);      //Copy entire string

  return buff;          //Return buff we were passed.
}


void check_for_duplicates()
{
ged_type *g;
ged_type *lspo;
ged_type *lspo_rec;
ged_type *name;
int count = 1;

  for(g = head_INDI.next_this_type; g != &head_INDI; g=g->next_this_type) //for each INDI record
  {
    if((lspo = find_type(g,LSPO)) && find_type(g,ESPO) == 0)
    {
      lspo_rec = g;
      do
      {
        if( (name = find_type(lspo_rec, NAME)) )
          printf("%d: INDI %s NAME %s\n", count, lspo_rec->data, name->data);
        else
          printf("%d: INDI %s\n", count, lspo_rec->data);
        if( (lspo_rec = find_hash(lspo->data)) )
          lspo = find_type(lspo_rec, LSPO);
      } while(lspo_rec);
      count++;
    }

  }
}

void check_dates(void)
{
ged_type *g;
ged_type *fams;
ged_type *fams_rec;
ged_type *marr;
ged_type *marr_date;
ged_type *birt;
ged_type *birt_date;
ged_type *deat;
ged_type *deat_date;

  for(g = head_INDI.next_this_type; g != &head_INDI; g=g->next_this_type) //for each INDI record
  {
    if( (fams = find_type(g, FAMS)) )
    {
      if( (fams_rec = find_hash(fams->data)) )
      {
        if( (marr = find_type(fams_rec, MARR)) )
          marr_date = find_type(marr, DATE);
        else
          marr_date = 0;
      }
      else
        printf("Can't locate %s\n", fams->data);
    }
    else
      marr_date = 0;
    if( (birt = find_type(g, BIRT)) )
      birt_date = find_type(birt, DATE);
    else
      birt_date = 0;
    if( (deat = find_type(g, DEAT)) )
      deat_date = find_type(deat, DATE);
    else
      deat_date = 0;
    if(birt_date && deat_date && cmp_dates(birt_date->data, deat_date->data) == 1)
      printf("Warning INDI %s died before he was born b. %s d.%s\n", g->data, birt_date->data, deat_date->data);
    if(birt_date && marr_date && cmp_dates(birt_date->data, marr_date->data) == 1)
      printf("Warning INDI %s married before he was born b. %s m.%s\n", g->data, birt_date->data, marr_date->data);
    if(deat_date && marr_date && cmp_dates(marr_date->data, deat_date->data) == 1)
      printf("Warning INDI %s married after he died m. %s d.%s\n", g->data, marr_date->data, deat_date->data);
  }

}

void check_for_in_fam(void)
{
ged_type *g;
ged_type *fams;
ged_type *famc;
ged_type *marr;

  for(g = head_INDI.next_this_type; g != &head_INDI; g=g->next_this_type) //for each INDI record
  {
    fams = find_type(g, FAMS);
    famc = find_type(g, FAMC);      //Find the cspo's name.
    marr = find_type(g, MARR);
    if(fams == 0 && famc == 0)
      printf("WARNING: INDI %s doesn't have a FAMS or FAMC record\n", g->data);
    if(marr && fams == 0)
      printf("WARNING: INDI %s Has a MARR But no FAMS record\n", g->data);
  }

}

void display_stats(void)
{
ged_type *g;
int nINDI, nFAM;

  nINDI = 0;
  nFAM = 0;
  for(g = all.next; g != &all; g=g->next)
    if(type_to_num(g->type) == FAM)
      nFAM++;
    else if(type_to_num(g->type) == INDI)
      nINDI++;
  printf("Number of Individuals: %d\nNumber of Families: %d\n", nINDI, nFAM);
}

int all_present_and_correct(char *indi, char *fam, ged_type *marr, ged_type *fam_marr)
{
ged_type *g;
ged_type *s;
ged_type *next;
//Checks that the sub fields of marr exist and are the same as the subfields of fam_marr
//Needs work, it reports some diff badly
  if(marr == 0 || fam_marr ==0)
    return 0;

  for(g = marr->next; g && g != &all && g->level > marr->level; g = g->next)
  {
    if(g->level == marr->level + 1 && strcmp(g->type, "CSPO") != 0)
    {
      if( (s = find_type( fam_marr, type_to_num(g->type) ) ) )
      {
        if(field_match(g->data, s->data) == 0)
        {
          printf("INDI %s type %s %s != INDI %s %s\n",indi, g->type, g->data, fam, s->data);
          return 0;
        }
        else if(g->next && g->next->level > g->level)
        {
          all_present_and_correct(indi, fam, g, s);
        }
      }
      else
      {
        printf("INDI %s type %s %s not in INDI %s record\n", indi, g->type, fam, fam_marr->data);
        return 0;
      }
    }
  }
  return 1;
}

void repo_given(FILE *fp, ged_type *record)
{
ged_type *repo;

  if( (repo = find_type(record, REPO)) )
    fprintf(fp, "<a href=\"/ruby/gedrelay.rbx?type=repo&target=%s\"><b>Source</b></a>", repo->data);
}

static void source_body(FILE *fp, ged_type *source)
{
ged_type *g;
int t;
  if(*source->data == '@')
    fprintf(fp, "&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"/ruby/gedrelay.rbx?type=sour&target=%s\"><b>Source</b></a>", source->data);
  else
  {
    fprintf(fp, "<br><ul><b>Source.</b>%s\n", source->data);
    for(g = source->next; g && g != &all && g->level > source->level && ((t = type_to_num(g->type)) == CONT || t == CONC ); g = g->next)
      if(t == CONC)
        fprintf(fp, "%s\n", g->data);
      else
        fprintf(fp, "<BR>%s\n", g->data);
    fprintf(fp, "<br></ul>\n");
  }
}

void source_given(FILE *fp, ged_type *record)
{
ged_type *source;

  if( (source = find_type(record, SOUR)) )
  {
    source_body(fp,source);
    while( (source = find_next_this_type(record, source)) )
      source_body(fp,source);
  }
}


//Note valid unless this is an INDI record
//I have applied it to other records I want to mark as private
int isPrivate(ged_type *record)
{
ged_type *resn;

  if((resn = find_type(record, RESN)) && strcmp(resn->data, PRIVACY) == 0)
    return 1;
  else
    return 0;
}

static void note_body(FILE *fp, ged_type *note, int indent)
{
ged_type *g;
int t;
  if(isPrivate(note) == 0)
  {
    if(indent)
      fprintf(fp, "<ul>");
    else
      fprintf(fp, "<p>");
    fprintf(fp, "<b>Nb.</b> %s\n", note->data);
    for(g = note->next; g && g != &all && g->level > note->level && ((t = type_to_num(g->type)) == CONT || t == CONC ) ; g = g->next)
      if(t == CONC)
        fprintf(fp, "%s\n", g->data);
      else
        fprintf(fp, "<BR>%s\n", g->data);
    source_given(fp, note);
    fprintf(fp, "<br>\n");
    if(indent)
      fprintf(fp, "</ul>");
  }
}

//Nb. NOTE records marked with "RESN private" will not be displayed
void dump_notes(FILE *fp, ged_type *record, int indent)
{
ged_type *note;

  if( (note = find_type(record, NOTE)) )
  {
    note_body(fp, note, indent);
    while( (note = find_next_this_type(record, note)) )
      note_body(fp, note, indent);
  }
}

void html_dump_indi(FILE *fp, ged_type *indi, int parent)
{
int privacy = 0;
ged_type *name;
ged_type *note;
ged_type *g;
ged_type *birt;
ged_type *chr;
ged_type *deat;
ged_type *crem;
ged_type *buri;
ged_type *date;
ged_type *plac;
ged_type *famc;
ged_type *fams;
ged_type *resn;
ged_type *title;
ged_type *will;
ged_type *adop;
ged_type *afamc;
char buff[128];
int mcount;


  if((resn = find_type(indi, RESN)) && strcmp(resn->data, PRIVACY) == 0)
  {
    privacy = 1;
    fprintf(fp, "<dl><dt>");
    fprintf(fp, "<b>%s</b> \n", "Record Withheld at the persons request");
     fams = find_type(indi, FAMS);
    famc = find_type(indi, FAMC);
  }
  if( (name = find_type(indi, NAME)) )
  {
    if(!privacy)
    {
      fprintf(fp, "<dl><dt>");
      if((title = find_type(name, TITL)))
        fprintf(fp, "<b>%s </b>", title->data);

      fprintf(fp, "<b>%s</b> \n", name->data);

      if((adop = find_type(indi, ADOP)))
      { parent = 1;
      }

      if((famc = find_type(indi, FAMC)) && parent)
      {
        strip_ats(buff, famc->data);
        strcat(buff, ".html");
        fprintf(fp, " <a href=\"/ruby/gedrelay.rbx?type=html&target=%s\" ><b>Parents</b></a>. ", buff);
      }

      if(adop && (afamc = find_type(adop, FAMC)))
      {
        strip_ats(buff, afamc->data);
        strcat(buff, ".html");
        fprintf(fp, " <a href=\"/ruby/gedrelay.rbx?type=html&target=%s\" ><b>Adopt Parents</b></a>. ", buff);
      }

      fprintf(fp, "<A NAME=\"%s\"></A>\n", strip_ats(buff, indi->data));
      if(famc)
        fprintf(fp, " Draw <a href=\"/ruby/gedrelay.rbx?type=TA&depth=15&target=%s\"  ><b>Anc.</b></a> Tree. ", buff);

      if( (fams = find_type(indi, FAMS)) )
        fprintf(fp, " Draw <a href=\"/ruby/gedrelay.rbx?type=TD&depth=15&target=%s\"  ><b>Desc.</b></a> Tree.\n", buff);

      source_given(fp, name);

      while( (name = find_next_this_type(indi, name)) )
      {
        fprintf(fp, "<br>- ");
        if((title = find_type(name, TITL)))
          fprintf(fp, "<b>%s </b>", title->data);

        fprintf(fp, "<b>%s</b> \n", name->data);
        source_given(fp, name);
      }

      if( (birt = find_type(indi, BIRT)) )
      {
        fprintf(fp, "<dd><b>b.</b>");
        if( (date = find_type(birt, DATE)) )
          fprintf(fp, " %s", date->data);
        if( (plac = find_type(birt, PLAC)) )
          fprintf(fp, " <b>at</b> %s", plac->data);
        source_given(fp, birt);
        dump_notes(fp, birt,1 );
      }
      if( (chr = find_type(indi, CHR)) )
      {
        fprintf(fp, "<dd><b>c.</b>");
        if( (date = find_type(chr, DATE)) )
          fprintf(fp, " %s", date->data);
        if( (plac = find_type(chr, PLAC)) )
          fprintf(fp, " <b>at</b> %s", plac->data);
        source_given(fp, chr);
        dump_notes(fp, chr, 1 );
      }
      if( (deat = find_type(indi, DEAT)) )
      {
        fprintf(fp, "<dd><b>d.</b>");
        if( (date = find_type(deat, DATE)) )
          fprintf(fp, " %s", date->data);
        if( (plac = find_type(deat, PLAC)) )
          fprintf(fp, " <b>at</b> %s", plac->data);
        source_given(fp, deat);
        dump_notes(fp, deat, 1);
      }

      if( (buri = find_type(indi, BURI)) )
      {
        fprintf(fp, "<dd><b>Buried.</b>");
        if( (date = find_type(buri, DATE)) )
          fprintf(fp, " %s", date->data);
        if( (plac = find_type(buri, PLAC)) )
          fprintf(fp, " <b>at</b> %s", plac->data);
        source_given(fp, buri);
        dump_notes(fp, buri, 1);
      }

      if( (crem = find_type(indi, CREM)) )
      {
        fprintf(fp, "<dd><b>Cremated.</b>");
        if( (date = find_type(crem, DATE)) )
          fprintf(fp, " %s", date->data);
        if( (plac = find_type(crem, PLAC)) )
          fprintf(fp, " <b>at</b> %s", plac->data);
        source_given(fp, crem);
        dump_notes(fp, crem, 1);
      }

    }
    if(fams)
    {
      mcount = 1;
      strip_ats(buff, fams->data);
      strcat(buff, ".html");
      fprintf(fp, "<dd><a href=\"/ruby/gedrelay.rbx?type=html&target=%s\" ><b>M.%d.</b></a>\n",  buff, mcount++ );
      while( (fams = find_next_this_type(indi, fams)) )
      {
        strip_ats(buff, fams->data);
        strcat(buff, ".html");
        fprintf(fp, "<a href=\"/ruby/gedrelay.rbx?type=html&target=%s\" ><b>, M.%d.</b></a>\n",  buff, mcount++);
      }
      fprintf(fp,"</dd>\n");
    }

    if(!privacy && (note = find_type(indi, OCCU)) )
    {
      fprintf(fp, "<dd><b>Occupation</b> %s\n", note->data);
      for(g = note->next; g && g != &all && g->level > note->level; g = g->next)
        fprintf(fp, "%s\n", g->data);
      source_given(fp, note);
      while( (note = find_next_this_type(indi, note)) )
      {
        fprintf(fp, "<dd><b>Occupation</b> %s\n", note->data);
        for(g = note->next; g && g != &all && g->level > note->level; g = g->next)
          fprintf(fp, "%s\n", g->data);
        source_given(fp, note);
      }
    }
    if(!privacy && (note = find_type(indi, EDUC)) )
    {
      fprintf(fp, "<dd><b>Education</b> %s\n", note->data);
      for(g = note->next; g && g != &all && g->level > note->level; g = g->next)
        fprintf(fp, "%s\n", g->data);
      source_given(fp, note);
      while( (note = find_next_this_type(indi, note)) )
      {
        fprintf(fp, "<dd><b>Education</b> %s\n", note->data);
        for(g = note->next; g && g != &all && g->level > note->level; g = g->next)
          fprintf(fp, "%s\n", g->data);
        source_given(fp, note);
      }
    }
    if(!privacy && (will = find_type(indi, WILL)) )
    {
      fprintf(fp, "<dd><b>Will</b>\n");
      dump_notes(fp, will,1 );
    }
    fprintf(fp, "<br>");
    source_given(fp, indi);
    dump_notes(fp, indi, 1);
    fprintf(fp, "</dl>\n");
  }
}


char *extract_filename(char *data)
{
static char filename[256], *p;

  if(data == 0)
    return "";
  //Skip the @#A=
  while(*data && isspace(*data))
    data++;
  if(*data == '\0' || strncmp(data, "@#A=", 4) != 0)
    return "";
  else
    data += 4;
  //Copy the filename until the end of the buffer or a space
  p = filename;
  while(*data && !isspace(*data))
    *p++ = *data++;
  *p++ = '\0';
  return filename;
}

void html_dump(void)
{
ged_type *g;
ged_type *h;
ged_type *marr;
ged_type *pict;
ged_type *divo;
ged_type *note;
ged_type *date;
ged_type *plac;
ged_type *husb, *husb_rec;
ged_type *wife, *wife_rec;
ged_type *child, *child_rec;
FILE *fp;
char buff[64];
int dirname_l;
int j;

  sprintf(buff, ":%s:",  HTML_DIR);
  dirname_l = strlen(buff);

  for(g = head_FAM.next_this_type; g != &head_FAM; g=g->next_this_type) //for each FAM record
  {
    strip_ats(&buff[dirname_l], g->data);
    strcat(buff, ".html");
    if( (fp = fopen(buff, "w")) )
    {
      fprintf(fp,"<html>\n<head><Title>%s</Title>\n<META NAME=\"ROBOTS\" CONTENT=\"INDEX, NOFOLLOW\">\n</head>\n<body>\n", buff);
      for(j = 0; j < 26; j++)
        fprintf(fp, "<A HREF=\"/ruby/gedrelay.rbx?type=index&target=index_%c\">%c</A> \n",  j+'A', j+'A');
      fprintf(fp, "<A HREF=\"/ruby/gedrelay.rbx?type=surname&target=index_A\">Surname Only Index</A>\n");

      fprintf(fp, "<h2>Marriage</h2>\n");
      if( (husb = find_type(g, HUSB)) )
        if( (husb_rec = find_hash(husb->data)) )
          html_dump_indi(fp, husb_rec, 1);
      if( (wife = find_type(g, WIFE)) )
        if( (wife_rec = find_hash(wife->data)) )
          html_dump_indi(fp, wife_rec, 1);
      if( (marr = find_type(g, MARR)) )
      {
        fprintf(fp, "<dl><dt><b>Marriage.</b>");
        if( (date = find_type(marr, DATE)) )
          fprintf(fp, " %s", date->data);
        if( (plac = find_type(marr, PLAC)) )
          fprintf(fp, " <b>at</b> %s", plac->data);
        fprintf(fp, "<br>\n");
        dump_notes(fp, marr, 1);
        fprintf(fp, "<br></dl>\n");
      }
      if( (divo = find_type(g, DIVO)) )
      {
        fprintf(fp, "<dl><dt><b>Divorce.</b>");
        if( (date = find_type(divo, DATE)) )
          fprintf(fp, " %s", date->data);
        if( (plac = find_type(divo, PLAC)) )
          fprintf(fp, " <b>at</b> %s", plac->data);
        fprintf(fp, "<br>\n");
        dump_notes(fp, divo, 1);
        fprintf(fp, "<br></dl>\n");
      }
      dump_notes(fp, g, 0);
      if( (pict = find_type(g, PICT)) )
      {
        fprintf(fp, "<IMG SRC=\"http://www.burrowes.org/FamilyTree/%s\" ALIGN=TOP><br>\n", extract_filename(pict->data));
      }
      fprintf(fp, "<h2>Children</h2>\n");
      if( (child = find_type(g, CHIL)) )
      {
        fprintf(fp, "<ul>\n");
        if( (child_rec = find_hash(child->data)) )
          html_dump_indi(fp, child_rec, 0);
        while( (child = find_next_this_type(g, child)) )
          if( (child_rec = find_hash(child->data)) )
            html_dump_indi(fp, child_rec, 0);
        fprintf(fp, "</ul>\n");
      }
      fprintf(fp,"<b>Go to the Tree's </b><A HREF=\"http://www.burrowes.org/FamilyTree/\" ><b>Entry Page</b></A>.<p>\n");
      fprintf(fp,"<hr>Maintained by <A HREF=\"mailto:rob@cs.auckland.ac.nz\">Rob Burrowes</A>.<br>\n");
      fprintf(fp,"Rob's <A HREF=\"http://www.burrowes.org/~rob/\" >Home Page</A>\n");
      fprintf(fp,"</body></html>\n");
      fclose(fp);
    }
    else
      printf("Error: unable to create %s\n", buff);
  }

}

void Indi_html_dump(FILE *fp, ged_type *g)
{
char buff[64];
int j;


  strip_ats(buff, g->data);
  fprintf(fp, "<html>\n<head><Title>%s</Title>\n<NAME=\"FamWindow\">\n<META NAME=\"ROBOTS\" CONTENT=\"INDEX, NOFOLLOW\">\n</head>\n<body>\n", buff);

  for(j = 0; j < 26; j++)
    fprintf(fp, "<A HREF=\"/ruby/gedrelay.rbx?type=surname&target=index_%c\"  >%c</A> \n", j+'A', j+'A');
  fprintf(fp, "<A HREF=\"/ruby/gedrelay.rbx?type=surname&target=index_%c\"  >%c</A><br>\n", '?', '?');
  //fprintf(fp, "<A HREF=\"/ruby/gedrelay.rbx?type=index&target=index_A\" >Full Index</A>\n");

  for(j = 0; j < 26; j++)
    fprintf(fp, "<A HREF=\"/ruby/gedrelay.rbx?type=surnamef&target=index_%c\" >%c</A> \n", j+'A', j+'A');
  fprintf(fp, "<A HREF=\"/ruby/gedrelay.rbx?type=surnamef&target=index_?\" >?</A> <b>No Surname INDEX</b><p>\n");

  html_dump_indi(fp, g, 1);
  source_given(fp, g);

  fprintf(fp,"<b>Go to the Tree's </b><A HREF=\"http://www.burrowes.org/FamilyTree/\"  ><b>Entry Page</b></A>.<br>\n");
  fprintf(fp,"Dump raw <A HREF=\"/ruby/gedrelay.rbx?type=ged&target=%s\" >GEDCOM</a><p>\n", buff);
  fprintf(fp,"<hr>Maintained by <A HREF=\"mailto:rob@cs.auckland.ac.nz\">Rob Burrowes</A>.<br>\n");
  fprintf(fp,"Rob's <A HREF=\"http://www.burrowes.org/~rob/\"  >Home Page</A>\n");
  fprintf(fp,"</body></html>\n");
}

void Family_html_dump(FILE *fp, ged_type *g)
{
ged_type *h;
ged_type *marr;
ged_type *pict;
ged_type *divo;
ged_type *note;
ged_type *date;
ged_type *plac;
ged_type *husb, *husb_rec;
ged_type *wife, *wife_rec;
ged_type *child, *child_rec;
char buff[64];
int j;


  strip_ats(buff, g->data);
  fprintf(fp, "<html>\n<head><Title>%s</Title>\n<NAME=\"FamWindow\">\n<META NAME=\"ROBOTS\" CONTENT=\"INDEX, NOFOLLOW\">\n</head>\n<body>\n", buff);

  for(j = 0; j < 26; j++)
    fprintf(fp, "<A HREF=\"/ruby/gedrelay.rbx?type=surname&target=index_%c\"  >%c</A> \n", j+'A', j+'A');
  fprintf(fp, "<A HREF=\"/ruby/gedrelay.rbx?type=surname&target=index_%c\"  >%c</A><br>\n", '?', '?');
  //fprintf(fp, "<A HREF=\"/ruby/gedrelay.rbx?type=index&target=index_A\" >Full Index</A>\n");

  for(j = 0; j < 26; j++)
    fprintf(fp, "<A HREF=\"/ruby/gedrelay.rbx?type=surnamef&target=index_%c\" >%c</A> \n", j+'A', j+'A');
  fprintf(fp, "<A HREF=\"/ruby/gedrelay.rbx?type=surnamef&target=index_?\" >?</A> <b>No Surname INDEX</b><p>\n");

  fprintf(fp, "<h2>Marriage</h2>\n");
  if( (husb = find_type(g, HUSB)) )
  {
    if( (husb_rec = find_hash(husb->data)) )
      html_dump_indi(fp, husb_rec, 1);
    source_given(fp, husb);
  }
  if( (wife = find_type(g, WIFE)) )
  {
    if( (wife_rec = find_hash(wife->data)) )
      html_dump_indi(fp, wife_rec, 1);
    source_given(fp, wife);
  }
  if( (marr = find_type(g, MARR)) )
  {
    fprintf(fp, "<dl><dt><b>Marriage.</b>");
    if( (date = find_type(marr, DATE)) )
    {
      fprintf(fp, " %s", date->data);
      source_given(fp, date);
    }
    if( (plac = find_type(marr, PLAC)) )
    {
      fprintf(fp, " <b>at</b> %s", plac->data);
      source_given(fp, plac);
    }
    fprintf(fp, "<br>\n");
    source_given(fp, marr);
    dump_notes(fp, marr,1);
    fprintf(fp, "<br></dl>\n");
  }
  if( (divo = find_type(g, DIVO)) )
  {
    fprintf(fp, "<dl><dt><b>Divorce.</b>");
    if( (date = find_type(divo, DATE)) )
    {
      fprintf(fp, " %s", date->data);
      source_given(fp, date);
    }
    if( (plac = find_type(divo, PLAC)) )
    {
      fprintf(fp, " <b>at</b> %s", plac->data);
      source_given(fp, plac);
    }
    fprintf(fp, "<br>\n");
    source_given(fp, divo);
    dump_notes(fp, divo, 1);
    fprintf(fp, "<br></dl>\n");
  }
  source_given(fp, g);
  dump_notes(fp, g, 0);
  if( (pict = find_type(g, PICT)) )
  {
    fprintf(fp, "<IMG SRC=\"http://www.burrowes.org/FamilyTree/%s\" ALIGN=TOP><br>\n", extract_filename(pict->data));
  }
  fprintf(fp, "<h2>Children</h2>\n");
  if( (child = find_type(g, CHIL)) )
  {
    fprintf(fp, "<ul>\n");
    if( (child_rec = find_hash(child->data)) )
    {
      html_dump_indi(fp, child_rec, 0);
      source_given(fp, child);
    }
    while( (child = find_next_this_type(g, child)) )
      if( (child_rec = find_hash(child->data)) )
      {
        html_dump_indi(fp, child_rec, 0);
        source_given(fp, child);
      }
    fprintf(fp, "</ul>\n");
  }
  fprintf(fp,"<b>Go to the Tree's </b><A HREF=\"http://www.burrowes.org/FamilyTree/\"  ><b>Entry Page</b></A>.<br>\n");
  fprintf(fp,"Dump raw <A HREF=\"/ruby/gedrelay.rbx?type=ged&target=%s\" >GEDCOM</a><p>\n", buff);
  fprintf(fp,"<hr>Maintained by <A HREF=\"mailto:rob@cs.auckland.ac.nz\">Rob Burrowes</A>.<br>\n");
  fprintf(fp,"Rob's <A HREF=\"http://www.burrowes.org/~rob/\"  >Home Page</A>\n");
  fprintf(fp,"</body></html>\n");

}

void dump_gedcom()
{
ged_type *g;
ged_type *name;
FILE *fp, *fp2;
int i;

//#define FULL_DUMP
#define DUPLICATES


#ifdef FULL_DUMP
  if((fp = fopen("cspo", "w+")) == NULL)
  {
    printf("Can't open cspo file\n");
    return;
  }
#endif

#ifdef DUPLICATES
  check_for_duplicates();
#endif

  check_for_in_fam();
  check_links();
  display_stats();
  check_dates();
  html_dump();
  create_index();

  i = 0;
  while(Ancestor_Tree[i].name)
  {
    if((fp2 = fopen(Ancestor_Tree[i].name, "w+")) == NULL)
      printf("Can't open %s file\n", Ancestor_Tree[i].name);
    else
    {
      if( (g = find_hash(Ancestor_Tree[i].base)) )
      {
        if( (name = find_type(g, NAME)) )
          printf("Charting pedigree %s\n", name->data);
        output_pedigree(fp2, g, 0);
      }
      fclose(fp2);
    }
    i++;
  }

  i = 0;
  while(Descendants_Tree[i].name)
  {
    if((fp2 = fopen(Descendants_Tree[i].name, "w+")) == NULL)
      printf("Can't open %s file\n", Descendants_Tree[i].name);
    else
    {
      if( (g = find_hash(Descendants_Tree[i].base)) )
      {
        if( (name = find_type(g, NAME)) )
          printf("Charting Decendants %s\n", name->data);
        output_decendants(fp2, g, 0);
      }
      fclose(fp2);
    }
    i++;
  }

  i = 0;
  while(Descendants_Single_Tree[i].name)
  {
    if((fp2 = fopen(Descendants_Single_Tree[i].name, "w+")) == NULL)
      printf("Can't open %s file\n", Descendants_Single_Tree[i].name);
    else
    {
      if( (g = find_hash(Descendants_Single_Tree[i].base)) )
      {
        if( (name = find_type(g, NAME)) )
          printf("Charting Decendants %s (his surname only)\n", name->data);
        output_decendants_of_name(fp2, g, 0);
      }
      fclose(fp2);
    }
    i++;
  }

#ifdef FULL_DUMP
  for(g = all.next; g != &all; g=g->next)
  {
    if(g->level == 0)
      fprintf(fp, "0 %s %s\n", g->data, g->type);
    else
    {
      //for(i = 0; i < g->level; i++)
      //  putchar(' ');
      fprintf(fp, "%d %s %s\n", g->level, g->type, g->data);
    }
  }
  fprintf(fp, "%d %s\n", 0, "TRLR");

  fclose(fp);
#endif


}


time_t file_time = 0;

time_t get_modtime(int fd)
{
struct stat stat_buf;

  if(fstat(fd, &stat_buf) != -1)
    return stat_buf.st_mtime;
  else
    return time(0); //Now is probably the best time to give.
}

int filesize(int fd)
{
int length = lseek(fd, 0, SEEK_END);
  lseek(fd, 0, SEEK_SET);
  return length;
}

static char *Data_buffer;

void load_gedcom(void)
{
int fd;
int file_size = 0;
int j = 0;
int i;
int num_files = 0;
time_t mod_time;

  init_gedlists();
//  chdir(PATH);

  i = 0;
  while( Ged_File[i].name ) /*work out how much memory we need*/
  {
    if((fd = open(Ged_File[i].name, O_RDONLY)) != -1)
    {
      Ged_File[i].length = filesize(fd);
      file_size += Ged_File[i].length;

      //while we are at it, set the mod time to the latest mod time for this file.
      if((mod_time = get_modtime(fd)) > file_time)
        file_time = mod_time;

      close(fd);

    }
    else
    {
      printf("Unable to open file %s\n",Ged_File[i].name);
      return;
    }
    i++;
  }
  Data_buffer = malloc(file_size + 1); //Alloc the memory

  i = 0;
  while( Ged_File[i].name ) //read in the file
  {
    if((fd = open(Ged_File[i].name, O_RDONLY)) != -1)
    {
      read(fd, &Data_buffer[j], Ged_File[i].length);
      j += Ged_File[i].length;
      close(fd);
      num_files++;
    }
    else
      printf("Unable to open file %s: Skipping\n",Ged_File[i].name);
    i++;
  }
  Data_buffer[file_size] = '\0';

  if(j) //If we read anything; process it.
  {
    parse_gedcom(Data_buffer, num_files);
    //dump_gedcom();
  }
}

void reload(void)
{
ged_type *g, *n;

  for(g = all.next; g != &all; g=n)
  {
    n = g->next;
    free(g);
  }
  free(Data_buffer);

  load_gedcom();
  create_index();
}
