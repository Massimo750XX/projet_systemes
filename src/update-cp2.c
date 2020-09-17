#include "update_paths.c"

int main(int argc, char const *argv[])
{
    init_file_paths();
    
    char *fileremp = buildfileremplace();
    
    FILE *remplace = fopen(fileremp, "r");
    if (remplace == NULL)
        erreur("ERREUR fopen\n");
    char *line_buff = NULL;
    size_t line_buf_size = 0;
	ssize_t line_size;

    deletefrompaths();

    char *pathdest = getdestination();
    if (pathdest == NULL)
        exit(EXIT_SUCCESS);
    
    add_delete_in_dir(pathdest);
    free(pathdest);

    line_size = getline(&line_buff, &line_buf_size, remplace);
    while (line_size >= 0)
    {
        line_buff[strcspn(line_buff, "\n")] = '\0';
        char *remp =realpath(line_buff, NULL);
        if (remp != NULL && find_path(remp) == 0)
            add_path(line_buff);
        line_size = getline(&line_buff, &line_size, remplace);
        free(remp);
    }
    fclose(remplace);
    free(line_buff);
    free(fileremp);
    return 0;
}