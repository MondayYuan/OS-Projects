#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UNUSED "unused"
#define MAX_SIZE 255

typedef struct block
{
    int start;
    int end;
    int size;
    char name[MAX_SIZE];
}Block;

typedef struct block_node
{
    Block block;
    struct block_node *next;
}Node;

Node *memory;
int memory_size;

int allocate(char *name, int size, char mode);
void compact();
void show();
void release(char *name);
void init();

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("too many or too less parameters!\n");
        exit(-1);
    }

    memory_size = atoi(argv[1]);
    init();

    char command[MAX_SIZE];
    char *my_argv[MAX_SIZE];
    int my_argc;

    while(1){
        printf("allocator> ");
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

        // printf("my_argc=%d\n", my_argc);
        // printf("my_argv[0]=%s\n", my_argv[0]);

        if(strcmp(my_argv[0], "RQ") == 0 && my_argc == 4){
            int r = allocate(my_argv[1], atoi(my_argv[2]), my_argv[3][0]);
            if(r < 0){
                printf("fail to allocate memory!\n");
            }else{
                printf("succeed to allocate memory\n");
            }
        }else if(strcmp(my_argv[0], "RL") == 0 && my_argc == 2){
            release(my_argv[1]);
        }else if(strcmp(my_argv[0], "C") == 0 && my_argc == 1){
            compact();
        }else if(strcmp(my_argv[0], "STAT") == 0 && my_argc == 1){
            show();
        }else if(strcmp(my_argv[0], "exit") == 0){
            break;
        }else
        {
            printf("wrong command!\n");
        }
    }

}

void init(){
    memory = (Node *)malloc(sizeof(Node));

    Block new_block;
    new_block.start = 0;
    new_block.end = memory_size - 1;
    new_block.size = memory_size;
    strcpy(new_block.name, UNUSED);
    
    Node *new_node = (Node *)malloc(sizeof(Node));
    new_node->block = new_block;
    new_node->next = NULL;

    memory = new_node;
}

int allocate(char *name, int size, char mode){
    Node *pre = NULL;
    Node *tmp = memory;
    if(mode == 'F'){
        while(1){
            if(tmp == NULL){
                return -1;
            }
            if(strcmp(tmp->block.name, UNUSED) == 0){
                if(tmp->block.size > size){
                    Block new_block;
                    strcpy(new_block.name, name);
                    new_block.start = tmp->block.start;
                    new_block.size = size;
                    new_block.end = new_block.start + new_block.size - 1;

                    Node *new_node = (Node *)malloc(sizeof(Node));
                    new_node->block = new_block;
                    new_node->next = tmp;

                    tmp->block.start = new_block.end + 1;
                    tmp->block.size = tmp->block.end - tmp->block.start + 1;

                    if(pre == NULL){
                        memory = new_node;
                    }else{
                        pre->next = new_node;
                    }
                    break;
                }else if(tmp->block.size == size){
                    strcpy(tmp->block.name, name);
                    break;
                }
            }
            pre = tmp;
            tmp = tmp->next;
        }
    }else if(mode == 'W'){
        Node *biggest = NULL;
        while(tmp != NULL){
            if(strcmp(tmp->block.name, UNUSED) == 0){
                if(biggest == NULL){
                    biggest = tmp;
                    continue;
                }else if(tmp->block.size > biggest->block.size){
                    biggest = tmp;
                }
            }
            tmp = tmp->next;
        }
        if(biggest == NULL || biggest->block.size < size){
            return -1;
        }else if(biggest->block.size == size){
            strcpy(biggest->block.name, name);
        }else{
            Block new_block;
            strcpy(new_block.name, UNUSED);
            new_block.start = biggest->block.start + size;
            new_block.size = biggest->block.size - size;
            new_block.end = biggest->block.end;

            Node *new_node = (Node *)malloc(sizeof(Node));
            new_node->block = new_block;
            new_node->next = biggest->next;

            biggest->next = new_node;
            biggest->block.size = size;
            biggest->block.end = biggest->block.start + size - 1;
            strcpy(biggest->block.name, name);
        }
    }else if(mode == 'B'){
        Node *best = NULL;

        while(tmp != NULL){
            if(strcmp(tmp->block.name, UNUSED) == 0 && tmp->block.size >= size){
                if(best == NULL){
                    best = tmp;
                    continue;
                }else if(tmp->block.size < best->block.size){
                    best = tmp;
                }
            }
            tmp = tmp->next;
        }

        if(best == NULL){
            return -1;
        }else if(best->block.size == size){
            strcpy(best->block.name, name);
        }else{
            Block new_block;
            strcpy(new_block.name, UNUSED);
            new_block.start = best->block.start + size;
            new_block.size = best->block.size - size;
            new_block.end = best->block.end;

            Node *new_node = (Node *)malloc(sizeof(Node));
            new_node->block = new_block;
            new_node->next = best->next;

            best->next = new_node;
            best->block.size = size;
            best->block.end = best->block.start + size - 1;
            strcpy(best->block.name, name);
        }
    }else{
        printf("wrong mode!\n");
        return -1;
    }

    return 0;
}

void compact(){
    Node *new_memory = NULL;
    Node *tmp = memory;
    Node *new_tmp = NULL;

    while(tmp != NULL){
        if(strcmp(tmp->block.name, UNUSED) != 0){
            //first used block
            if(new_memory == NULL){
                new_memory = tmp;
                tmp->block.start = 0;
                tmp->block.end = tmp->block.size - 1;
                new_tmp = new_memory;
            }else{
                new_tmp->next = tmp;
                tmp->block.start = new_tmp->block.end + 1;
                tmp->block.end = tmp->block.start + tmp->block.size - 1;
                new_tmp = tmp;
            }
        }
        tmp = tmp->next;
    }
    //the memory is empty
    if(new_memory == NULL){
        return;
    //the memory is full
    }else if(new_tmp->block.end == memory_size - 1){
        new_tmp->next = NULL;
        memory = new_memory;
    }else{//create a new hole
        Block new_block;
        strcpy(new_block.name, UNUSED);
        new_block.start = new_tmp->block.end + 1;
        new_block.end = memory_size - 1;
        new_block.size = new_block.start - new_block.end + 1;

        Node *new_node = (Node *)malloc(sizeof(Node));
        new_node->block = new_block;
        new_node->next = NULL;

        new_tmp->next = new_node;
        
        memory = new_memory;
    }
}

void show(){
    Node *tmp = memory;
    while(tmp != NULL){
        printf("Addresses [%d : %d] %s\n", tmp->block.start, tmp->block.end, tmp->block.name);
        tmp = tmp->next;
    }
}

void release(char *name){
    Node *tmp = memory;
    Node *pre = NULL;
    while(tmp != NULL){
        //find the block to release
        if(strcmp(tmp->block.name, name) == 0){
            // if the next block is unused
            if(tmp->next != NULL && strcmp(tmp->next->block.name, UNUSED) == 0){
                //if the next block and the last block are both unused, merge three unused blocks
                if(pre != NULL && strcmp(pre->block.name, UNUSED) == 0){
                    pre->block.size += tmp->block.size + tmp->next->block.size;
                    pre->block.end = tmp->next->block.end;
                    pre->next = tmp->next->next;
                }else{ // if only the next block is unused, merge two unused blocks
                    strcpy(tmp->block.name, UNUSED);
                    tmp->block.size += tmp->next->block.size;
                    tmp->block.end = tmp->next->block.end;
                    tmp->next = tmp->next->next;
                }
            //if only the last block is unused, merge two unused blocks
            }else if(pre != NULL && strcmp(pre->block.name, UNUSED) == 0){
                pre->block.size += tmp->block.size;
                pre->block.end = tmp->block.end;
                pre->next = tmp->next;
            }else{//if neither the last block or the next block is unused, just change the name
                strcpy(tmp->block.name, UNUSED);
            }
            break;
        }
        pre = tmp;
        tmp = tmp->next;
    }

    if(tmp == NULL){
        printf("fail to find %s\n", name);
    }else{
        printf("succeed to release %s\n", name);
    }
}