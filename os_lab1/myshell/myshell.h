#ifndef MYSHELL_H
#define MYSHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>

#define MAX_LINE 1024
#define MAX_ARGS 64

// Прототипы функций
void print_prompt();
int read_command(char *line);
void parse_command(char *line, char **args);
int execute_builtin(char **args);
int execute_external(char **args);
void handle_redirection(char **args, int *in_fd, int *out_fd, int *append);

#endif
