// threadpool.h
#ifndef THREADPOOL_H
#define THREADPOOL_H


void pool_init(void);
int pool_submit(void (*somefunction)(void *p), void *p);
void pool_shutdown(void);

void execute(void (*somefunction)(void *p), void *p);
void *worker(void *param);

#endif