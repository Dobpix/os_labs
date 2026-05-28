#ifndef TASK_H
#define TASK_H


typedef struct task {
    char *name;
    int tid;
    int priority;
    int burst;          // время выполнения
    int remaining_time; // оставшееся время выполнения
} Task;

#endif