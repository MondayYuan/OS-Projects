/*
a higher numeric value indicates a higher relative priority
*/

#include <stdlib.h>
#include <stdio.h>

#include "schedulers.h"
#include "list.h"
#include "task.h"
#include "cpu.h"

//represent a list of tasks with the same priority
typedef struct priority_node {
    struct node *head;
    int priority;
    struct priority_node *next;
} PriNode;

PriNode *taskListHead = NULL;

// add a task to the list 
void add(char *name, int priority, int burst){
    Task *newTask = malloc(sizeof(Task));

    newTask->name = name;
    newTask->priority = priority;
    newTask->burst = burst;

    PriNode *pri_temp = taskListHead;
    PriNode *pri_prev = taskListHead;

    while(1){
        if(pri_temp != NULL && pri_temp->priority > priority){
            pri_prev = pri_temp;
            pri_temp = pri_temp->next;
        }else{
            struct node *newNode = malloc(sizeof(struct node));
            newNode->task = newTask;
            newNode->next = NULL;

            if(pri_temp != NULL && pri_temp->priority == priority){
                newNode->next = pri_temp->head;
                pri_temp->head = newNode;
            }else{
                PriNode *newPriNode = malloc(sizeof(PriNode));
                newPriNode->head = newNode;
                newPriNode->priority = priority;
                newPriNode->next = pri_temp;

                if(pri_prev == pri_temp){
                    taskListHead = newPriNode;
                }else{
                    pri_prev->next = newPriNode;
                }
            }
            break;
        }
    }

    task_num ++;
}

// invoke the scheduler
void schedule(){
    PriNode *pri_temp = taskListHead;
    struct node *temp;

    while(pri_temp != NULL){
        temp = pri_temp->head;

        while(temp != NULL){
            if(temp->task->burst > QUANTUM){
                run(temp->task, QUANTUM);
                temp->task->burst -= QUANTUM;
            }else
            {
                run(temp->task, temp->task->burst);
                delete(&(pri_temp->head), temp->task);
            }

            temp = temp->next;

            if(temp == NULL){ //When reach the rail, return to the head
                temp = pri_temp->head;
            }
        }

        pri_temp = pri_temp->next;
    }

    printf("average turnaround time = %f\n", turnaround_time / (float)task_num);
    printf("average waiting time = %f\n", waiting_time / (float)task_num);
    printf("average respond time = %f\n", respond_time / (float)task_num);
}