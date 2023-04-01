#include "dump_gedcom.h"

// Not called from anywhere.
void dump_gedcom(void)
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
      fprintf(fp,"<HTML>\n<HEAD><Title>%s</Title>\n<META NAME=\"ROBOTS\" CONTENT=\"INDEX, NOFOLLOW\">\n</HEAD>\n<BODY>\n", buff);
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
        fprintf(fp, "<IMG SRC=\"https://www.burrowes.org/FamilyTree/%s\" ALIGN=TOP><br>\n", extract_filename(pict->data));
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
      fprintf(fp,"<b>Go to the Tree's </b><A HREF=\"https://www.burrowes.org/FamilyTree/\" ><b>Entry Page</b></A>.<p>\n");
      fprintf(fp,"<hr>Maintained by <A HREF=\"mailto:rob@cs.auckland.ac.nz\">Rob Burrowes</A>.<br>\n");
      fprintf(fp,"Rob's <A HREF=\"https://www.burrowes.org/~rob/\" >Home Page</A>\n");
      fprintf(fp,"</body></html>\n");
      fclose(fp);
    }
    else
      printf("Error: unable to create %s\n", buff);
  }

}
