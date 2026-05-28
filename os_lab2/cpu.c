#include <stdio.h>
#include "task.h"
#include "cpu.h"

// Выполнение задачи в течение указанного кванта времени
void run(Task *task, int slice) {
    printf("Running task = [%s] [%d] [%d] for %d units.\n", 
           task->name, task->priority, task->burst, slice);
}