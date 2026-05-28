#include <stdio.h>
#include <stdlib.h>
#include "list.h"

// Вставка задачи в конец списка
void insert(struct node **head, Task *newTask) {
    struct node *newNode = malloc(sizeof(struct node));
    newNode->task = newTask;
    newNode->next = NULL;
    
    if (*head == NULL) {
        *head = newNode;
        return;
    }
    
    struct node *temp = *head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = newNode;
}

// Удаление задачи из списка
void delete(struct node **head, Task *task) {
    struct node *temp = *head;
    struct node *prev = NULL;
    
    if (temp != NULL && temp->task == task) {
        *head = temp->next;
        free(temp);
        return;
    }
    
    while (temp != NULL && temp->task != task) {
        prev = temp;
        temp = temp->next;
    }
    
    if (temp == NULL) return;
    
    prev->next = temp->next;
    free(temp);
}

// Вывод списка
void traverse(struct node *head) {
    struct node *temp = head;
    while (temp != NULL) {
        printf("[%s] [%d] [%d]\n", 
               temp->task->name, 
               temp->task->priority, 
               temp->task->burst);
        temp = temp->next;
    }
}