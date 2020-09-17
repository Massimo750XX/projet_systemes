CC = gcc
CFLAGS = -Wall -g -std=c11
PKGCONFIG = `pkg-config --cflags --libs glib-2.0`
EXEC = tag

all : $(EXEC)

tag : main.c tag_hierarchy.c research.c tag_file.c paths_manager.c tag_hierarchy.h research.h tag_file.h paths_manager.h
	$(CC) $(CFLAGS) main.c research.c tag_hierarchy.c tag_file.c paths_manager.c -o tag $(PKGCONFIG)

cleanall:
	rm -rf *~ $(EXEC)