#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/xattr.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "tag_file.h"
#include "paths_manager.h"
#include "tag_hierarchy.h"




extern char file_paths[1024];


/**
* @brief Vérifie si tag est présent dans une suite de tags.
* 
* @param tags Une suite de tags.
* @param tag Un tag.
* @return Renvoie 1 new_tag existe dans tags, sinon 0.
*/
int check_tag_existence(char * tags, char * tag){

	char * pch;
  	pch = strstr(tags,tag);
  	
  	if(pch != NULL) return 1;
  	
  	return 0;

}


/**
* @brief Ajoute des attributs étendus (tags) au fichier.
*
* @param path Chemin vers le fichier qui va être taggé.
* @param usertag Nom d'un attribut.
* @param flags Variable qui détermine si on crée l'attribut (XATTR_CREATE) ou si remplace on le remplace (XATTR_REPLACE).
* @return Renvoie 1 si les tags ont bien été ajouté, sinon 0.
*/
int set_tag(char * path, char * usertag, char * all_tags, int flags){

    if(flags == 0){

    	if(setxattr(path,usertag,all_tags,strlen(all_tags),XATTR_CREATE) > -1){

    		add_path(path);
    	} 

    	else {

    		perror("setxattr error: ");
    		exit(EXIT_FAILURE);
    	}
    }

    if(flags){

    	if(setxattr(path,usertag,all_tags,strlen(all_tags),XATTR_REPLACE) > -1){		

    		add_path(path);
    	}   

    	else {

    		perror("setxattr error: ");
    		exit(EXIT_FAILURE);
    	}
    }

	return 1;
}


/**
* @brief Concatène des tags.
*
* @param tag_string Va contenir une suite de tags, séparés avec "/".
* @param tags[] Tableau qui contient tous les tags à concaténer.
* @param tags_size Nombre de tags.
*/
void concatenate_tags(char * tag_string, char * tags[], size_t tags_size){
    
	if(tags_size == 1){

		strcat(tag_string,tags[0]);
		strcat(tag_string,"/"); 

	}

	if(tags_size > 1){

		for(int i=0; i<tags_size; i++){

			strcat(tag_string,tags[i]); 
			strcat(tag_string,"/");
			strcat(tag_string,"\0"); 
		}

	}
}



/* *
* @brief Renvoie le nom du fichier, dans le cas où il s'agit d'un chemin il cherche le nom du fichier.
*
* @param file Nom/chemin du fichier.
* @return Renvoie le nom du fichier.
*/
char * find_filename(char * file){

	if(strrchr(file, '/') == NULL) return file;
	return strrchr(file, '/') + 1;	
}



/**
* @brief Lie des tags à un fichier grâce à xattr.
*
* @param filename Nom du fichier.
* @param tags[] Tableau qui contient tous les tags à ajouter.
* @param subtags_size Nombre de tags.
* @return Renvoie 1 si les tags ont bien été ajouté, sinon 0.
*/
int link_tag(char *filename, char * tags[], size_t tags_size){

	
	int val, res;
	int count = 0;
	char *path = absolute_path(filename);
	char buff_tag[1024];
	memset(buff_tag,'\0',1024);
	char new_buff_tag[1024];
	memset(new_buff_tag,'\0',1024);

	//on déclare new_tags qui va contenir tous les nouveaux tags concaténés
	char new_tags[1024];
	memset(new_tags,'\0',1024);

	char * usertag = "user.tags";


	val = listxattr(path, NULL, 0);

	if (val == -1) {
		perror("listxattr error");
		exit(EXIT_FAILURE);
	}

	//Cas où user.tags n'existe pas
	if (val == 0) {

		concatenate_tags(new_tags, tags, tags_size);
		res = set_tag(path, usertag, new_tags,0);

		if(res){

			printf("The file %s was tagged with the following tag(s):\n", find_filename(filename));			
			for(int i=0; i<tags_size; i++) printf("- %s\n", tags[i]);
			return 1;
		} 

	}

	if(val > 0){

		//On récupère dans buff_tag les tags concaténés du fichier
		//On appelle getxattr que dans le cas où user.tags existe déjà
		val = getxattr(path,usertag, &buff_tag, sizeof(buff_tag));
		
		if(val == -1 ){
			perror("getxattr error: ");
			exit(EXIT_FAILURE);
		} 

		//Cas où user.tags existe mais ne contient rien
		if(val == 0){

			concatenate_tags(new_tags, tags, tags_size);
			res = set_tag(path, usertag, new_tags,1);

			if(res){

				printf("The file %s was tagged with the following tag(s):\n", find_filename(filename));			
				for(int i=0; i<tags_size; i++) printf("- %s\n", tags[i]);
				

				return 1;
			} 

		}

		//Cas où on a déjà un ou plusieurs tags dans user.tags, on ajoute les nouveaux tags à la fin
		if(val > 0){

			//Concaténation des tags à ajouter en vérifiant si ils sont pas déjà liés au fichier
			if(tags_size == 1){

				if(check_tag_existence(buff_tag, tags[0]) == 1){

					printf("The file %s is already tagged with %s\n", find_filename(filename),tags[0]);
					return 1;
				}

				if(check_tag_existence(buff_tag, tags[0]) == 0){

					strcat(new_tags,tags[0]);
					strcat(new_tags,"/"); 

				}
			}

			if(tags_size > 1){

				for(int i=0; i<tags_size; i++){
					if(check_tag_existence(buff_tag, tags[i]) == 1){

						printf("The file %s is already tagged with %s\n", find_filename(filename),tags[i]);						
						count++;
					} 
					
					if(check_tag_existence(buff_tag, tags[i]) == 0){
						strcat(new_tags,tags[i]); 
						strcat(new_tags,"/");
						strcat(new_tags,"\0"); 
					}
				}
			}

			//On ajouter les nouveaux tags à la fin de buff_tags
			memcpy(new_buff_tag, buff_tag, 1024);
			strcat(new_buff_tag,new_tags);
			strcat(new_buff_tag,"\0"); 

			res = set_tag(path, usertag, new_buff_tag,1);

			if(res && (count != tags_size)){

				printf("The file %s was tagged with the following tag(s):\n", find_filename(filename));


				for(int i=0; i<tags_size; i++){

					if(check_tag_existence(buff_tag, tags[i]) == 0){
						printf("- %s\n", tags[i]);
					} 
				
				}
							
				return 1;
			} 

		}

	}

	return 0;

}


/**
* @brief Supprime un seul tag d'un fichier.
* 
* @param path Chemin vers le fichier taggé.
* @param existing_tags Une suite de tags.
* @param tag Tag qu'on va supprimer.
* @return Renvoie 1 si le tag a été supprimé.
*/
int delete_one_tag(char * path, char *existing_tags, char * tag, int print){

	char new_tag_string[1024];
	memset(new_tag_string,'\0',1024);
	char delim[]= "/";
	int val;
	char *ptr = NULL;

	ptr = strtok(existing_tags, delim);

	while(ptr != NULL)
	{
		if(strcmp(ptr,tag) != 0){

			strcat(new_tag_string,ptr); 
			strcat(new_tag_string,"/"); 

		}

		ptr = strtok(NULL, delim);
	}

	strcat(new_tag_string,"\0");

	//On attribut les sous-tags sans tag
	val = set_tag(path, "user.tags", new_tag_string,1);

	if(val == 1 && print == 1) printf("The tag %s has been deleted from %s.\n", tag, find_filename(path));

	return 1;

}



/**
* @brief Supprime un tag du fichier.
*
* @param filename Nom du fichier.
* @param tags[] Tags qu'on va supprimer.
* @param tags_size Taille de tags.
* @return Renvoie 0 si le ou les tags ont bien été supprimé, sinon 1.
*/
int unlink_tag(char * filename, char * tags[], size_t tags_size, int ask, int print){

	int val;
	char reply[100];
	char all_tags[1024];
	memset(all_tags,'\0',1024);
	char * subtags[100];
	char subtags_concat[1024];
	char *path = absolute_path(filename);
	char * usertag = "user.tags";
	char buff_tag[1024];
	struct tag_node * children_list = NULL;
	int j = 0;

    
	val = listxattr(path, NULL, 0);

	if (val == -1) {
		perror("listxattr error");
		exit(EXIT_FAILURE);
	}

	//Cas où user.tags n'existe pas
	if (val == 0 && print == 1) {

		printf("The file %s doesn't contain any tags.\n", find_filename(filename));
		return 0;

	}

	for (int i=0; i<tags_size;i++){

		val = getxattr(filename,usertag, &buff_tag, sizeof(buff_tag));
		
		if(val == -1 ){
			perror("getxattr error: ");
			exit(EXIT_FAILURE);
		} 

		if(check_tag_existence(buff_tag,tags[i]) == 0 && print == 1){
			printf("The file %s doesn't contain the tag %s.\n", find_filename(filename),tags[i]);
			continue;

		}
		//Cas où user.tags existe mais ne contient aucun tag
		if(val == 0 && print == 1){
			printf("The file %s doesn't contain tags anymore.\n", find_filename(filename));
			return 1;
		}

		buff_tag[val] = '\0';

	    children_list = get_tag_children(tags[i]);

	    if(children_list == NULL){

	    	delete_one_tag(path, buff_tag, tags[i], print);
	    }
       

        //Si le tag a des enfants dans la hierarchie
		if(children_list != NULL){
  
            //On vérifie si un ou plusieurs tags de children_list sont liés au fichier
            //On ajoute les tags de children_list qui sont liés dans char * subtags [100]
			while(children_list != NULL) {

				if(check_tag_existence(buff_tag, children_list->name) == 1){
					
					subtags[j] = children_list->name;
					j++;
				
				}
				
				children_list = children_list->next;
			}
			
			//Si on trouve pas un ou des enfants du tag lié au fichier, on supprime alors que le tag
			if(j == 0){

				delete_one_tag(path, buff_tag, tags[i], print);
				continue;
			} 

			//Si on trouve un ou plusieurs enfants liés au fichier

			if(j == 1 && ask && print){ 
				printf("The file %s has also the following subtag inherited from %s", find_filename(filename), tags[i]);
				printf("- %s\n", subtags[0]);
				printf("Do you want to delete this subtag? [yes/no]\n");
			}

			if(j > 1 && ask && print){
				printf("The file %s has also the following subtags inherited from %s", find_filename(filename), tags[i]);
				for(int i=0; i < j; i++) printf("- %s\n", subtags[i]);
				printf("Do you want to delete these subtags? [yes/no]\n");
			}
			
			if (ask && print) {

				while(1){
					scanf("%s", reply);
					if(strcmp(reply,"yes") == 0 || strcmp(reply,"no") ==0) break;
					printf("Reply with 'yes' or 'no'\n");
				}			
			}
			
			//Suppression du tag ainsi que ses enfants liés au fichier

			if(strcmp(reply,"yes") == 0 || ask == 0 || print == 0){

				concatenate_tags(subtags_concat, subtags, j);

				char delim[] = "/";

				char *ptr = strtok(buff_tag, delim);
				while(ptr != NULL)
				{

					if(strcmp(ptr,tags[i]) != 0 && check_tag_existence(subtags_concat, ptr) == 0){

						strcat(all_tags,ptr); 
						strcat(all_tags,"/"); 

					}

					ptr = strtok(NULL, delim);
				}

				//all_tags = contient tous les tags sauf ceux qu'on a supprimé
				strcat(all_tags,"\0");

				//On attribut les tags après supression au fichier
				val = set_tag(path, usertag, all_tags, 1);

				if(val == 0) return 0;

				if(print) printf("The tag %s and its subtags have been deleted from %s.\n", tags[i],strrchr(path, '/') + 1);
		
			}

			if(strcmp(reply,"no") == 0){

				//Suppression de tags[i] 
				delete_one_tag(path, buff_tag, tags[i], print);


			}
		}

		memset(buff_tag,'\0',1024);
		free_tag_list(children_list);
	}
    
	val = getxattr(filename,usertag, &buff_tag, sizeof(buff_tag));

	if(val == 0 && print){

		printf("The file %s doesn't contain tags anymore.\n", find_filename(filename));
		delete_path(filename);

	}

	return 1;

}


/**
* @brief Récupère tous les tags d'un fichier et les stocke dans une liste de type struct tag_node.
*
* @param path Chemin vers le fichier taggé.
* @return Renvoie la liste des tags, si le fichier ne contient aucun tag renvoie NULL.
*/
void * get_file_tag_list(char * path){

	int val;
	char buff_tag[1024];
	memset(buff_tag,'\0',1024);
	struct tag_node *tag_list = NULL;
	struct tag_node *temp = NULL;

	val = listxattr(path, NULL, 0);

	if (val == -1) {
		perror("get_file_tag_list listxattr error: ");
		exit(EXIT_FAILURE);
	}

	//Cas où user.tags n'existe pas
	if (val == 0) {

		return NULL;

	}

	val = getxattr(path,"user.tags", &buff_tag, sizeof(buff_tag));

	if(val == -1 ){
		perror("get_file_tag_list getxattr error: ");
		exit(EXIT_FAILURE);
	} 

	if(val > 0){

		char delim[] = "/";

		char * tag = strtok(buff_tag, delim);

		while(tag != NULL) {

			temp = tag_list; 
			tag_list = malloc(sizeof(struct tag_node));
			memset(tag_list, 0, sizeof(struct tag_node));
			memcpy(tag_list->name, tag, TAGNAME);
			tag_list->next = temp;

			tag = strtok(NULL, delim);
		}

	}

	return tag_list;

}



/**
* @brief Supprime tous les tags d'un fichier.
* 
* @param filename Nom du fichier taggé.
* @return Renvoie 1 si tous les tags du fichier ont été supprimé.
*/
int delete_all_tags(char * filename){

	char *path = absolute_path(filename);

	int val = removexattr(path, "user.tags");

	if (val == -1){
		perror("removexattr error: ");
		exit(EXIT_FAILURE);
	}

	if(delete_path(path) == 1){
		printf("All tags have been deleted from %s.\n",strrchr(path, '/') + 1);
	}

	return 1;
}


/**
* @brief Supprime de tous les fichiers un tag (et les sous-tags s'il y en a).
*
* @param tag[] Contient le nom du tag.
* @return Renvoie 1 si le tag a bien été supprimé des fichiers.
*/
int for_all_files_delete(char * tag[]){


	char *path = NULL;
	size_t line_buf_size = 0;
	ssize_t line_size;
	size_t ln;
	FILE *file = fopen(file_paths, "r");

	if(file == NULL) {
        perror("fopen error: ");
        exit(EXIT_FAILURE);
    }
	
	line_size = getline(&path, &line_buf_size, file);

	while (line_size>= 0) {

		ln = line_size-1;
		if(path[ln] == '\n') path[ln] = '\0';

		if (strlen(path) != 1) {

			unlink_tag(path,tag, 1, 0, 0);
		
		}


		line_size = getline(&path, &line_buf_size, file);
	}

	return 1;




}


/**
* @brief Supprime tous les tags des fichiers, et supprime les chemins des fichiers de paths.txt.
*
* @return Renvoie 1 si tout a été supprimé.
*/
int reset_all_files(){


	char *path = NULL;
	size_t line_buf_size = 0;
	ssize_t line_size;
	size_t ln;
	int val;

	FILE *file = fopen(file_paths, "r");
	
	if(file == NULL) {
        perror("fopen error: ");
        exit(EXIT_FAILURE);
    }

	line_size = getline(&path, &line_buf_size, file);

	while (line_size>= 0) {

		ln = line_size-1;
		if(path[ln] == '\n') path[ln] = '\0';

		if (strlen(path) != 1) {

			val = removexattr(path, "user.tags");
			
			if (val == -1 ){

				perror("removexattr error: ");
				exit(EXIT_FAILURE);
			}

			if(delete_path(path) == 0){

				printf("The following path wasn't deleted:\n");
				printf("%s\n",path);
				return 0;
			}
		}


		line_size = getline(&path, &line_buf_size, file);
	}

	return 1;


}
