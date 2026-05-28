#ifndef LIST_H
#define LIST_H

#include "task.h"

// Структура узла списка
struct node {
    Task *task;
    struct node *next;
};

// Вставка задачи в список
void insert(struct node **head, Task *task);

// Удаление задачи из списка
void delete(struct node **head, Task *task);

// Обход списка
void traverse(struct node *head);

#endif