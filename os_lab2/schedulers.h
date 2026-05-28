#ifndef SCHEDULERS_H
#define SCHEDULERS_H

#include "list.h"

// Глобальные переменные для статистики
extern int task_count;
extern int total_turnaround;
extern int total_waiting;
extern int total_response;

// Функции планировщиков
void schedule_fcfs(struct node *head);
void schedule_sjf(struct node *head);
void schedule_priority(struct node *head);
void schedule_rr(struct node *head);
void schedule_priority_rr(struct node *head);

#endif