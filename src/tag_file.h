#ifndef __TAG_FILE_H__
#define __TAG_FILE_H__


/* fonction qui lie des tags à un fichier grâce à xattr */
int link_tag(char *filename, char * tags[], size_t tags_size);

/* fonction qui supprime un tag du fichier */
int unlink_tag(char * filename, char * tags[], size_t tags_size, int ask, int print);

/* fonction qui récupère tous les tags d'un fichier et les stocke dans une liste de type struct tag_node */
void * get_file_tag_list(char * path);

/* fonction qui supprime tous les tags d'un fichier */
int delete_all_tags(char * filename);

/* fonction qui supprime de tous les fichiers un tag (et les sous-tags s'il y en a) */
int for_all_files_delete(char * tag[]);

/* fonction qui supprime tous les tags des fichiers, et supprime les chemins des fichiers de paths.txt */
int reset_all_files();

#endif