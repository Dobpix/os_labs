#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "threadpool.h"

#define NUMBER_OF_THREADS 3

typedef struct task {
    void (*function)(void *p);
    void *data;
} task;


typedef struct task_node {
    task t;
    struct task_node *next;
} task_node_t;


static task_node_t *head = NULL;
static task_node_t *tail = NULL;
static int task_count = 0;
static int shutdown_flag = 0;

static pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t queue_cond = PTHREAD_COND_INITIALIZER;
static pthread_t workers[NUMBER_OF_THREADS];

static int enqueue(task t) {
    task_node_t *node = (task_node_t*)malloc(sizeof(task_node_t));
    if (node == NULL) {
        return 1;
    }
    node->t = t;
    node->next = NULL;

    if (tail == NULL) {
        head = tail = node;
    } else {
        tail->next = node;
        tail = node;
    }
    task_count++;
    return 0;
}


static task dequeue(void) {
    task_node_t *node = head;
    task t = node->t;
    head = head->next;
    if (head == NULL) {
        tail = NULL;
    }
    free(node);
    task_count--;
    return t;
}


static int is_queue_empty(void) {
    return head == NULL;
}


void *worker(void *param) {
    (void)param;

    while (1) {
        pthread_mutex_lock(&queue_mutex);


        while (task_count == 0 && !shutdown_flag) {
            pthread_cond_wait(&queue_cond, &queue_mutex);
        }

        if (shutdown_flag && task_count == 0) {
            pthread_mutex_unlock(&queue_mutex);
            break;
        }

        task t = dequeue();
        pthread_mutex_unlock(&queue_mutex);

        execute(t.function, t.data);
    }

    return NULL;
}


void execute(void (*somefunction)(void *p), void *p) {
    somefunction(p);
}


int pool_submit(void (*somefunction)(void *p), void *p) {
    task new_task;
    new_task.function = somefunction;
    new_task.data = p;

    pthread_mutex_lock(&queue_mutex);

    if (shutdown_flag) {
        pthread_mutex_unlock(&queue_mutex);
        return 1;
    }

    if (enqueue(new_task) != 0) {
        pthread_mutex_unlock(&queue_mutex);
        return 1;
    }


    pthread_cond_signal(&queue_cond);
    pthread_mutex_unlock(&queue_mutex);
    return 0;
}


void pool_init(void) {
    shutdown_flag = 0;
    head = tail = NULL;
    task_count = 0;

    pthread_mutex_init(&queue_mutex, NULL);
    pthread_cond_init(&queue_cond, NULL);

    for (int i = 0; i < NUMBER_OF_THREADS; i++) {
        pthread_create(&workers[i], NULL, worker, NULL);
    }
}


void pool_shutdown(void) {
    pthread_mutex_lock(&queue_mutex);
    shutdown_flag = 1;
    pthread_cond_broadcast(&queue_cond);
    pthread_mutex_unlock(&queue_mutex);

    for (int i = 0; i < NUMBER_OF_THREADS; i++) {
        pthread_join(workers[i], NULL);
    }


    pthread_mutex_destroy(&queue_mutex);
    pthread_cond_destroy(&queue_cond);

    while (!is_queue_empty()) {
        dequeue();
    }
}