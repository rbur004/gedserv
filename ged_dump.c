#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define GED_DUMP_C

#include "ged.h"
#include "croll.h"
#include "ged_dump.h"
#include "stringfunc.h"
#include "date.h"

void Family_GED_dump(FILE *fp, ged_type *g) {
  int i, j;
  int current_level;
  ged_type *indi;
  ged_type *name;
  ged_type *title;
  ged_type *resn;
  char buff[REF_LENGTH];
  char buff2[REF_LENGTH];
  int type;
  int privacy = 0;
  time_t t = time(0);                       // Current time
  int year = 1900 + localtime(&t)->tm_year; // Current year

  strip_ats(buff2, g->data);
  fprintf(fp,
          "<HTML>\n<HEAD><TITLE>%s</TITLE>\n<NAME=\"FamWindow\">\n<META "
          "NAME=\"ROBOTS\" CONTENT=\"noindex, nofollow, "
          "noarchive\">\n</HTML>\n<BODY>\n",
          buff2);

  for (j = 0; j < 26; j++)
    fprintf(fp,
            "<A HREF=\"/ruby/gedrelay.rbx?type=surname&target=index_%c\" "
            ">%c</A> \n",
            j + 'A', j + 'A');
  fprintf(fp,
          "<A HREF=\"/ruby/gedrelay.rbx?type=surname&target=index_%c\"  "
          ">%c</A><br>\n",
          '?', '?');
  // fprintf(fp, "<A HREF=\"/ruby/gedrelay.rbx?type=index&target=index_A\" >Full
  // Index</A>\n");

  for (j = 0; j < 26; j++)
    fprintf(fp,
            "<A HREF=\"/ruby/gedrelay.rbx?type=surnamef&target=index_%c\" "
            ">%c</A> \n",
            j + 'A', j + 'A');
  fprintf(fp, "<A HREF=\"/ruby/gedrelay.rbx?type=surnamef&target=index_?\" "
              ">?</A> <b>No Surname INDEX</b><p>\n");

  if (g == 0) {
    fprintf(fp, "Print_record passed a null pointer\n");
    return;
  }

  if ((current_level = g->level)) {
    for (i = 0; i < g->level; i++)
      fputc(' ', fp);
    fprintf(fp, "%d <b>%s</b> %s<br>\n", g->level, g->type, g->data);
  } else
    fprintf(fp, "0 %s <b>%s</b><br>\n", g->data, g->type);

  resn = find_type(g, RESN); // look for restriction notices
  for (g = g->next; g && g->level > current_level; g = g->next) {
    for (i = 0; i < g->level; i++)
      fputc(' ', fp);
    if (findstring(g->type, link_types, NLINK_TYPES) != -1 && *g->data == '@') {
      fprintf(fp,
              "%d <b>%s</b> <A HREF=\"/ruby/gedrelay.rbx?type=ged&target=%s\" "
              ">%s</A>\n",
              g->level, g->type, strip_ats(buff, g->data), g->data);
      switch (type_to_num(g->type)) {
      case HUSB:
      case WIFE:
      case CHIL:
        if ((indi = find_hash(g->data)) && (name = find_type(indi, NAME)) &&
            !((resn = find_type(indi, RESN)) &&
              strcmp(resn->data, PRIVACY) == 0)) {
          if ((title = find_type(name, TITL)))
            fprintf(fp, "<b>%s </b>", title->data);
          fprintf(fp, "%s", name->data);
        }
        break;
      default:
        break;
      }
      fprintf(fp, "<br>\n");
    } else if (!(resn && strcmp(resn->data, PRIVACY) == 0)) {
      int e_date = 0;
      if (type_to_num(g->type) == DATE) {
        if ((e_date = return_year(g->data)) && e_date != 0 &&
            (year - e_date) <= 100)
          fprintf(fp, "%d <b>%s</b> %4.4d<br>\n", g->level, g->type, e_date);
        else
          fprintf(fp, "%d <b>%s</b> %s<br>\n", g->level, g->type, g->data);
      } else
        fprintf(fp, "%d <b>%s</b> %s<br>\n", g->level, g->type, g->data);
    }
  }

  fprintf(fp, "<p><b>Go to the Tree's </b><A "
              "HREF=\"https://www.burrowes.org/FamilyTree/\"  ><b>Entry "
              "Page</b></A>.<br>\n");
  fprintf(fp,
          "<A HREF=\"/ruby/gedrelay.rbx?type=html&target=%s\" >Format</a><p>\n",
          buff2);
  fprintf(fp, "<hr>Maintained by <A HREF=\"mailto:rob@cs.auckland.ac.nz\">Rob "
              "Burrowes</A>.<br>\n");
  fprintf(fp,
          "Rob's <A HREF=\"https://www.burrowes.org/~rob/\"  >Home Page</A>\n");
  fprintf(fp, "</body></html>\n");
}
