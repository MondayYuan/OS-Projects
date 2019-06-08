#include <stdlib.h>
#include <stdio.h>  
#include <string.h>

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4
#define MAX_SIZE 255
/* the available amount of each resource */
int available[NUMBER_OF_RESOURCES];
/*the maximum demand of each customer */
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
/* the amount currently allocated to each customer */
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
/* the remaining need of each customer */
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

int request_resources(int customer_num, int request[]);
void release_resources(int customer_num, int release[]);
int is_safe();
void init(int argc, char *argv[]);
void show();

int main(int argc, char *argv[]){
    if(argc != NUMBER_OF_RESOURCES+1){
        printf("The number of parameters do not match with NUMBER_OF_RESOURCES!\n");
        exit(-1);
    }

    init(argc, argv);

    char command[MAX_SIZE];
    int request[NUMBER_OF_RESOURCES];
    int custom_num;

    char *my_argv[MAX_SIZE];
    int my_argc;

    while(1){
        printf("> ");
        // gets(command);
        fgets(command, sizeof(command), stdin);

        my_argc = 0;
        for(int i=0; i<MAX_SIZE; ++i){
            if(command[i] == '\n' || command[i] == '\0'){
                command[i] = '\0';
                break;
            }
            if(command[i] == ' ' || command[i] == '\t'){
                command[i] = '\0';
            }else if(i == 0 || command[i-1] == '\0'){
                my_argv[my_argc++] = &command[i];
            }
        }
        my_argv[my_argc] = NULL;

        // printf("argc = %d\n", my_argc);
        // printf("command = %s\n", my_argv[0]);

        if(strcmp(my_argv[0], "*") == 0 && my_argc == 1){
            show();
        }else if(strcmp(my_argv[0], "RQ") == 0 && my_argc == NUMBER_OF_RESOURCES + 2){
            custom_num = atoi(my_argv[1]);
            for(int i=0; i<NUMBER_OF_RESOURCES; ++i){
                request[i] = atoi(my_argv[i+2]);
            }
            int r = request_resources(custom_num, request);
            if(r == -1){
                printf("fail to request\n");
            }else if(r == 0){
                printf("succeed to request\n");
            }
        }else if(strcmp(my_argv[0], "RL") == 0 && my_argc == NUMBER_OF_RESOURCES + 2){
            custom_num = atoi(my_argv[1]);
            for(int i=0; i<NUMBER_OF_RESOURCES; ++i){
                request[i] = atoi(my_argv[i+2]);
            }
            release_resources(custom_num, request);
        }else if(strcmp(my_argv[0], "exit") == 0 && my_argc == 1){
            break;
        } else{
            printf("wrong command!\n");
        }
    }

    return 0;
}

void init(int argc, char *argv[]){
    for(int i=1; i<argc; ++i){
        available[i-1] = atoi(argv[i]);
    }

    FILE *fp;
    fp = fopen("maximum.txt", "r");
    if(!fp){
        printf("fail to find the file!\n");
        exit(-1);
    }

    char buff[255];
    for(int i=0; i<NUMBER_OF_CUSTOMERS; ++i){
        fgets(buff, 255, fp);
        int start = 0;
        int k = 0;
        for(int j=0; j<255; ++j){
            if(buff[j] == ','){
                buff[j] = '\0';
                maximum[i][k] = atoi(&buff[start]);
                k++;
                start = j + 1;
            }else if(buff[j] == '\0'){
                maximum[i][k] = atoi(&buff[start]);
                break;
            }
        }
    }

    for(int i=0; i<NUMBER_OF_CUSTOMERS; i++){
        for(int j=0; j<NUMBER_OF_RESOURCES; ++j){
            allocation[i][j] = 0;
            need[i][j] = maximum[i][j];
        }
    }
} 

int request_resources(int customer_num, int request[]){
    if(customer_num >= NUMBER_OF_CUSTOMERS){
        printf("customer num is out of index!\n");
        return -2;
    }

    for(int i=0; i<NUMBER_OF_RESOURCES; ++i){
        if(request[i] > need[customer_num][i] || request[i] > available[i]){
            return -1;
        }
    }

    for(int i=0; i<NUMBER_OF_RESOURCES; ++i){
        available[i] -= request[i];
        allocation[customer_num][i] += request[i];
        need[customer_num][i] -= request[i];
    }

    if(is_safe() >= 0){
        return 0;
    }else{
        for(int i=0; i<NUMBER_OF_RESOURCES; ++i){
            available[i] += request[i];
            allocation[customer_num][i] -= request[i];
            need[customer_num][i] += request[i];
        }
        return -1;
    }
}

void release_resources(int customer_num, int release[]){
    if(customer_num >= NUMBER_OF_CUSTOMERS){
        printf("customer num is out of index!\n");
        return;
    }

    for(int i=0; i<NUMBER_OF_RESOURCES; ++i){
        int r;
        if(release[i] <= allocation[customer_num][i]){
            r = release[i];
        }else{
            r = allocation[customer_num][i];
        }
        available[i] += r;
        allocation[customer_num][i] -= r;
        need[customer_num][i] += r;
    }
}

int is_safe(){
    // show();
    int work[NUMBER_OF_RESOURCES];
    for(int j=0; j<NUMBER_OF_RESOURCES; ++j){
        work[j] = available[j];
    }
    int finish[NUMBER_OF_CUSTOMERS];
    for(int i=0; i<NUMBER_OF_CUSTOMERS; ++i){
        finish[i] = -1;
    }

    while(1){
        int i;
        for(i=0; i<NUMBER_OF_CUSTOMERS; ++i){
            if(finish[i] >= 0) continue;
            int j;
            for(j=0; j<NUMBER_OF_RESOURCES; ++j){
                if(need[i][j] > work[j]) break;
            }
            // printf("j=%d \n", j);
            if(j < NUMBER_OF_RESOURCES) continue;

            finish[i] = 1;
            // printf("work: ");
            for(int k=0; k<NUMBER_OF_RESOURCES; ++k){
                work[k] += allocation[i][k];
                // printf("%d ", work[k]);
            }
            // printf("\n");
            break;
        }

        // printf("i=%d \n", i);

        if(i == NUMBER_OF_CUSTOMERS){
            for(int k=0; k<NUMBER_OF_CUSTOMERS; ++k){
                if(finish[k] < 0) return -1;
            }
            return 0;
        }
    }
}

void show(){
    printf("available: \n");
    for(int i=0; i<NUMBER_OF_RESOURCES; ++i)
        printf("%d ", available[i]);
    printf("\nmaximum:\n");
    for(int i=0; i<NUMBER_OF_CUSTOMERS; ++i){
        for(int j=0; j<NUMBER_OF_RESOURCES; ++j){
            printf("%d ", maximum[i][j]);
        }
        printf("\n");
    }
    printf("allocation:\n");
    for(int i=0; i<NUMBER_OF_CUSTOMERS; ++i){
        for(int j=0; j<NUMBER_OF_RESOURCES; ++j){
            printf("%d ", allocation[i][j]);
        }
        printf("\n");
    }
    printf("need:\n");
    for(int i=0; i<NUMBER_OF_CUSTOMERS; ++i){
        for(int j=0; j<NUMBER_OF_RESOURCES; ++j){
            printf("%d ", need[i][j]);
        }
        printf("\n");
    }
}

