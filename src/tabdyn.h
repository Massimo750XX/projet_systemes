#include "paths_manager.c"

typedef struct tabdyn
{
    int size;
    int capacity;
    char **tab;
}tabdyn;

void erreur(char *msg);

tabdyn create_table();

void push(tabdyn *t, char *path);

void deletetable(tabdyn t);

void affiche(tabdyn t);