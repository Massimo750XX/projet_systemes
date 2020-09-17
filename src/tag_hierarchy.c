 #define _GNU_SOURCE
#include <fcntl.h> 
#include <unistd.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <string.h> 

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include <glib.h>
#include <glib/gprintf.h>

#include "tag_hierarchy.h"

struct tag_t {
	char			father[TAGNAME];
	char			name[TAGNAME];
	struct tag_t	*brother;
	struct tag_t	*children;
};

struct hierarchy {
	GHashTable 		*point_table;
	struct tag_t 	*tree;
};

#define TAGSIZE sizeof(struct tag_t)

char hierarchy_file[1024];

 
void write_tree(struct tag_t *tag, int fd);
void *build_tree(); 
void print_shift(int shift);
void print_tree(struct tag_t *tag, int shift); 
void print_tree_children(struct tag_t *tag);
void print_tag(struct tag_t *tag); 


/**
* @brief Initialise variable globale du nom du fichier contenant la hiérarchie.
*/
void init_hierarchy() {
    memset(hierarchy_file, 0, 1024);
	strcat(hierarchy_file, "/.tag/tag_hierarchy");
}

/** 
* @brief Supprime toute la liste et hiérarchie de tags existante en demandant confirmation à l'utilisateur. 
*/
void reset_hierarchy() {

	printf("You're going to suppress all tags from all tagged files and completely reset your tagset."
        "This reset operation cannot be undone. Do you want to reset ? Enter Y(es) or N(o)\n");
	char answer;

	wait_answer:
	scanf("%c", &answer);
	switch(answer) {
		case 'y':
		case 'Y':
			printf("Reset operation validated.\n");
			break;
		case 'n':
		case 'N':
			printf("Reset operation canceled.\n");
			exit(0);
		default :
			printf("Please enter Y or y to validate the reset operation, N or n to cancel it.\n");
			while(answer != '\n') scanf("%c", &answer);
			goto wait_answer;
	}

	FILE *f = fopen(hierarchy_file, "w");
    if(f == NULL) {
        perror("fopen");
        exit(1);
    }
    
	fclose(f);
}

// ----------------------------------------------------------------------------------------------
// -------------------------------- OBTENIR LES ENFANTS D'UN TAG --------------------------------

/**
* @brief Libère la mémoire d'une liste de struct tag_node.
* 
* @param tag_list Liste chaînée basée sur la structure struct tag_node.
*/
void free_tag_list(struct tag_node *tag_list) {
	struct tag_node *tag = tag_list;
	struct tag_node *temp;
	while (tag != NULL) {
		temp = tag;
		tag = tag->next;
		free(temp);
	}
}

/**
* @brief Affiche une liste chaînée de tag_node.
* 
* @param tag_list Liste chaînée basée sur la structure struct tag_node.
*/
void print_list(struct tag_node *tag_list) {
	struct tag_node *tag = tag_list;
	while(tag != NULL) {
		printf("%s\n", tag->name);
		tag = tag->next;
	}
}

/**
* @brief Construit la liste chaînée de struct tag_node avec une tête de lecture.
* 
* @param tag Tag à insérer.
* @param list Pointeur vers l'adresse où insérer le tag.
*/
void write_tag_list(struct tag_t *tag, struct tag_node **list) {
	while(tag != NULL) {
		struct tag_node *temp = *list;
		*list = malloc(sizeof(struct tag_node));
		memset(*list, 0, sizeof(struct tag_node));
		memcpy((*list)->name, tag->name, TAGNAME);
		(*list)->next = temp;

		if(tag->children != NULL) write_tag_list(tag->children, list);

		tag = tag->brother;
	}
}

/**
* @brief Récupère les enfants d'un tag.
*
* @param tag_name Nom d'un tag existant.
* @return Liste chaînée de struct tag_node des enfants de tag_name.
*/
void *get_tag_children(char *tag_name) {

	// --- CONSTRUCTION ARBRE ET HASHMAP ---

	struct hierarchy *h = build_tree(); 
	GHashTable *point_table = h->point_table;

	// 1. Avec la hashmap, trouver l'adresse du tag dans l'arbre
	struct tag_t *tag = g_hash_table_lookup(point_table, tag_name);

	if (tag == NULL) {
		perror("The tag you want the children from does not exist.\n");
		exit(1);
	}

	// -- PARCOURIR L'ARBRE POUR CONSTRUIRE LISTE CHAÎNÉE

	if (tag->children == NULL) return NULL;
	tag = tag->children;
	struct tag_node *list = NULL;

	write_tag_list(tag, &list);

	// -- LIBÉRER MÉMOIRE

	g_hash_table_destroy(point_table);
	free(h);

	// -- RENVOYER LA LISTE

	return list;
}


// ----------------------------------------------------------------------------------------------
// ------------------------------ EXISTENCE TAG DANS LA HIÉRARCHIE ------------------------------

/**
* @brief Détermine si un tag existe dans le tagset.
*
* @param tag_name Nom d'un tag.
* @return 1 si tag_name existe dans la liste et hiérarchie stockées, 0 sinon.
*/
int tag_exists(char *tag_name) {

	int fd = open(hierarchy_file, O_RDWR);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

	struct tag_t *tag = malloc(TAGSIZE);
	while(read(fd, tag, TAGSIZE) != 0) {
		if (strcmp(tag->name, tag_name) == 0) {
			free(tag);
			close(fd);
			return TRUE;
		}
	}
	free(tag);
	close(fd);
	return FALSE;
}

// ----------------------------------------------------------------------------------------------
// --------------------------------------- AJOUTER UN TAG ---------------------------------------

/**
* @brief Ajoute un tag dans le système de tags.

* Si "father" est NULL, le tag prend "root" comme père.
* Si tag_name existe déjà dans la liste des tags, 
* ou si father est différent de NULL et que le nom n'est pas trouvé, une erreur est renvoyée.
*
* @param father : nom du père.
* @param tags : tableau de noms de tags.
* @param nb_tags : nombre d'éléments dans tags.
*/
int create_tag(char *father, char* tags[], int nb_tags) {

    // -- VÉRIFICATION LONGUEURS DES TAGS --
    for(int i = 0; i < nb_tags; i++) {
        if(strlen(tags[i]) > 18) {
            printf("%s is too long for a tag name. Try a name with 18 caracters or less.\n", tags[i]);
            exit(1);
        }
    }

    // -- VÉRIFICATION (NON)EXISTENCE FATHER ET TAG --
    int fd = open(hierarchy_file, O_RDWR);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

    int father_exists = FALSE;

    struct tag_t *tag = malloc(TAGSIZE);

    while(read(fd, tag, TAGSIZE) != 0) {
        for (int i = 0; i < nb_tags; i++) {
            if(strcmp(tag->name, tags[i]) == 0) {
                printf("%s already exists in your tagset. Two tags cannot have the same name.\n", tags[i]);
                exit(1);
            }
        }
        if (father != NULL && strcmp(tag->name, father) == 0) father_exists = TRUE;
    }

    if (father != NULL && !father_exists) {
        printf("%s does not exist in your tagset yet.\nusage : tag create [-f <tag>] <tagname> [tagname] ...\n", father);
        exit(1);
    }

    for(int i = 0; i < nb_tags; i++) {
        memset(tag, 0, TAGSIZE);
        memcpy(tag->name, tags[i], strlen(tags[i]) + 1);
        tag->name[TAGNAME - 1] = '%';

        if (father == NULL) memcpy(tag->father, "root", strlen("root") + 1);
        else memcpy(tag->father, father, strlen(father) + 1);
        tag->father[TAGNAME-1] = '-';
        write(fd, tag, TAGSIZE);
    }


    close(fd);
    free(tag);

    return 0;
}

// ----------------------------------------------------------------------------------------------
// -------------------------------------- SUPPRIMER UN TAG --------------------------------------

/**
* @brief Supprime tag_name - et ses enfants - de la liste et la hiérarchie stockée.
*
* @param tag_name Nom du tag à supprimer.
*/
int delete_tag(char *tag_name) {

	// --- CONSTRUCTION ARBRE ET HASHMAP ---

	struct hierarchy *h = build_tree(); 
	GHashTable *point_table = h->point_table;
	struct tag_t *tree = h->tree;

	// --- DEMANDER CONFIRMATION SUPPRESSION ARBORESCENCE ---

	// 1. Avec la hashmap, trouver l'adresse du tag dans l'arbre
	struct tag_t *tag_to_delete = g_hash_table_lookup(point_table, tag_name);

	if (tag_to_delete == NULL) {
		printf("The tag you want to delete does not exist in your tagset.\n");
		exit(EXIT_FAILURE); 
	}

	printf("You're going to delete all this sub-hierarchy from your tagset and from the files tagged with them :\n");
	print_tree_children(tag_to_delete);
	printf("The removal cannot be undone, do you want to proceed ? Enter Y(es) or N(o)\n");
	char answer;

	wait_answer:
	scanf("%c", &answer);
	switch(answer) {
		case 'y':
		case 'Y':
			printf("Delete operation validated.\n");
			break;
		case 'n':
		case 'N':
			printf("Delete operation canceled.\n");
			exit(0);
		default :
			printf("Please enter Y or y to validate the delete operaton, N or n to cancel it.\n");
			while(answer != '\n') scanf("%c", &answer);
			goto wait_answer;
	}

	// --- SUPPRIMER DE L'ARBORESCENCE ---

	// 2. Trouver l'adresse du père
	struct tag_t *father = g_hash_table_lookup(point_table, tag_to_delete->father);
	struct tag_t **precedent = &(father->children);

	// 3. Introduire les frères de tag_to_delete en tête de liste des enfants du père
	if (*precedent == tag_to_delete) {
		*precedent = tag_to_delete->brother;
	} else {
		while((*precedent)->brother != tag_to_delete) precedent = &(*precedent)->brother;
		(*precedent) -> brother = tag_to_delete->brother;
	}

	// --- RÉ-ÉCRIRE SUR LE FICHIER ---

	int fd = open(hierarchy_file, O_WRONLY| O_TRUNC);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

	write_tree(tree->children, fd);
	close(fd);

	g_hash_table_destroy (point_table);
	free(h);

	return 0;
}

/**
* @brief Écrit un arbre dans un fichier.
*
* @param tag Pointeur vers la racine de l'arbre à écrire.
* @param fd Descripteur d'écriture d'un fichier.
*/
void write_tree(struct tag_t *tag, int fd) { 
	while(tag != NULL) {
		struct tag_t tag_bis = *tag;
		tag_bis.children = NULL;
		tag_bis.brother = NULL;
		write(fd, &tag_bis, TAGSIZE);
		if(tag->children != NULL) write_tree(tag->children, fd);
		tag = tag->brother;
	}
}


// ----------------------------------------------------------------------------------------------
// ------------------------ CONSTRUCTION HIERARCHIE PAR LECTURE FICHIER -------------------------

/**
* @brief Construit l'arbre du tagset à partir du fichier hierarchy.
*/
void *build_tree() {

	// --- INITIALISATION ---

	GHashTable *point_table = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, &free); 

	// insert root;
	struct tag_t *tree = malloc(TAGSIZE);
	struct tag_t *tag = tree;
	memset(tag, 0, TAGSIZE);
	memcpy(tag->name, "root", strlen("root") + 1);

	g_hash_table_insert(point_table, tag->name, tag);

	// ---- VARIABLES DE PARCOURS ----

	int fd = open(hierarchy_file, O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

	char *buf = malloc(TAGSIZE);
	memset(buf, 0, TAGSIZE);

	// --- LIRE LE FICHIER, CONSTRUIRE LE TABLEAU DE HIÉRARCHIE EN MÉMOIRE  ---

	while(read(fd, buf, TAGSIZE) != 0) {
		tag = (struct tag_t *)buf;

		// 0. Ajouter l'adresse de ce tag à la hashmap
		g_hash_table_insert(point_table, tag->name, tag) ; 

		// 1. Trouver l'adresse du père
		struct tag_t *inter = g_hash_table_lookup(point_table, tag->father);
		struct tag_t **father = &inter;

		// 2. Lier père à son fils (introduire entre père et fils)
		tag->brother = (*father)->children;
		(*father)->children = tag;

		// 3. Pour la prochaine itération
		buf = malloc(TAGSIZE);
		memset(buf, 0, TAGSIZE);
	}

	close(fd);

	struct hierarchy *h = malloc(sizeof(struct hierarchy));
	memset(h, 0, sizeof(struct hierarchy));
	h->point_table = point_table;
	h->tree = g_hash_table_lookup(point_table, "root");
	return h;
}

// ----------------------------------------------------------------------------------------------
// ----------------------------------- FONCTIONS D'AFFICHAGES -----------------------------------


/**
* @brief Affiche un décalage d'affichage.
*/
void print_shift(int shift) {
    for (int i = 0; i < shift; ++i) {
        printf("|");
        if (i == shift - 1) printf("-");
        else printf(" ");
    }
}

/**
* @brief Affiche le tagset enregistré.
*/
void print_hierarchy() {
    struct hierarchy *h = build_tree();
    print_tree(h->tree, 0);
    g_hash_table_destroy(h->point_table); 
    free(h);
}


/**
* @brief Affiche l'arbre de hiérarchie à partir d'un tag.
*
* @param tag Nom d'un tag.
* @param shift Indicateur de décalage d'affichage pour tag.
*/
void print_tree(struct tag_t *tag, int shift) { 
    while(tag != NULL) {
        print_shift(shift);
        if (strcmp(tag->name, "root") == 0) printf("TAGSET\n");
        else printf("%s\n", tag->name);
        if(tag->children != NULL) print_tree(tag->children, shift + 1);
        tag = tag->brother;
    }
}


/**
* @brief Affiche l'arbre des enfants d'un tag.
*
* @param tag Nom d'un tag.
*/
void print_tree_children(struct tag_t *tag) {
    printf("%s\n", tag->name);
    if(tag->children != NULL) print_tree(tag->children, 1);
}


/**
* @brief Affiche un tag.
*
* @param tag Adresse d'une structure tag_t.
*/
void print_tag(struct tag_t *tag) {
	printf("--PRINTING TAG--\n");
	printf("Father = %s\n", tag->father);
	printf("Name = %s\n", tag->name);
	printf("Brother = %p\n", tag->brother);
	printf("Child = %p\n", tag->children);
}
