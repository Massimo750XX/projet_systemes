#include "update_paths.c"

void execcp(char **argv)
{
    int pid = fork();
    if (pid < 0)
        erreur("ERREUR pid\n");
    else if(pid > 0)
    {
        execvp("cp", argv+1);
        exit(EXIT_FAILURE);
    }
    pid = wait(NULL);
}

int main(int argc, char *argv[])
{
    init_file_paths();
    char *pwd = getcwd(NULL, 200);
    int debut = -1;
    int t = -1;
    tabdyn tremplace = create_table();
    
    for (int i = 2; i < argc; i++)
    {
        char *tmp = realpath(argv[i], NULL);
        if (strcmp(argv[i], "-t") == 0)
            t = i;
        else if (tmp != NULL && debut == -1)
            debut = i;
        free(tmp);
    }
    if (debut == -1)
        goto exe;
    
    int nbfile = argc-debut-1;
    if (t!=-1)
    {
        char *tmp = realpath(argv[t+1], NULL);
        if (tmp == NULL)
            push(&tremplace, argv[t+1]);
        else
        {
            for (int i = t+2; i < argc; i++)
            {
                char *arg1 = absolute_path(argv[i]);
                char *arg2 = absolute_path(argv[t+1]);
                lister_supp(arg1, arg2, &tremplace);
                free(arg1);
                free(arg2);
            }
        }
        free(tmp);
    }
    else
    {   
        char *tmp = realpath(argv[argc-1], NULL);
        if (tmp == NULL)
            push(&tremplace, argv[argc-1]);
        else
        {
            for (int i = debut; i < argc-1; i++)
            {
                char *arg1 = absolute_path(argv[i]);
                char *arg2 = absolute_path(argv[argc-1]); 
                lister_supp(arg1, arg2, &tremplace);
                free(arg1);
                free(arg2);
            }
        }
        free(tmp);
    }
    exe : chdir(pwd);
    execcp(argv);
    
    char *fileremp = buildfileremplace();
    char *filedest = buildfiledest();
    FILE *remplace = fopen(fileremp, "w");
    FILE *dest = fopen(filedest, "w");
    if (remplace == NULL || dest == NULL)
        erreur("ERREUR fopen\n");
    
    for (int i = 0; i < tremplace.size; i++)
        fprintf(remplace, "%s\n",tremplace.tab[i]);
    if (t != -1)
    {
        char *arg = realpath(argv[t+1], NULL);
        if (arg != NULL)
            fprintf(dest, "%s\n", arg);
        else 
            fprintf(dest, "%s\n", argv[t+1]);
        free(arg);
    }
    else
    {
        char *arg = realpath(argv[argc-1], NULL);
        if (arg != NULL)
            fprintf(dest, "%s\n", arg);
        else
            fprintf(dest, "%s\n", argv[argc-1]);
        free(arg);
    }
    fclose(remplace);
    fclose(dest);

    deletetable(tremplace);
    free(pwd);
    free(filedest);
    free (fileremp);
    return 0;
}
