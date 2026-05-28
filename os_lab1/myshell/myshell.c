#define _GNU_SOURCE
#include "myshell.h"

int main(int argc, char *argv[]) {
    char line[MAX_LINE];
    char *args[MAX_ARGS];

    // Установка shell и parent
    char shell_path[PATH_MAX];
    if (argc > 0 && realpath(argv[0], shell_path) != NULL) {
        setenv("shell", shell_path, 1);
    } else {
        setenv("shell", "/home/dobrs/os_lab1/myshell/myshell", 1);
    }
    setenv("parent", getenv("shell"), 1);

    printf("Добро пожаловать в myshell!\n");

    while (1) {
        print_prompt();

        if (read_command(line) == 0) break;
        if (strlen(line) == 0) continue;

        parse_command(line, args);
        if (args[0] == NULL) continue;

        if (execute_builtin(args) == 1) {
            // Специальная обработка quit
            if (strcmp(args[0], "quit") == 0 || strcmp(args[0], "exit") == 0) {
                break;
            }
            continue;
        }

        execute_external(args);
    }

    printf("До свидания!\n");
    return 0;
}

//ВСТРОЕННЫЕ КОМАНДЫ
int execute_builtin(char **args) {
    if (strcmp(args[0], "quit") == 0 || strcmp(args[0], "exit") == 0) return 1;

    if (strcmp(args[0], "help") == 0) {
        printf("Встроенные команды myshell:\n");
        printf("  cd [dir]     - сменить/показать текущий каталог\n");
        printf("  clr          - очистить экран\n");
        printf("  dir [dir]    - содержимое каталога\n");
        printf("  environ      - переменные окружения\n");
        printf("  help         - эта справка\n");
        printf("  pause        - ждать Enter\n");
        printf("  quit         - выход\n");
        return 1;
    }

    if (strcmp(args[0], "clr") == 0) { printf("\033[2J\033[H"); return 1; }

    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            printf("%s\n", getenv("PWD") ? getenv("PWD") : ".");
        } else if (chdir(args[1]) == 0) {
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd))) setenv("PWD", cwd, 1);
        } else perror("cd");
        return 1;
    }

    if (strcmp(args[0], "dir") == 0) {
        char *dir = args[1] ? args[1] : ".";
        DIR *d = opendir(dir);
        if (!d) { perror("dir"); return 1; }
        struct dirent *e;
        while ((e = readdir(d))) printf("%s\n", e->d_name);
        closedir(d);
        return 1;
    }

    if (strcmp(args[0], "environ") == 0) {
        extern char **environ;
        for (char **env = environ; *env; env++) printf("%s\n", *env);
        return 1;
    }

    if (strcmp(args[0], "pause") == 0) {
        printf("Нажмите Enter для продолжения...");
        while (getchar() != '\n');
        return 1;
    }

    return 0;
}

//ВНЕШНИЕ КОМАНДЫ + ПЕРЕНАПРАВЛЕНИЕ
int execute_external(char **args) {
    char *input_file = NULL, *output_file = NULL;
    int append = 0;
    char *clean_args[MAX_ARGS];
    int j = 0;

    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "<") == 0 && args[i+1] != NULL) {
            input_file = args[i+1];
            i++;
        } else if (strcmp(args[i], ">") == 0 && args[i+1] != NULL) {
            output_file = args[i+1];
            append = 0;
            i++;
        } else if (strcmp(args[i], ">>") == 0 && args[i+1] != NULL) {
            output_file = args[i+1];
            append = 1;
            i++;
        } else {
            clean_args[j++] = args[i];
        }
    }
    clean_args[j] = NULL;

    if (clean_args[0] == NULL) return 1;

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }

    if (pid == 0) {
        setenv("parent", getenv("shell"), 1);

        if (input_file) {
            int fd = open(input_file, O_RDONLY);
            if (fd >= 0) {
                dup2(fd, STDIN_FILENO);
                close(fd);
            } else {
                perror("open input");
                exit(1);
            }
        }

        if (output_file) {
            int flags = O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC);
            int fd = open(output_file, flags, 0666);
            if (fd >= 0) {
                dup2(fd, STDOUT_FILENO);
                close(fd);
            } else {
                perror("open output");
                exit(1);
            }
        }

        execvp(clean_args[0], clean_args);
        fprintf(stderr, "myshell: %s: команда не найдена\n", clean_args[0]);
        exit(1);
    } 

    wait(NULL);
    return 0;
}
