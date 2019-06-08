/* client.c */
/**
 * Example client program that uses thread pool.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "threadpool.h"

struct data
{
    int a;
    int b;
};

void add(void *param)
{
    struct data *temp;
    temp = (struct data*)param;

    printf("I add two values %d and %d result = %d\n",temp->a, temp->b, temp->a + temp->b);
}

int main(void)
{
    // create some work to do
    struct data *work;

    // initialize the thread pool
    pool_init();
    sleep(3);

    // submit the work to the queue
    for(int i=0; i<15; ++i){
        work = (struct data *)malloc(sizeof(struct data));
        work->a = i;
        work->b = 2*i;
        pool_submit(&add, work);
    }

    // may be helpful 
    sleep(3);
    pool_shutdown();

    return 0;
}
