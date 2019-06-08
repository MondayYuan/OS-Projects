/**
 * Implementation of thread pool.
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "threadpool.h"
#include <fcntl.h>

// #define QUEUE_SIZE 10
#define NUMBER_OF_THREADS 3

#define TRUE 1

// this represents work that has to be 
// completed by a thread in the pool
typedef struct 
{
    void (*function)(void *p);
    void *data;
}
task;

typedef struct taskNode{
    task Task;
    struct taskNode *next;
}
node;

// sem_t *sem_submit;
sem_t *sem_queue;
pthread_mutex_t mutex;
node *taskListHead;

int run;

pthread_t tid[NUMBER_OF_THREADS];

// the work queue
task *worktodo;

// the worker bee
// pthread_t bee;

// insert a task into the queue
// returns 0 if successful or 1 otherwise, 
int enqueue(task t) 
{
    node *newNode = (node *)malloc(sizeof(node));
    newNode->Task = t;
    newNode->next = taskListHead;

    taskListHead = newNode;

    return 0;
}

// remove a task from the queue
task dequeue() 
{
    node *temp = taskListHead;
    node *prev = taskListHead;

    while(temp->next != NULL){
        prev = temp;
        temp = temp->next;
    }

    if(prev == temp){
        taskListHead = NULL;
        return temp->Task;
    }else{
        prev->next = NULL;
        return temp->Task;
        // return taskListHead->Task;
    }
}

// the worker thread in the thread pool
void *worker(void *param)
{
    while(run){
        sem_wait(sem_queue);
        if(run == 0) break;
        pthread_mutex_lock(&mutex);
        worktodo = (task *)malloc(sizeof(task));
        *worktodo =  dequeue();
        execute(worktodo->function, worktodo->data);
        pthread_mutex_unlock(&mutex);
    }
    printf("exit worker\n");
    pthread_exit(0);
}

/**
 * Executes the task provided to the thread pool
 */
void execute(void (*somefunction)(void *p), void *p)
{
    (*somefunction)(p);
}

/**
 * Submits work to the pool.
 */
int pool_submit(void (*somefunction)(void *p), void *p)
{
    // worktodo.function = somefunction;
    // worktodo.data = p;

    task *newTask = (task *)malloc(sizeof(task));
    newTask->function = somefunction;
    newTask->data = p;

    pthread_mutex_lock(&mutex);
    enqueue(*newTask);
    sem_post(sem_queue);
    pthread_mutex_unlock(&mutex);

    return 0;
}

// initialize the thread pool
void pool_init(void)
{
    //pthread_create(&bee,NULL,worker,NULL);
    pthread_mutex_init(&mutex,NULL);
    sem_unlink("SEM_QUEUE");
    sem_close(sem_queue);
    sem_queue = sem_open("SEM_QUEUE", O_CREAT, 0666, 0);

    taskListHead = NULL;
    run = 1;

    for(int i=0; i<NUMBER_OF_THREADS; ++i)
        pthread_create(&tid[i], NULL, worker, NULL);
}

// shutdown the thread pool
void pool_shutdown(void)
{
    run = 0;
    for(int i=0; i<NUMBER_OF_THREADS; ++i)
        sem_post(sem_queue);
    for(int i=0; i<NUMBER_OF_THREADS; ++i)
        pthread_join(tid[i],NULL);
    sem_unlink("SEM_QUEUE");
    sem_close(sem_queue);
}
