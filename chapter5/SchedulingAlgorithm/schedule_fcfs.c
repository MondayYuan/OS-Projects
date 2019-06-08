#include <stdlib.h>
#include <stdio.h>

#include "schedulers.h"
#include "list.h"
#include "task.h"
#include "cpu.h"

struct node **taskListHead = NULL;

extern int turnaround_time;
extern int waiting_time;
extern int respond_time;
extern int task_num;

// add a task to the list 
void add(char *name, int priority, int burst){
    Task *newTask = malloc(sizeof(Task));

    newTask->name = name;
    newTask->priority = priority;
    newTask->burst = burst;
    newTask->startWaitingTime = 0;

    if(taskListHead == NULL){
        taskListHead = malloc(sizeof(struct node *));
    }

    insert(taskListHead, newTask);

    task_num ++;
}

// invoke the scheduler
void schedule(){
    struct node *temp;
    
    if(taskListHead == NULL){
        temp = NULL;
    }else{
        temp = *taskListHead;
    }

    while (temp != NULL) {
        run(temp->task, temp->task->burst);
        // printf("[%s] [%d] [%d]\n",temp->task->name, temp->task->priority, temp->task->burst);
        temp = temp->next;
    }

    printf("average turnaround time = %f\n", turnaround_time / (float)task_num);
    printf("average waiting time = %f\n", waiting_time / (float)task_num);
    printf("average respond time = %f\n", respond_time / (float)task_num);
}