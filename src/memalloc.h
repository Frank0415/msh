#ifndef MEMALLOC_H
#define MEMALLOC_H

char *cmdline_alloc();
char **args_list_alloc();

void free_all(char *cmdline, char **args);

#endif