#include "tabdyn.c"
#include <dirent.h>
#include <sys/wait.h>

char* traitement(char *path, char *dest);

char *getdest();

void lister_supp(char *path, char *dest, tabdyn *add);

char *buildfiledelete();

char *buildfiledest();

char *buildfileremplace();

char *getdestination();

void choose_dir(char *path);

void add_delete_in_dir(char *pathdest);

void deletefrompaths();