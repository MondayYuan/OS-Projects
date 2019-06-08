#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>

sem_t *sem_full;
sem_t *sem_empty;
pthread_mutex_t mutex;

#define true 1

typedef int buffer_item;
#define BUFFER_SIZE 5
int head, rail;
/* the buffer */
buffer_item buffer[BUFFER_SIZE + 1];

void display(void){
    if(head == rail)
        printf("buffer is null\n");
    else{
        int i = (head + 1) % (BUFFER_SIZE + 1);
        while (true)
        {
            printf("%d ", buffer[i]);
            if(i == rail)
                break;
            i = (i + 1) % (BUFFER_SIZE + 1);
        }
        printf("\n");
    }
}

int insert_item(buffer_item item) {
    /* insert item into buffer
    return 0 if successful, otherwise
    return -1 indicating an error condition */

    rail = (rail + 1) % (BUFFER_SIZE + 1);
    buffer[rail] = item;

    printf("insert %d :", item);
    display();

    return 0;
}

int remove_item(buffer_item *item){
    /* remove an object from
    placing it in item
    return 0 if successful,
    return -1 indicating an error condition */
    

    head = (head + 1) % (BUFFER_SIZE + 1);
    *item = buffer[head];

    printf("remove %d :", *item);
    display();

    return 0;
}

void *producer(void *param) {
    buffer_item item;
    while (true) {
        /* sleep for a random period of time */
        sleep(rand()%4 + 1);
        sem_wait(sem_empty);
        pthread_mutex_lock(&mutex);

        item = rand() % 10;
        if (insert_item(item))
            printf("report error condition");
        // else
        //     printf("producer produced %d\n", item);

        pthread_mutex_unlock(&mutex);
        sem_post(sem_full);
    }
}

void *consumer(void *param) {
    buffer_item item;
    while (true) {
        /* sleep for a random period of time */
        sleep(rand()%4 + 1);
        sem_wait(sem_full);
        pthread_mutex_lock(&mutex);

        if (remove_item(&item))
            printf("report error condition");
        // else
        //     printf("consumer consumed %d\n",item);
        
        pthread_mutex_unlock(&mutex);
        sem_post(sem_empty);
    }
}

int main(int argc, char *argv[]){
    /* 1. Get command line arguments argv[1],argv[2],argv[3] */
    if(argc != 4){
        printf("numbers of params are wrong!");
        exit(-1);
    }
    int sleep_time = atoi(argv[1]);
    int num_producer = atoi(argv[2]);
    int num_consumer = atoi(argv[3]);

    printf("sleep time = %d, num producer = %d, num consumer = %d\n", sleep_time, num_producer, num_consumer);

    /* 2. Initialize buffer */
    pthread_mutex_init(&mutex,NULL);
    head = rail = 0;

    sem_unlink("ZYY_SEM_FULL");
    sem_close(sem_full);
    sem_unlink("ZYY_SEM_EMPTY");
    sem_close(sem_empty);
    sem_full = sem_open("ZYY_SEM_FULL", O_CREAT, 066, 0);
    sem_empty = sem_open("ZYY_SEM_EMPTY", O_CREAT, 066, BUFFER_SIZE);

    // /* Create producer thread(s) */
    pthread_t *producer_tid = (pthread_t *)malloc(num_producer * sizeof(pthread_t));
    for(int i=0; i<num_producer; ++i)
        pthread_create(&producer_tid[i], NULL, producer, NULL);
    
    // /* 4. Create consumer thread(s) */
    pthread_t *consumer_tid = (pthread_t *)malloc(num_consumer * sizeof(pthread_t));
    for (int i=0; i<num_consumer; ++i)
        pthread_create(&consumer_tid[i], NULL, consumer, NULL);

    /* 5. Sleep */
    sleep(sleep_time);

    /* 6. Exit */
    sem_unlink("ZYY_SEM_FULL");
    sem_close(sem_full);

    sem_unlink("ZYY_SEM_EMPTY");
    sem_close(sem_empty);
    return 0;
}