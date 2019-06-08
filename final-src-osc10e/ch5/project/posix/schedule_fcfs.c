#include "schedulers.h"
#include "list.h"
#include "task.h"

extern node *taskListHead = NULL;

// add a task to the list 
void add(char *name, int priority, int burst){
    Task newTask;
    newTask.name = name;
    newTask.priority = priority;
    newTask.burst = burst;

    insert(&taskListHead, newTask);
}

// invoke the scheduler
void schedule();