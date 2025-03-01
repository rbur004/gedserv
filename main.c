#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>

#include "socket.h"
#include "pathnames.h"
#include "croll.h"
#include "btree.h"

int main(int argc, char **argv) {
  int i;
  if (argc == 1)
    exit(0);

  openlog("Gedserv", LOG_PID, LOG_USER);

  if ((Ged_File = (t_gedfile *)calloc(argc, sizeof(t_gedfile))) == NULL) {
    syslog(LOG_ERR, "Ged_File calloc failed");
    exit(1);
  }

  for (i = 1; i < argc; i++) {
    Ged_File[i - 1].name = argv[i];
    Ged_File[i - 1].length = 0;
  }
  Ged_File[i - 1].name = 0;
  Ged_File[i - 1].length = 0;

  load_gedcom();
  create_index();
  display_stats();
  main_loop();

  closelog();
  return 0;
}
