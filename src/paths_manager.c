#define _GNU_SOURCE
//#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/xattr.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>

#include "paths_manager.h"


char file_paths[1024];

/**
* @brief Initialise variable globale du nom du fichier contenant les chemins des fichiers taggés.
*/
void init_file_paths() {
    memset(file_paths, 0, 1024);
   strcat(file_paths, "/.tag/paths.txt");
}



/**
* @brief Vérifie si le fichier existe.
*
* @param filename Nom du fichier.
* @return Renvoie 1 si le fichier existe, sinon 0.
*/
int check_file_existence(char * filename){

	if(access( filename, F_OK ) == -1) {

		perror("access error: ");
		exit(EXIT_FAILURE);
	}  

	return 1;

}


/**
* @brief Renvoie le chemin abosolu d'un fichier.
*
* @param filename Nom du fichier. 
* @return Renvoie le chemin absolu du fichier si il existe.
*/
char * absolute_path(char * filename){

	check_file_existence(filename);
	char *path = realpath(filename, NULL);

	if(path == NULL){
		perror("realpath error: ");
		exit(EXIT_FAILURE);
	} 

	return path;
	
}


/**
* @brief Vérifie si le chemin existe déjà dans paths.txt.
*
* @param file_paths Nom du fichier qui contient les chemins des fichiers taggés.
* @param filename Nom du fichier qu'on veut tagger.
* @return Renvoie 1 si le chemin existe sinon 0.
*/
int find_path(char * filename){

	char *path = absolute_path(filename);
	char *line_buf = NULL;
	size_t line_buf_size = 0;
	ssize_t line_size;
	FILE *file = fopen(file_paths, "r");

	if(file == NULL) {
        perror("fopen error: ");
        exit(EXIT_FAILURE);
    }

	line_size = getline(&line_buf, &line_buf_size, file);
		
	while (line_size>= 0) {

		line_buf[strcspn(line_buf, "\r\n")] = 0;

		if (strcmp(path, line_buf) == 0) {
			free(path);
			fclose(file);
			free(line_buf);
			return 1;
		}

		line_size = getline(&line_buf, &line_buf_size, file);
	}
	free(path);
	fclose(file);
	free(line_buf);
	return 0;
}


/**
* @brief Ajoute le chemin du fichier taggé dans paths.txt.
*
* @param filename Nom du fichier qu'on veut tagger.
* @return Renvoie 1 si le chemin a été ajouté dans le fichier paths.txt sinon 0.
*/
int add_path(char * filename){

	if(check_file_existence(filename) == 0){

		printf("The file doesn't exist!\n");
		return 0;
	}
	char *path = absolute_path(filename);
	if(path == NULL){
		printf("cannot find file with name [%s]\n", filename);
	}

	FILE  *file;

	file = fopen(file_paths, "a");

	if (file == NULL)
	{
		fprintf(stderr, "cannot open %s for appending\n", file_paths);
		exit(EXIT_FAILURE);
	}

	if(find_path(filename) == 1){
		free(path);
		return 0;
	}

	else {
		
		fprintf(file, "%s\n", path);
		free(path);

	}

	fclose(file);

	return 1;

}


/** 
* @brief Supprime le chemin absolu de "filename" qui se trouve dans "paths.txt".
*
* @param filename Nom du fichier.
* @return Renvoie 1 si le chemin a bien été supprimé, sinon 0.
*/
int delete_path(char * filename){

  
	if(check_file_existence(filename) == 0){

		//printf("le fichier n'existe pas !\n");
		return 0;
	}

	if(find_path(filename) == 0){

		//printf("This path doesn't exist in paths.txt!\n");
		return 0;
	}

	char *path = absolute_path(filename);

	char *line_buf = NULL;
	size_t line_buf_size = 0;
	ssize_t line_size;
	char *temp_file = "temp.txt";
	FILE *file = fopen(file_paths, "r");

	if(file == NULL) {
        perror("fopen error: ");
        exit(EXIT_FAILURE);
    }

	FILE *file2 = fopen(temp_file, "a+");

	if(file2 == NULL) {
        perror("fopen error: ");
        exit(EXIT_FAILURE);
    }

	line_size = getline(&line_buf, &line_buf_size, file);

	while (line_size>= 0) {

		if (strncmp(path, line_buf,strlen(line_buf)-1) != 0) {

			fprintf(file2, "%s", line_buf);
		}


		line_size = getline(&line_buf, &line_buf_size, file);
	}

	free(path);
	free(line_buf);
	fclose(file);
	fclose(file2);
	remove(file_paths);
	rename(temp_file, file_paths);


	return 1;
  
}


// ---------------------------  fonctions utilisées dans la partie recherche   ---------------------------

/**
* @brief Initialise un itérateur afin de parcourir chaque ligne de "paths.txt".
*
* @return Renvoie le fichier ouvert.
*/
void * init_iterator(){

	FILE *file = fopen(file_paths, "r"); 

	if(file == NULL) {
        perror("fopen error: ");
        exit(EXIT_FAILURE);
    }

	return file;
}


/**
* @brief Renvoie un chemin de "paths.txt".
*
* @param file Fichier contenant les chemins des fichiers taggés.
* @return Renvoie une ligne (chemin) du fichier.
*/
void * next_path(FILE * file){

	char *path = NULL;
	size_t line_buf_size = 0;
	ssize_t line_size;
    size_t ln;

	line_size = getline(&path, &line_buf_size, file);
    ln = line_size-1;
    
    if(path[ln] == '\n') path[ln] = '\0';
	
	if(line_size < 0) return NULL;
	return path;

}


/**
 * @brief Supprime le chemin absolu de "filename" qui se trouve dans "paths.txt" (sans vérifier s'il existe).
 * 
 * @param filename Le chemin absolu du fichier à enlever.
 * @return int 1.
 */
int delete_path2(char * filename){
    
	char *path = filename;
	char *line_buf = NULL;
	size_t line_buf_size = 0;
	ssize_t line_size;
	char *temp_file = "temp.txt";
	FILE *file = fopen(file_paths, "r");

	if(file == NULL) {
        perror("fopen error: ");
        exit(EXIT_FAILURE);
    }
	FILE *file2 = fopen(temp_file, "a+");

	if(file2 == NULL) {
        perror("fopen error: ");
        exit(EXIT_FAILURE);
    }

	line_size = getline(&line_buf, &line_buf_size, file);

	while (line_size>= 0) {
		if (strncmp(path, line_buf,strlen(line_buf)-1) != 0)
			fprintf(file2, "%s", line_buf);
		line_size = getline(&line_buf, &line_buf_size, file);
	}
    free(line_buf);
	fclose(file);
	fclose(file2);
	remove(file_paths);
	rename(temp_file, file_paths);
	return 1;
}
