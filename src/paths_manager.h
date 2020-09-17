#ifndef __PATHS_MANAGER_H__
#define __PATHS_MANAGER_H__



/* Initialise variable globale du nom du fichier contenant les chemins des fichiers taggés */
void init_file_paths();

/* fonction qui vérifie si le fichier existe */
int check_file_existence(char * filename);

/* fonction qui renvoie le chemin abosolu d'un fichier */
char * absolute_path(char * filename);

/* fonction qui vérifie si le chemin existe déjà dans paths.txt */
int find_path(char * filename);

/* fonction qui ajoute le chemin du fichier taggé dans paths.txt */
int add_path(char * filename);

/* fonction qui  supprime le chemin absolu de "filename" qui se trouve dans paths.txt */
int delete_path(char * filename);

/* fonction qui initialise un itérateur afin de parcourir chaque ligne de "paths.txt" */
void * init_iterator();

/* fonction qui renvoie un chemin de "paths.txt" */
void * next_path(FILE * file);

/* fonction qui supprime le chemin absolu de "filename" qui se trouve dans paths.txt (sans vérifier s'il existe) */
int delete_path2(char *pathfile);

#endif