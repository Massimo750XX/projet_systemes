#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "tag_hierarchy.h"
#include "paths_manager.h"
#include "tag_file.h"

struct tag_l {
	int 			must_be;
	struct tag_node *list;
};


/**
* @brief Construit une liste contenant un tag et ses enfants.
*
* @param tag_name Nom d'un tag.
* @param must_be Indicateur de recherche à positionner pour la liste à créer.
*
* @return Struct tag_l contenant tag_name et ses enfants.
*/
struct tag_l build_list(char *tag_name, int must_be) {
	struct tag_l tag_list;
	tag_list.must_be = must_be;
	tag_list.list = get_tag_children(tag_name);
	struct tag_node *temp = tag_list.list;
	tag_list.list = malloc(sizeof(struct tag_node));
	memcpy(tag_list.list->name, tag_name, TAGNAME);
	tag_list.list->next = temp;

	return tag_list;
}


/**
* @brief Détermine si un tag appartient à une liste de tag.
*
* @param tag_name Nom d'un tag.
* @param tag_list Liste de tags.
* @return 1 si tag_name appartient à la liste tag_list, 0 sinon.
*/
int belong_to_list(char *tag_name, struct tag_l tag_list) {
	struct tag_node *tag = tag_list.list;
	while(tag != NULL) {
		if(strcmp(tag_name, tag->name) == 0) return 1;
		tag = tag->next;
	}
	return 0;
}


/**
* @brief Affiche le nom d'un fichier ou son chemin complet selon l'agument ABS.
* 
* @param path Chemin absolu d'un fichier.
* @param ABS 0 ou 1, détermine si le chemin affiché doit être absolu ou non.
*/
void print_filename(char *path, int ABS) {
	if(ABS) printf("%s\n", path);
	else printf("%s\n", strrchr(path, '/') + 1);
}


/**
* @brief Affiche les chemins des fichiers correspondant à la combinaison de tags passée en paramètres.
*
* @param ABS 0 ou 1, détermine si les chemins affichés doivent être absolus ou non.
* @param terms Tableau de chaîne de caractères, soit des noms de tags soit des indicateurs "-not".
* @param nb_terms Nombre d'éléments dans terms.
*/
void research(int ABS, char **terms, int nb_terms) {

	// 0. Compter le nombre de termes "utiles" ( -> #listes ).
	int c = 0;
	for (int i = 0; i < nb_terms; i++) {
		if(strcmp(terms[i], "-not") != 0) {
			if (!tag_exists(terms[i])) {
				printf("%s does not exist in your tagset yet. Create it first with tag create\n", terms[i]);
				exit(0);
			}
			c ++;
		}
	}
	
	// 1. Construire le tableau.
	struct tag_l *tab = malloc(sizeof(struct tag_l) * c);

	// 2. Parcourir les termes 1 par 1 et créer une liste par terme.
	c = 0;
	for (int i = 0; i < nb_terms; i++) {
		int must_be = 1;

		if(strcmp(terms[i], "-not") == 0) {
			must_be = 0;
			i++;
		}

		if (i >= nb_terms) {
			printf("Tagname missing after -not : stoping\n");
			exit(EXIT_FAILURE);
		}

		tab[c] = build_list(terms[i], must_be);
		c++;
	}

	// 3. Filtrer la liste de documents taggés.
	FILE *path_file = init_iterator();
	char *path = next_path(path_file);

	int nb_file = 0;

	while(path != NULL) { // parcours de path

		struct tag_node *tag_list = get_file_tag_list(path);

		for(int i = 0; i < c; i++) { // parcours de liste

			struct tag_node *tag = tag_list;
			int tag_found = 0;

			while(tag != NULL) { // parcours des tags du fichier

				int belong = belong_to_list(tag->name, tab[i]);
				if(tab[i].must_be) {
					if (belong) goto next_list;
				} else {
					if (belong) goto next_file;
				}
				tag = tag->next;
			}

			if (tab[i].must_be && !tag_found) goto next_file;

			next_list :
			;

		}

		if (nb_file == 0) printf("Here are the files corresponding to your research : \n");
		print_filename(path, ABS);
		nb_file += 1;

		next_file :
		path = next_path(path_file);

		free_tag_list(tag_list);
	}

	if(nb_file == 0) printf("No file corresponds to your research.\n");

	for(int i = 0; i < c; i++) {
		free_tag_list(tab[i].list);
	}
	free(tab);

}
