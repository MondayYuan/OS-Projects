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
        //run(temp->task, temp->task->burst);
        if(temp->task->burst > QUANTUM){
            run(temp->task, QUANTUM);
            temp->task->burst -= QUANTUM;
        }else
        {
            run(temp->task, temp->task->burst);
            delete(taskListHead, temp->task);
        }

        temp = temp->next;

        if(temp == NULL){ //When reach the rail, return to the head
            temp = *taskListHead;
        }
    }

    printf("average turnaround time = %f\n", turnaround_time / (float)task_num);
    printf("average waiting time = %f\n", waiting_time / (float)task_num);
    printf("average respond time = %f\n", respond_time / (float)task_num);
}