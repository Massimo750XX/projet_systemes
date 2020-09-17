#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "tag_hierarchy.h"
#include "research.h"
#include "tag_file.h"
#include "paths_manager.h"

#define DEBUG 0

enum error { CREATE, DELETE, PRINT, LINK, UNLINK, SEARCH, RESET };

void exit_with_man_page();
void exit_with_help_page();
void exit_with_syntax_error(enum error e);
void exit_with_file_error(enum error e, char *filename);

int main(int argc, char **argv) {

	init_file_paths();
	init_hierarchy();

	if (argc < 2) exit_with_man_page();

    if (strcmp(argv[1], "--help") == 0) exit_with_help_page();

	char command;

	if      (strcmp(argv[1], "create") == 0) command = 'c';
	else if (strcmp(argv[1], "delete") == 0) command = 'd';
	else if (strcmp(argv[1], "print") == 0) command = 'p';
	else if (strcmp(argv[1], "link") == 0) command = 'l';
	else if (strcmp(argv[1], "unlink") == 0) command = 'u';
	else if (strcmp(argv[1], "search") == 0) command = 's';
	else if (strcmp(argv[1], "reset") == 0) command = 'r';
	else {
		printf("%s is not a tag command. See --help\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	char* filename;
	int tags_nb = argc - 3;
    char * tags[tags_nb];


	switch(command) {
		case 'c' :
			if (argc < 3) exit_with_syntax_error(CREATE);
			int begin = 2;
			char *father = NULL;
			if (strcmp(argv[2], "-f") == 0) {
				if (argc < 5) exit_with_syntax_error(CREATE);
				father = argv[3];
				begin += 2;
			}
			create_tag(father, argv + begin, argc - begin);
			break;

		case 'd' :
			if (argc != 3) exit_with_syntax_error(DELETE); 
			char *tag_to_delete = argv[2];
			if (tag_exists(tag_to_delete)) for_all_files_delete(argv+2);
			delete_tag(tag_to_delete);
			break;

		case 'p' :
			if (argc == 2) print_hierarchy();
			else if (argc == 3) {
				filename = argv[2];
				if (access(filename, F_OK) < 0) exit_with_file_error(UNLINK, filename);
				 struct tag_node *tag_list = get_file_tag_list(filename);
				 if(tag_list == NULL) printf("This file doesn't contain any tags.\n");
				 else print_list(tag_list);
				 free_tag_list(tag_list);
			}
			else exit_with_syntax_error(PRINT); 
			break;

		case 'l' :
			if (argc < 4) exit_with_syntax_error(LINK); 
			filename = argv[2];
			int size = 0;
			if (access(filename, F_OK) < 0) exit_with_file_error(LINK, filename);
			for (int i = 3; i < argc; i++) {
				if (!tag_exists(argv[i])) {
					printf("%s does not exist in your tagset yet. Create it first with tag create\n", argv[i]);
					continue;
				}
				tags[size] = malloc(1 + strlen(argv[i])); 
				strcpy(tags[size],argv[i]);
				size++;
			}
			if (size > 0) {

				link_tag(argv[2],tags, size);
				for(int i=0; i<size;i++) free(tags[i]);
			}

			break;

		case 'u' :
			if (argc < 4) exit_with_syntax_error(UNLINK); 
			filename = argv[2];
			if (access(filename, F_OK) < 0) exit_with_file_error(UNLINK, filename);
			if (strcmp(argv[3], "--all") == 0) { 
				if (argc == 4) delete_all_tags(filename);
				else exit_with_syntax_error(UNLINK);
			}
			else unlink_tag(filename, argv + 3, argc - 3, 1, 1);
			break;

		case 's' :
			if (argc < 3) exit_with_syntax_error(SEARCH);
			int opt = strcmp(argv[2], "-s");
			int ABS = opt == 0 ? 0 : 1;
			if (!ABS && argc < 4) exit_with_syntax_error(SEARCH);
			int place = opt == 0 ? 3 : 2;
			research(ABS, argv + place, argc - place);
			break;

		case 'r' :
			if (argc != 2) exit_with_syntax_error(RESET);
			reset_hierarchy();
			reset_all_files();
			break;

		default :
			fprintf(stderr, "Wrong parsing of arguments.\n");
			exit(-1);
	}
}


void exit_with_file_error(enum error e, char *filename) {
	printf("%s is not a valid file name\n", filename);
	switch(e) {
		case PRINT : printf("usage : tag print [file]\n"); break;
		case LINK : printf("usage : tag link <file> <tag> [tag] ...\n"); break;
		case UNLINK : printf("usage : tag unlink <file> <tag> [tag] ... or tag unlink <file> [--all]\n"); break;
		default : ;
	}
	exit(EXIT_FAILURE);
}


void exit_with_syntax_error(enum error e) {
	printf("Arguments are missing\n");
	switch(e) {
		case CREATE : printf("usage : tag create [-f <tag>] <tagname> [tagname] ...\n"); break;
		case DELETE : printf("usage : tag delete <tag>\n"); break;
		case PRINT : printf("usage : tag print [file]\n"); break;
		case LINK : printf("usage : tag link <file> <tag> [tag] ...\n"); break;
		case UNLINK : printf("usage : tag unlink <file> <tag> [tag] ... or tag unlink <file> [--all] \n"); break;
		case SEARCH : printf("usage : tag search [-s] tag [-not] [tag] ...\n"); break;
		case RESET : printf("usage : tag reset\n"); break;
	}
	printf("See --help to get more informations\n");
	exit(EXIT_FAILURE);
}


void exit_with_help_page() {
	printf(""
	"usage : tag [--help] <command> [<args>]\n"
	"These are common Tag commands used in various situations:\n"
	"\n"
	"manage the tagset\n"
	"	create 		Add tag to your tagset\n"
	"	delete  	Supress tag from your tagset, also supressing its children if needed\n"
	"	print 		Display the tagset\n"
	"\n"
	"add tags to file\n"
	"	link 		Add tags to a file\n"
	"	unlink 		Delete tags from a file\n"
	"\n"
	"find files\n"
	"	search 		Display the absolute path of files associated with a tag combination"
	"\n");
	exit(0);
}


void exit_with_man_page() {
	printf(""
	"NAME\n"
	"	tag - the manager of you tagging-file system.\n"
	"\n"
	"SYNOPSIS\n"
	"	tag [--help] <command> [<args>]\n"
	"\n"
	"DESCRIPTION\n"
	"	Tag is a tagging-file system manager made to organize files. You can create and manage\n"
	"	a tagset, associate files with the tags you have created and research files by tag combinations.\n"
	"\n"
	"OPTIONS\n"
	"	--help\n"
	"		Prints the synopsis and a list of the most commonly used commands and their descriptions\n"
	"\n"
	"TAG COMMANDS\n"
	"	tag create [-f <tag>] <tagname> [tagname] ...\n"
	"		Add tags to the tagset.\n"
	"		The -f option allows to specify a hierarchy : the tagnames are added to the\n"
	"		tagset as children of the tag following the -f option. A file tagged by any tag\n"
	"		is implicitly tagged with its father, grand_father etc. in the tagset hierarchy\n"
	"\n"
	"	tag delete <tag>\n"
	"		Delete a tag and its children from the tagset and, accordingly, from the files \n"
	"		containing them.\n"
	"\n"
	"	tag print [file]\n"
	"		Print the content of the tagset and its hierarchy, or the tags associated with a\n"
	"		file if given.\n"
	"\n"
	"	tag link <file> <tag> [tag] ...\n"
	"		Associate tags from the tagset to a file.\n"
	"\n"
	"	tag unlink <filename> <tag> [tag] ... \n"
	"		Disassociate tags from a file.\n"
	"	tag unlink <filename> --all\n"
	"		The --all option allows to delete all tags at once.\n"
	"\n"
	"	tag search [-s] tag [-not] [tag] ...\n"
	"		Print the absolute paths of the files corresponding to the given combinaison.\n"
	"		A tag combinaison enumerates valid tagnames preceded by an optional -not,\n"
	"		dividing tagnames in two groups : included or excluded.\n"
	"		To pass the combination test, a file must be tagged by all the included tagnames,\n"
	"		or any of their children, but must not be tagged by any of the excluded tagnames,\n"
	"		or any of their children.\n" 
	"		With the -s option, print only the names of the file, not the absolute path.\n"
	"\n"
	"	tag reset\n"
	"		Reset the tag-system : delete all tags from all files and reset the tagset.\n"
	"\n"
	"EXAMPLES\n"
	"	The following commands create a tagset with a hierarchy\n"
	"\n"
	"		tag create color genre movie music\n"
	"		tag create -f color red blue yellow pink\n"
	"		tag create -f blue light-blue azur turquoise\n"
	"		tag create -f genre western drama\n"
	"\n"
	"	The following commands tag and un-tag some files\n"
	"\n"
	"		tag link file1 movie western red yellow\n"
	"		tag link file2 music western\n"
	"		tag link file3 red light-blue yellow\n"
	"\n"
	"	The following command research files tagged with a color but without blue\n"
	"\n"
	"		tag search color -not blue\n"
	"	After the previous operations, it should return absolute path of file1\n"
	"\n"
	"IDENTIFIER TERMINOLOGY\n"
	"	<tag>\n"
	"		Indicates a valid tagname, already existing in the tagset.\n"
	"\n"
	"	<tagname>\n"
	"		Indicates a tagname to create and add to the hierarchy.\n"
	"		Length must be inferior to 18 caracters.\n"
	"\n"
	"	<file>\n"
	"		Indicates a filename.\n"
	"\n"
	"AUTHORS\n"
	"	TAG was written by Louis Castelbou, Hiba-Asmaa Chekroun and Blanche Miret.\n"
	"	Initiated by a class subject submitted by Inès Klimann.\n"
	"\n");
	exit(0);
}
