#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "schedulers.h"
#include "cpu.h"
#include "task.h"

// Глобальные переменные для статистики
int task_count = 0;
int total_turnaround = 0;
int total_waiting = 0;
int total_response = 0;

// FCFS планировщик
void schedule_fcfs(struct node *head) {
    printf("\n=== FCFS Scheduling ===\n");
    struct node *current = head;
    int time = 0;
    task_count = 0;
    total_turnaround = 0;
    total_waiting = 0;
    total_response = 0;
    
    while (current != NULL) {
        Task *task = current->task;
        run(task, task->burst);
        
        // Подсчет статистики
        task_count++;
        int completion_time = time + task->burst;
        int turnaround_time = completion_time;
        int waiting_time = time;
        int response_time = time;
        
        total_turnaround += turnaround_time;
        total_waiting += waiting_time;
        total_response += response_time;
        
        time += task->burst;
        current = current->next;
    }
    
    // Вывод средних значений
    printf("\nAverage Turnaround Time: %.2f\n", (float)total_turnaround / task_count);
    printf("Average Waiting Time: %.2f\n", (float)total_waiting / task_count);
    printf("Average Response Time: %.2f\n", (float)total_response / task_count);
}

// SJF планировщик
void schedule_sjf(struct node *head) {
    printf("\n=== SJF Scheduling ===\n");
    task_count = 0;
    total_turnaround = 0;
    total_waiting = 0;
    total_response = 0;
    
    int time = 0;
    struct node *current;
    
    while (1) {
        // Поиск задачи с наименьшим burst
        Task *shortest_task = NULL;
        int min_burst = INT_MAX;
        
        current = head;
        while (current != NULL) {
            Task *task = current->task;
            if (task->remaining_time > 0 && task->burst < min_burst) {
                min_burst = task->burst;
                shortest_task = task;
            }
            current = current->next;
        }
        
        if (shortest_task == NULL) break;
        
        run(shortest_task, shortest_task->burst);
        task_count++;
        
        int completion_time = time + shortest_task->burst;
        total_turnaround += completion_time;
        total_waiting += time;
        total_response += time;
        
        time += shortest_task->burst;
        shortest_task->remaining_time = 0;
    }
    
    printf("\nAverage Turnaround Time: %.2f\n", (float)total_turnaround / task_count);
    printf("Average Waiting Time: %.2f\n", (float)total_waiting / task_count);
    printf("Average Response Time: %.2f\n", (float)total_response / task_count);
}

// Приоритетный планировщик
void schedule_priority(struct node *head) {
    printf("\n=== Priority Scheduling ===\n");
    task_count = 0;
    total_turnaround = 0;
    total_waiting = 0;
    total_response = 0;
    
    int time = 0;
    struct node *current;
    
    while (1) {
        // Поиск задачи с наивысшим приоритетом
        Task *highest_priority_task = NULL;
        int max_priority = -1;
        
        current = head;
        while (current != NULL) {
            Task *task = current->task;
            if (task->remaining_time > 0 && task->priority > max_priority) {
                max_priority = task->priority;
                highest_priority_task = task;
            }
            current = current->next;
        }
        
        if (highest_priority_task == NULL) break;
        
        run(highest_priority_task, highest_priority_task->burst);
        task_count++;
        
        int completion_time = time + highest_priority_task->burst;
        total_turnaround += completion_time;
        total_waiting += time;
        total_response += time;
        
        time += highest_priority_task->burst;
        highest_priority_task->remaining_time = 0;
    }
    
    printf("\nAverage Turnaround Time: %.2f\n", (float)total_turnaround / task_count);
    printf("Average Waiting Time: %.2f\n", (float)total_waiting / task_count);
    printf("Average Response Time: %.2f\n", (float)total_response / task_count);
}

// Round Robin планировщик
void schedule_rr(struct node *head) {
    printf("\n=== Round Robin Scheduling ===\n");
    task_count = 0;
    total_turnaround = 0;
    total_waiting = 0;
    total_response = 0;
    
    // Сначала подсчитаем количество задач
    struct node *temp = head;
    int total_tasks = 0;
    while (temp != NULL) {
        total_tasks++;
        temp = temp->next;
    }
    
    int time = 0;
    int all_completed = 0;
    int *response_recorded = calloc(total_tasks, sizeof(int));
    
    // Инициализация массива
    for (int i = 0; i < total_tasks; i++) {
        response_recorded[i] = 0;
    }
    
    while (!all_completed) {
        all_completed = 1;
        struct node *current = head;
        int task_index = 0;
        
        while (current != NULL) {
            Task *task = current->task;
            
            if (task->remaining_time > 0) {
                all_completed = 0;
                
                // Запись времени отклика при первом запуске
                if (!response_recorded[task_index]) {
                    total_response += time;
                    response_recorded[task_index] = 1;
                }
                
                int execution_time = (task->remaining_time < QUANTUM) ? 
                                     task->remaining_time : QUANTUM;
                
                run(task, execution_time);
                task->remaining_time -= execution_time;
                time += execution_time;
                
                // Если задача завершена
                if (task->remaining_time == 0) {
                    total_turnaround += time;
                    total_waiting += (time - task->burst);
                    task_count++;
                }
            }
            current = current->next;
            task_index++;
        }
    }
    
    free(response_recorded);
    
    if (task_count > 0) {
        printf("\nAverage Turnaround Time: %.2f\n", (float)total_turnaround / task_count);
        printf("Average Waiting Time: %.2f\n", (float)total_waiting / task_count);
        printf("Average Response Time: %.2f\n", (float)total_response / task_count);
    }
}

// Приоритетный Round Robin планировщик
void schedule_priority_rr(struct node *head) {
    printf("\n=== Priority Round Robin Scheduling ===\n");
    task_count = 0;
    total_turnaround = 0;
    total_waiting = 0;
    total_response = 0;
    
    int time = 0;
    int *response_recorded = malloc(sizeof(int) * 100);
    for (int i = 0; i < 100; i++) response_recorded[i] = 0;
    
    while (1) {
        // Найти наивысший приоритет среди оставшихся задач
        int max_priority = -1;
        struct node *current = head;
        
        while (current != NULL) {
            Task *task = current->task;
            if (task->remaining_time > 0 && task->priority > max_priority) {
                max_priority = task->priority;
            }
            current = current->next;
        }
        
        if (max_priority == -1) break;
        
        // Выполнять задачи с максимальным приоритетом по Round Robin
        current = head;
        
        while (current != NULL) {
            Task *task = current->task;
            
            if (task->remaining_time > 0 && task->priority == max_priority) {
                // Запись времени отклика
                if (!response_recorded[task->tid]) {
                    total_response += time;
                    response_recorded[task->tid] = 1;
                }
                
                int execution_time = (task->remaining_time < QUANTUM) ? 
                                     task->remaining_time : QUANTUM;
                
                run(task, execution_time);
                task->remaining_time -= execution_time;
                time += execution_time;
                
                if (task->remaining_time == 0) {
                    total_turnaround += time;
                    total_waiting += (time - task->burst);
                    task_count++;
                }
            }
            current = current->next;
        }
    }
    
    free(response_recorded);
    
    if (task_count > 0) {
        printf("\nAverage Turnaround Time: %.2f\n", (float)total_turnaround / task_count);
        printf("Average Waiting Time: %.2f\n", (float)total_waiting / task_count);
        printf("Average Response Time: %.2f\n", (float)total_response / task_count);
    }
}