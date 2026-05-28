#include "myshell.h"

// Вывод приглашения
void print_prompt() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s> ", cwd);
    } else {
        printf("myshell> ");
    }
}

// Чтение команды
int read_command(char *line) {
    if (fgets(line, MAX_LINE, stdin) == NULL) {
        return 0;
    }
    line[strcspn(line, "\n")] = '\0';
    return 1;
}

//парсер
void parse_command(char *line, char **args) {
    int i = 0;
    args[i] = strtok(line, " \t");
    while (args[i] != NULL && i < MAX_ARGS - 1) {
        args[++i] = strtok(NULL, " \t");
    }
    args[i] = NULL;
}
