#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task.h"
#include "list.h"
#include "schedulers.h"

#define SIZE 100

int main(int argc, char *argv[]) {
    FILE *file;
    char temp[SIZE];
    char *name;
    int priority;
    int burst;
    
    struct node *head = NULL;
    
    // Проверка аргументов командной строки
    if (argc < 2) {
        printf("Usage: %s <schedule_file>\n", argv[0]);
        return 1;
    }
    
    // Открытие файла с задачами
    file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("Error: Cannot open file %s\n", argv[1]);
        return 1;
    }
    
    int tid = 0;
    
    // Чтение задач из файла
    while (fgets(temp, SIZE, file) != NULL) {
        Task *task = malloc(sizeof(Task));
        
        name = strtok(temp, ",");
        priority = atoi(strtok(NULL, ","));
        burst = atoi(strtok(NULL, ","));
        
        task->name = strdup(name);
        task->tid = tid++;
        task->priority = priority;
        task->burst = burst;
        task->remaining_time = burst;
        
        insert(&head, task);
    }
    
    fclose(file);
    
    // Выбор алгоритма планирования
    #ifdef FCFS
        schedule_fcfs(head);
    #elif SJF
        schedule_sjf(head);
    #elif PRIORITY
        schedule_priority(head);
    #elif RR
        schedule_rr(head);
    #elif PRIORITY_RR
        schedule_priority_rr(head);
    #else
        printf("No scheduling algorithm selected\n");
    #endif
    
    struct node *current = head;
    while (current != NULL) {
        struct node *next = current->next;
        free(current->task->name);
        free(current->task);
        free(current);
        current = next;
    }
    
    return 0;
}