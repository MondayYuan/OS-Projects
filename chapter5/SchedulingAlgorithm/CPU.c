/* CPU.c */
/**
 * "Virtual" CPU that also maintains track of system time.
 */

#include <stdio.h>
#include "task.h"

int tid = 0;

// run this task for the specified time slice
void run(Task *task, int slice) {
    extern int turnaround_time;
    extern int waiting_time;
    extern int respond_time;
    extern int clock;

    if(task->startWaitingTime == 0){
        respond_time += clock - task->startWaitingTime;
    }
    waiting_time += clock - task->startWaitingTime;

    task->tid = __sync_fetch_and_add(&tid, 1);
    // printf("tid = %d \n", task->tid);

    printf("Running task = [%s] [%d] [%d] for %d units.\n",task->name, task->priority, task->burst, slice);

    turnaround_time += slice;
    clock += slice;
    task->startWaitingTime = clock;
}
