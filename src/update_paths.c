#include "update_paths.h"

/**
 * @brief Renvoie le chemin du fichier path dans sa nouvelle destination dest
 * 
 * @param path Fichier à deplacer
 * @param dest Destination
 * @return char* Le nouveau chemin de path
 */
char* traitement(char *path, char *dest)
{
    char *arg1 = absolute_path(path);
    char arg2[500] = "";
    strcat(arg2, dest);
    if (arg1 == NULL)
        erreur("ERREUR arguments invalides\n");
    int debut = -1;
    for (int j = 0; j < strlen(arg1); j++)
    {
        if (arg1[j] == '/')
            debut = j;
    }
   char namefile[100] = "";
    strcat(namefile, arg1+debut+1);
    char *remplace = calloc(600, 1);
    if (remplace == NULL)
        erreur("ERREUR malloc\n");
    strcat(remplace, arg2);
    if (arg2[strlen(arg2)] != '/')
        strcat(remplace, "/");
    strcat(remplace, namefile);

    free(arg1);
    return remplace;
}

/**
 * @brief Retourne le repertoire de destination
 * 
 * @param dest Chemin vers la destination
 * @return char* Repertoire qui contient la destination
 */
char *getdest(char *dest)
{
   int debut = -1;
    for (int j = 0; j < strlen(dest); j++)
    {
        if (dest[j] == '/')
            debut = j;
    }
    char *tmp = calloc(debut+2, 1);
    if (tmp == NULL)
        erreur("ERREUR calloc getdest\n");
    strncpy(tmp, dest, debut+1);
    return tmp;
}


/**
 * @brief Traite un élément (fichier ou repertoire) pour trouver son nouveau chemin et celui de ses sous-éléments si il en a
 * 
 * @param path Fichier à deplacer (fichier simple ou repertoire)
 * @param dest Destination
 * @param remove Tableau dynamique où sont stockés les chemins à supprimer
 * @param add Tableau dynamique où sont stockés les nouveaux chemins 
 */
void lister_supp(char *path, char *dest, tabdyn *add)
{
    int is = 0;
    char *remplace = traitement(path, dest);
    char *abspath = absolute_path(path);
    if (find_path(abspath))
        push(add, remplace);
    else
        is = 1;
    
    struct stat sb;
    char *pwd;

    if (stat(path, &sb) == -1)
        erreur("ERREUR stat list_sup\n");
    
    if (S_ISDIR(sb.st_mode))
    {
        pwd = getcwd(NULL, 0);
        chdir(path);

        DIR *dirp = opendir(".");
        struct dirent *entry;

        while ((entry=readdir(dirp)))
        {
            if (strcmp(entry->d_name, ".")!= 0 && strcmp(entry->d_name, "..") != 0)
                lister_supp(entry->d_name, remplace, add);
        }
        free(pwd);
        closedir(dirp);
    }
    free(abspath);
    free(remplace);
}

/**
 * @brief Construit le chemin /.tag/stockdelete.txt
 * 
 * @return char* /.tag/stockdelete.txt
 */
char *buildfiledelete()
{
    char *filedelete = calloc(200, 1);
    if (filedelete == NULL)
        erreur("ERREUR calloc filedelete\n");
    strcat(filedelete, "/.tag/delete.txt");
    return filedelete;
}

/**
 * @brief Construit le chemin /.tag/dest.txt
 * 
 * @return char* /.tag/dest.txt
 */
char *buildfiledest()
{
    char *filedest = calloc(200, 1);
    if (filedest == NULL)
        erreur("ERREUR calloc filedelete\n");
    strcat(filedest, "/.tag/dest.txt");
    return filedest;
}

/**
 * @brief Construit le chemin /.tag/remplace.txt
 * 
 * @return char* /.tag/remplace.txt
 */
char *buildfileremplace()
{
    char *fileremplace = calloc(200, 1);
    if (fileremplace == NULL)
        erreur("ERREUR calloc filedelete\n");
    strcat(fileremplace, "/.tag/remplace.txt");
    return fileremplace;
}


/**
 * @brief Retourne le chemin de la destination stockée dans dest.txt
 * 
 * @return char* Destination
 */
char *getdestination()
{
    char *filedest = buildfiledest();
    FILE *dest = fopen(filedest, "r");
    if ( dest == NULL)
        erreur("ERREUR open\n");
    char *line_buff = NULL;
    size_t line_buf_size = 0;
	ssize_t line_size;
    line_size = getline(&line_buff, &line_buf_size, dest);
    line_buff[strcspn(line_buff, "\n")] = '\0';
    char *pathdest = realpath(line_buff, NULL);
    if (pathdest == NULL)
        exit(EXIT_SUCCESS);
    
    free(line_buff);
    fclose(dest);
    free(filedest);
    return pathdest;
}

/**
 * @brief Regarde dans le repertoire path les fichiers qui sont taggés ou non
 * 
 * @param path Repertoire
 */
void choose_dir(char *path)
{
    chdir(path);
       DIR *dir = opendir(path);
       struct dirent *entry;
       while ((entry=readdir(dir)))
       {
            char buff_tag[1024];
            ssize_t istag = getxattr(entry->d_name, "user.tags", buff_tag, sizeof(buff_tag));
            if (istag >= 0)
            {
                if (find_path(entry->d_name) == 0)
                    add_path(entry->d_name);
            }
            else
            {
                if (find_path(entry->d_name) == 1)
                    delete_path(entry->d_name);
            }   
        }
        free(dir); 
}

/**
 * @brief Ajoute ou supprime de paths.txt les fichiers de pathdest si c'est un repertoire ou ceux du repertoire courant sinon
 * 
 * @param pathdest Destination 
 */
void add_delete_in_dir(char *pathdest)
{
   struct stat sb;

   if (stat(pathdest, &sb) == -1)
        erreur("ERREUR stat\n");
   if (S_ISDIR(sb.st_mode))
       choose_dir(pathdest);      
   else
   {
        char *tmp = getdest(pathdest);
        choose_dir(tmp);
        free(tmp);
   }
}

/**
 * @brief Supprime de paths.txt les chemins qui ne sont plus accessibles
 * 
 */
void deletefrompaths()
{
    init_file_paths();
    FILE *delete = fopen(file_paths, "r");
    if (delete == NULL)
        erreur("ERREUR fopen\n");
    
    char *line_buff = NULL;
    size_t line_buf_size = 0;
	ssize_t line_size;
    line_size = getline(&line_buff, &line_buf_size, delete);

    while (line_size >= 0)
    {
        line_buff[strcspn(line_buff, "\n")] = '\0';
        char *tmp = realpath(line_buff, NULL);
        if (tmp == NULL)
            delete_path2(line_buff);
        line_size = getline(&line_buff, &line_size, delete);
        free(tmp);
    }
    fclose(delete);
    free(line_buff);
}