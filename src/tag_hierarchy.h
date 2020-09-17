#ifndef __TAG_MANAGER_H_
#define __TAG_MANAGER_H_

#define TAGNAME 20 // TAILLE EFFECTIVE : 18 

struct tag_node {
	char				name[TAGNAME];
	struct tag_node		*next;
};

void init_hierarchy();

/* Renvoie un pointeur vers un tag_node, liste chainée d'enfants de tag_name */
void *get_tag_children(char *tag_name);

/* Efface le fichier contenant la hiérarchie en demandant confirmation à l'utilisateur */
void reset_hierarchy();

/* Renvoie 1 si le tag de nom "tag_name" existe dans la hiérarchie, 0 sinon */
int tag_exists(char *tag_name);

/* Ajoute le tag "tag_name" à la hiérarchie comme enfant de "father" */
int create_tag(char *father, char *tags[], int nb_tags);

/* Supprime le tag de nom "tag_name" de la hiérarchie en demandant confirmation à l'utilisateur */
int delete_tag(char *tag_name);

/* Fonctions d'affichage */
void print_hierarchy();
void print_list(struct tag_node *tag_list);

/* Libération de mémoire */
void free_tag_list(struct tag_node *tag_list);

#endif