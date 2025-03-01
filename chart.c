/*
 * Construction of pedigree charts
 */
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <time.h>

 #include "ged.h"
 #include "croll.h"
 #include "chart.h"
 #include "pathnames.h"
 #include "bit.h"
 #include "btree.h"
 #include "rel_index.h"
 #include "relationship.h"
 #include "date.h"

#define PEDIGREE_WIDTH 3
int pass = 0; // have we looped

static void print_lastbar(FILE *ofile, int depth);
static void print_lastspace(FILE *ofile);

void output_pedigree(FILE *ofile, ged_type *rt, int max_depth) {
  int *widths, i;
  ged_type *name;
  char *title;
  unsigned int glob_map[8];

  for (i = 0; i < 8; i++)
    glob_map[i] = 0;

  if ((name = find_type(rt, NAME)))
    title = name->data;
  else
    title = "unknown";

  // compute_pedigree_widths(rt, depth, widths);
  fprintf(ofile,
          "<HTML><HEAD><TITLE>%s Ances.</TITLE><NAME=\"IndexWindow\">\n<META "
          "NAME=\"ROBOTS\" CONTENT=\"noindex, nofollow, "
          "noarchive\">\n</HEAD>\n<BODY><HR>\n<PRE>\n",
          title);
  pass++;
  output_pedigree_info(ofile, rt, 0, 0, glob_map, 0, max_depth);
  output_pedigree_index(ofile, rt, max_depth);
  fprintf(ofile, "</pre><b>Go to the Tree's </b><A "
                 "HREF=\"https://www.burrowes.org/FamilyTree/\"  ><b>Entry "
                 "Page</b></A>.<p>\n");
  fprintf(ofile,
          "<hr>Maintained by <A HREF=\"mailto:rob@cs.auckland.ac.nz\">Rob "
          "Burrowes</A>.<br>\n");
  fprintf(ofile,
          "Rob's <A HREF=\"https://www.burrowes.org/~rob/\"  >Home Page</A>\n");
  fprintf(ofile, "</BODY></HTML>\n");
}

void output_decendants(FILE *ofile, ged_type *rt, int max_depth) {
  int *widths, i;
  ged_type *name;
  char *title;
  unsigned int glob_map[8];

  for (i = 0; i < 8; i++)
    glob_map[i] = 0;

  if ((name = find_type(rt, NAME)))
    title = name->data;
  else
    title = "unknown";

  // compute_pedigree_widths(rt, depth, widths);
  fprintf(ofile,
          "<HTML><HEAD><TITLE>%s Desc.</TITLE><NAME=\"IndexWindow\">\n<META "
          "NAME=\"ROBOTS\" CONTENT=\"noindex, nofollow, "
          "noarchive\"></HEAD>\n<BODY><HR>\n<PRE>\n",
          title);
  pass++;
  output_decendants_info(ofile, rt, 0, 0, glob_map, 0, max_depth);
  output_descendants_index(ofile, rt, max_depth);
  fprintf(ofile, "</pre><b>Go to the Tree's </b><A "
                 "HREF=\"https://www.burrowes.org/FamilyTree/\"  ><b>Entry "
                 "Page</b></A>.<p>\n");
  fprintf(ofile,
          "<hr>Maintained by <A HREF=\"mailto:rob@cs.auckland.ac.nz\">Rob "
          "Burrowes</A>.<br>\n");
  fprintf(ofile,
          "Rob's <A HREF=\"https://www.burrowes.org/~rob/\"  >Home Page</A>\n");
  fprintf(ofile, "</BODY></HTML>\n");
}

void output_decendants_of_name(FILE *ofile, ged_type *rt, int max_depth) {
  int *widths, i;
  ged_type *name;
  char *title;
  unsigned int glob_map[8];

  for (i = 0; i < 8; i++)
    glob_map[i] = 0;

  if ((name = find_type(rt, NAME)))
    title = name->data;
  else
    title = "unknown";

  // compute_pedigree_widths(rt, depth, widths);
  fprintf(
      ofile,
      "<HTML><HEAD><TITLE>%s Desc.(R.)</TITLE><NAME=\"IndexWindow\">\n<META "
      "NAME=\"ROBOTS\" CONTENT=\"noindex, nofollow, noarchive, "
      "noarchive\">\n</HEAD>\n<BODY><HR>\n<PRE>\n",
      title);
  pass++;
  output_decendants_info_of_name(ofile, rt, 0, 0, glob_map, max_depth);
  fprintf(ofile, "</pre><b>Go to the Tree's </b><A "
                 "HREF=\"https://www.burrowes.org/FamilyTree/\"  ><b>Entry "
                 "Page</b></A>.<p>\n");
  fprintf(ofile,
          "<hr>Maintained by <A HREF=\"mailto:rob@cs.auckland.ac.nz\">Rob "
          "Burrowes</A>.<br>\n");
  fprintf(ofile,
          "Rob's <A HREF=\"https://www.burrowes.org/~rob/\"  >Home Page</A>\n");
  fprintf(ofile, "</BODY></HTML>\n");
}

void output_pedigree_info(FILE *ofile, ged_type *rt, int curdepth,
                          int direction, unsigned int *glob_map,
                          char *child_marker, int max_depth) {
  ged_type *famc;
  ged_type *fams;
  ged_type *family;
  ged_type *husb, *wife;
  ged_type *husb_rec, *wife_rec;

  if (rt) {

    if ((famc = find_type(rt, FAMC)))
      family = find_hash(famc->data);
    else
      family = 0;
    fams = find_type(rt, FAMS);

    if (rt->processed ==
        pass) { // A loop has occured. Print the record with a * and return
      output_pedigree_name(ofile, rt, curdepth, famc, fams, glob_map, "*",
                           child_marker, 1, "", max_depth);
      if (direction)
        setbit(glob_map,
               curdepth); // IF you are male parent then set this level bit
      else
        clearbit(glob_map,
                 curdepth); // below female parent we don't want the bit set
      print_bars(ofile, glob_map, curdepth);
      if (isbitset(glob_map, curdepth))
        print_lastbar(ofile, curdepth);
      else
        print_lastspace(ofile);
      fputc('\n', ofile);
      clearbit(glob_map, curdepth); // below this level we don't want the bit
                                    // set
      return;
    }

    rt->processed = pass;

    if ((max_depth == 0 || max_depth >= curdepth) && family &&
        (husb = find_type(family, HUSB)) &&
        (husb_rec = find_hash(husb->data))) {
      output_pedigree_info(ofile, husb_rec, curdepth + 1, 1, glob_map, rt->data,
                           max_depth);
    }

    output_pedigree_name(ofile, rt, curdepth, famc, fams, glob_map, "",
                         child_marker, 1, "", max_depth);

    if (direction)
      setbit(glob_map,
             curdepth); // IF you are male parent then set this level bit
    else
      clearbit(glob_map,
               curdepth); // below female parent we don't want the bit set

    if ((max_depth == 0 || max_depth >= curdepth) && family &&
        (wife = find_type(family, WIFE)) &&
        (wife_rec = find_hash(wife->data))) {
      setbit(glob_map, curdepth + 1); // set the next level line drawing bit
      print_bars(ofile, glob_map, curdepth + 1);
      print_lastbar(ofile, curdepth + 1);
      fputc('\n', ofile);
      output_pedigree_info(ofile, wife_rec, curdepth + 1, 0, glob_map, rt->data,
                           max_depth);
    } else {
      print_bars(ofile, glob_map, curdepth);
      if (isbitset(glob_map, curdepth))
        print_lastbar(ofile, curdepth);
      else
        print_lastspace(ofile);
      fputc('\n', ofile);
    }

    clearbit(glob_map, curdepth); // below this level we don't want the bit set
                                  // Not That it will be printed anyway.
  }
}

int output_decendants_info(FILE *ofile, ged_type *rt, int curdepth,
                           int direction, unsigned int *glob_map, int rel_chart,
                           int max_depth) {
  ged_type *famc;
  ged_type *fams;
  ged_type *nfams;
  ged_type *child;
  ged_type *marr;
  ged_type *date;
  ged_type *plac;
  ged_type *child_rec;
  ged_type *family;
  ged_type *husb, *wife;
  ged_type *husb_rec, *wife_rec;
  ged_type *next_child_rec;
  int mcount = 0;
  char buffer[LINE_LIMIT];
  char buff2[LINE_LIMIT];

  if (rt) {
    if ((fams = find_type(rt, FAMS))) // look for a spouse and family
      family = find_hash(fams->data);
    else
      family = 0;

    famc = find_type(rt, FAMC);

    if (rt->processed ==
        pass) { // A loop has occured. Print the record with a * and return
      // Or we are generating a relationship tree, and this is one of the two
      // people involved
      if (rel_chart == 0) {
        output_pedigree_name(ofile, rt, curdepth, famc, fams, glob_map, "*", 0,
                             0, "", max_depth);
        if (curdepth > 0 && direction == 0) // No more children or second spouse
          clearbit(glob_map, curdepth);
        return 1;
      } else // is a relationship chart, so we stop at this depth.
        max_depth = curdepth;
    }

    rt->processed = pass;

    if (max_depth == 0)
      output_pedigree_name(ofile, rt, curdepth, famc, fams, glob_map,
                           curdepth ? "-" : "", 0, 0, "", max_depth);
    else if (max_depth >= curdepth)
      if (output_pedigree_name(ofile, rt, curdepth, famc, fams, glob_map,
                               curdepth ? "-" : "", 0, 0, "", max_depth) == 1)
        return 1;

    while (family) {
      int ccount;

      ccount = 0;
      mcount++;

      sprintf(buffer, "<b>m.</b>");
      if ((marr = find_type(family, MARR))) {
        if ((date = find_type(marr, DATE)))
          sprintf(&buffer[strlen(buffer)], " %s ", date->data);
        if ((plac = find_type(marr, PLAC)))
          sprintf(buff2, "<b>,m. at</b> %s ", plac->data);
        else
          buff2[0] = '\0';
      } else
        buff2[0] = '\0';

      if ((husb = find_type(family, HUSB)) &&
          (husb_rec = find_hash(husb->data)) && husb_rec != rt) {
        famc = find_type(rt, FAMC);
        output_pedigree_name(ofile, husb_rec, curdepth, famc, fams, glob_map,
                             buffer, 0, 0, buff2, max_depth);
      } else if ((wife = find_type(family, WIFE)) &&
                 (wife_rec = find_hash(wife->data)) && wife_rec != rt) {
        famc = find_type(rt, FAMC);
        output_pedigree_name(ofile, wife_rec, curdepth, famc, fams, glob_map,
                             buffer, 0, 0, buff2, max_depth);
      }

      if (curdepth > 0 && direction == 0) // No more children or second spouse
        clearbit(glob_map, curdepth);

      fams = find_next_this_type(rt, fams);
      child = find_type(family, CHIL);

      if (fams)
        setbit(glob_map, curdepth);
      else if (direction == 0)
        clearbit(glob_map, curdepth);

      if ((max_depth == 0 || max_depth >= curdepth) && child) {
        ged_type *child_tmp = child;
        do {
          child_rec = find_hash(child_tmp->data);
          child_tmp = find_next_this_type(family, child_tmp);
          if (child_rec &&
              (rel_chart == 0 || in_rel_list(rt->rel_child_1, child_rec) ||
               in_rel_list(rt->rel_child_2, child_rec))) {
            ccount++;
          }
        } while (child_tmp);
        if (ccount) {
          if (max_depth == curdepth && max_depth != 0) {
            setbit(glob_map, curdepth + 1);
            print_bars(ofile, glob_map, curdepth + 1);
            print_lastbar(ofile, curdepth + 1);
            fputc('\n', ofile);
            print_bars(ofile, glob_map, curdepth + 1);
            print_lastbar(ofile, curdepth + 1);
            if (ccount == 1)
              fprintf(ofile, "- 1 Child\n");
            else
              fprintf(ofile, "- %d Children\n", ccount);
          } else {
            setbit(glob_map, curdepth + 1);
            print_bars(ofile, glob_map, curdepth + 1);
            print_lastbar(ofile, curdepth + 1);
            fputc('\n', ofile);
            next_child_rec = find_hash(child->data);
            do {
              child_rec = next_child_rec;
              if ((child = find_next_this_type(family, child)))
                next_child_rec = find_hash(child->data);
              if (child_rec &&
                  (rel_chart == 0 || in_rel_list(rt->rel_child_1, child_rec) ||
                   in_rel_list(rt->rel_child_2, child_rec)))
                output_decendants_info(ofile, child_rec, curdepth + 1,
                                       (next_child_rec && --ccount) ? 1 : 0,
                                       glob_map, rel_chart, max_depth);
            } while (child);
          }
          if (direction) {
            print_bars(ofile, glob_map, curdepth + 1);
            fputc('\n', ofile);
          }
        }
      } else if (family && fams == 0) {
        print_bars(ofile, glob_map, curdepth + 1);
        fputc('\n', ofile);
      }
      if (fams) {
        print_bars(ofile, glob_map, curdepth + 1);
        fputc('\n', ofile);
        family = find_hash(fams->data);
        if (curdepth == 0 && family == 0)
          clearbit(glob_map, curdepth);
      } else
        family = 0;
    }
  }
  return 0;
}

int output_decendants_info_of_name(FILE *ofile, ged_type *rt, int curdepth,
                                   int direction, unsigned int *glob_map,
                                   int max_depth) {
  ged_type *famc;
  ged_type *fams;
  ged_type *nfams;
  ged_type *child;
  ged_type *marr;
  ged_type *date;
  ged_type *plac;
  ged_type *child_rec;
  ged_type *family;
  ged_type *husb, *wife;
  ged_type *husb_rec, *wife_rec;
  ged_type *sex;
  int mcount = 0;
  char buffer[LINE_LIMIT];
  char buff2[LINE_LIMIT];

  if (rt) {
    if ((fams = find_type(rt, FAMS))) // look for a spouse and family
      family = find_hash(fams->data);
    else
      family = 0;

    famc = find_type(rt, FAMC);

    if (rt->processed ==
        pass) { // A loop has occured. Print the record with a * and return
      output_pedigree_name(ofile, rt, curdepth, famc, fams, glob_map, "*", 0, 0,
                           "", max_depth);
      if (curdepth > 0 && direction == 0) // No more children or second spouse
        clearbit(glob_map, curdepth);
      return 1;
    }

    rt->processed = pass;

    if (max_depth == 0)
      output_pedigree_name(ofile, rt, curdepth, famc, fams, glob_map,
                           curdepth ? "-" : "", 0, 0, "", max_depth);
    else if (max_depth >= curdepth)
      if (output_pedigree_name(ofile, rt, curdepth, famc, fams, glob_map,
                               curdepth ? "-" : "", 0, 0, "", max_depth) == 1)
        return 1;

    while (family) {
      mcount++;

      sprintf(buffer, "<b>m.</b>");
      if ((marr = find_type(family, MARR))) {
        if ((date = find_type(marr, DATE)))
          sprintf(&buffer[strlen(buffer)], " %s ", date->data);
        if ((plac = find_type(marr, PLAC)))
          sprintf(buff2, "<b>,m. at</b> %s ", plac->data);
        else
          buff2[0] = '\0';
      } else
        buff2[0] = '\0';

      if ((husb = find_type(family, HUSB)) &&
          (husb_rec = find_hash(husb->data)) && husb_rec != rt) {
        famc = find_type(rt, FAMC);
        output_pedigree_name(ofile, husb_rec, curdepth, famc, fams, glob_map,
                             buffer, 0, 0, buff2, max_depth);
      } else if ((wife = find_type(family, WIFE)) &&
                 (wife_rec = find_hash(wife->data)) && wife_rec != rt) {
        famc = find_type(rt, FAMC);
        output_pedigree_name(ofile, wife_rec, curdepth, famc, fams, glob_map,
                             buffer, 0, 0, buff2, max_depth);
      }

      if (curdepth > 0 && direction == 0) // No more children or second spouse
        clearbit(glob_map, curdepth);

      fams = find_next_this_type(rt, fams);
      child = find_type(family, CHIL);

      if (fams)
        setbit(glob_map, curdepth);

      if ((max_depth == 0 || max_depth >= curdepth) &&
          ((sex = find_type(rt, SEX)) && *sex->data == 'M') && child) {
        if (max_depth == curdepth && max_depth != 0) {
          setbit(glob_map, curdepth + 1);
          print_bars(ofile, glob_map, curdepth + 1);
          print_lastbar(ofile, curdepth + 1);
          fputc('\n', ofile);
          print_bars(ofile, glob_map, curdepth + 1);
          fputc('\n', ofile);
          fprintf(ofile, "- <b>...</b>\n");
        } else {
          setbit(glob_map, curdepth + 1);
          print_bars(ofile, glob_map, curdepth + 1);
          print_lastbar(ofile, curdepth + 1);
          fputc('\n', ofile);
          do {
            child_rec = find_hash(child->data);
            child = find_next_this_type(family, child);
            if (child_rec)
              if (output_decendants_info_of_name(ofile, child_rec, curdepth + 1,
                                                 child ? 1 : 0, glob_map,
                                                 max_depth) == 1 &&
                  max_depth != 0)
                break;
          } while (child);
        }
        print_bars(ofile, glob_map, curdepth + 1);
        fputc('\n', ofile);
        clearbit(glob_map, curdepth + 1);
      }
      if (fams) {
        print_bars(ofile, glob_map, curdepth + 1);
        family = find_hash(fams->data);
        if (curdepth == 0 && family == 0)
          clearbit(glob_map, curdepth);
      } else
        family = 0;
    }
  }
  return 0;
}

int output_pedigree_name(FILE *ofile, ged_type *indiv, int depth,
                         ged_type *famc, ged_type *fams, unsigned int *map,
                         char *prefix, char *child_marker, int blank,
                         char *suffix, int max_depth) {
  char *np;
  int space = 0;
  ged_type *name;
  ged_type *birt;
  ged_type *chr;
  ged_type *date;
  ged_type *deat;
  char *current_url;
  char buff[REF_LENGTH];
  char buff2[REF_LENGTH];
  ged_type *title;
  char *title_p;
  ged_type *resn;

  // If we are restricting the number of generations output
  // And we have passed the number of generations to output
  // print ... and return
  if (max_depth > 0 && max_depth < depth) {
    print_bars(ofile, map, depth);
    if (depth) {
      if (blank)
        print_lastspace(ofile);
      else
        print_lastbar(ofile, depth);
    }

    fprintf(ofile, "<b>...</b>\n");

    return 1;
  }

  if (indiv) {
    int privacy = 0;
    // if(blank)
    //	clearbit(map, depth );
    print_bars(ofile, map, depth);
    if (depth) {
      if (blank)
        print_lastspace(ofile);
      else
        print_lastbar(ofile, depth);
    }

    if ((resn = find_type(indiv, RESN)) && strcmp(resn->data, PRIVACY) == 0) {
      privacy = 1;
      title_p = "";
      np = "Name Withheld at the persons request";
    } else if ((name = find_type(indiv, NAME))) {
      np = name->data;
      if ((title = find_type(name, TITL)))
        title_p = title->data;
      else
        title_p = "";
    } else {
      np = "";
      title_p = "";
    }

    fprintf(ofile, "<A NAME=\"%s\"></A>",
            indiv->data); // Put a marker we can jump to

    if (!privacy) {
      if (fams)
        fprintf(ofile,
                "%s<b>%s</b>%s<A "
                "HREF=\"/ruby/gedrelay.rbx?type=html&target=%s\" >%s</A>",
                prefix, title_p, (*title_p ? " " : ""),
                strip_ats(buff, fams->data), np);
      else if (famc)
        fprintf(ofile,
                "%s<b>%s</b>%s<A "
                "HREF=\"/ruby/gedrelay.rbx?type=html&target=%s#%s\" >%s</A>",
                prefix, title_p, (*title_p ? " " : ""),
                strip_ats(buff, famc->data), strip_ats(buff2, indiv->data), np);
      else
        fprintf(ofile, "%s<b>%s</b>%s%s", prefix, title_p,
                (*title_p ? " " : ""), np);

      if ((birt = find_type(indiv, BIRT)) && (date = find_type(birt, DATE))) {
        int b_date = 0;
        if ((b_date = return_year(date->data)))
          fprintf(ofile, " <b>b.</b>%4.4d", b_date);
        else
          fprintf(ofile, " <b>b.</b>%s", date->data);
      } else if ((chr = find_type(indiv, CHR)) &&
                 (date = find_type(chr, DATE))) {
        int c_date = 0;
        if ((c_date = return_year(date->data)))
          fprintf(ofile, " <b>c.</b>%4.4d", c_date);
        else
          fprintf(ofile, " <b>c.</b>%s", date->data);
      }

      if ((deat = find_type(indiv, DEAT)) && (date = find_type(deat, DATE))) {
        int d_date = 0;
        if ((d_date = return_year(date->data)))
          fprintf(ofile, " <b>d.</b>%4.4d", d_date);
        else
          fprintf(ofile, " <b>d.</b>%s", date->data);
      }
    } else {
      fprintf(ofile, "%sName Withheld at the persons request", prefix);
    }
    fprintf(ofile, " %s\n", suffix);
  }
  return 0;
}

char *colour_chart[] = {
    "#000000", // black
    "#FF0522", // Red
    "#F58309", // Orange
    "#FBFF02", // Yellow
    "#09C710", // Green
    "#02EFF3", // Light Blue
    "#FF0CEB", // Inigo
    "#052EFF", // RBlue
};

#define NCOLORCHART_ITEMS 8

void print_bars(FILE *ofile, unsigned int *map, int depth) {
  int i;

  fprintf(ofile, "%s", isbitset(map, 0) ? "|" : " ");

  for (i = 1; i <= depth - 1; i++) {
    if (isbitset(map, i))
      fprintf(ofile, "<FONT COLOR=\"%s\">%*s</FONT>",
              colour_chart[i % NCOLORCHART_ITEMS], PEDIGREE_WIDTH, "|");
    else
      fprintf(ofile, "<FONT COLOR=\"%s\">%*s</FONT>",
              colour_chart[i % NCOLORCHART_ITEMS], PEDIGREE_WIDTH, " ");
  }
}

static void print_lastbar(FILE *ofile, int depth) {
  fprintf(ofile, "<FONT COLOR=\"%s\">%*s</FONT>",
          colour_chart[depth % NCOLORCHART_ITEMS], PEDIGREE_WIDTH, "|");
}

static void print_lastspace(FILE *ofile) {
  fprintf(ofile, "%*s", PEDIGREE_WIDTH - 1, "");
}
