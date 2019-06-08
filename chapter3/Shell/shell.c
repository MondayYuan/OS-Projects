#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define MAX_LINE 80

int process_input(char input[], char *args[], int *argc);
void process_args(char *args[], int *in, int *out, int *pipe);

int main(void)
{
    char *args[MAX_LINE/2 + 1];
    int argc;
    char input[MAX_LINE];
    char last_command[MAX_LINE];
    last_command[0] = '\0';
    char tmp_command[MAX_LINE];

    char *last_args[MAX_LINE/2 + 1];
    int last_waiting;
    int last_argc=-1;

    // printf(pipe_file);
    // fflush(stdout);

    while(1){
        printf("ZYY > ");
        fflush(stdout);

        // gets(input)
        fgets(input, sizeof(input), stdin);
        strcpy(tmp_command, input);
        
        int waiting = process_input(input, args, &argc);

        int in = -1;
        int out = -1;
        int pipe_index = -1;

        int pipefd[2];
        pipe(pipefd);
        
        //history feature
        if(strcmp(args[0], "!!") == 0 && argc == 1){
            if(last_command[0] == '\0'){
                printf("No commands in history!\n");
                continue;
            }else{
                strcpy(input, last_command);
                waiting = process_input(input, args, &argc);
            }
        }

        //command "exit"
        if(strcmp(args[0], "exit") == 0 && argc == 1){
            break;
        }

        strcpy(last_command, tmp_command);

        //detect '>'and'<'and'|'
        process_args(args, &in, &out, &pipe_index);

        pid_t pid = fork();
        if(pid < 0){
            printf("fail to fork\n");
            exit(-1);
        }else if(pid == 0) {
            //son

            //communication via a pipe
            if(pipe_index > 0){
                pid_t pid_son = fork();
                if(pid_son < 0){
                    printf("fail to fork\n");
                    exit(-1);
                }else if(pid_son == 0)
                {
                    //son'son
                    close(pipefd[0]);
                    dup2(pipefd[1], STDOUT_FILENO);
                    execvp(args[0], args);
                    exit(-1); 
                }else{
                    //son
                    wait(NULL);

                    close(pipefd[1]);      
                    dup2(pipefd[0], STDIN_FILENO);
                    execvp(args[pipe_index + 1], args + pipe_index + 1);
                    close(pipefd[1]);
                    exit(-1);
                }
            }else{
                //redirecting input
                if(in > 0){
                    dup2(open(args[in + 1], O_RDONLY), STDIN_FILENO);
                }
                //redirection output
                if(out > 0){
                    dup2(open(args[out + 1], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR), STDOUT_FILENO);
                }
                execvp(args[0], args);
                exit(-1);
            }

        }else{
            //father
            if(waiting) wait(NULL);
            // printf("son exit\n");
        }
        sleep(1);
        fflush(stdin);
    }
    return 0;
}

//if command ends with &, it returns 0 else 1
int process_input(char input[], char *args[], int *argc)
{
    *argc = 0;
    for(int i=0; i<MAX_LINE; ++i){
        if(input[i] == '\n' || input[i] == '\0'){
            input[i] = '\0';
            break;
        }
        if(input[i] == ' ' || input[i] == '\t'){
            input[i] = '\0';
        }else if(i == 0 || input[i-1] == '\0'){
            args[(*argc)++] = &input[i];
        }
    }
    args[*argc] = NULL;

    if(strcmp(args[*argc - 1], "&") == 0){
        args[--(*argc)] = NULL;
        return 0;
    }else
    {
        return 1;
    }
    
    // int i = 0;
    // int j = 0;
    // while(1)
    // {
    //     char c = *(input + i);
    //     if(*(input + i) == ' ' || *(input + i) == '\t') *(input + i) = '\0';
    //     else if(*(input + i) == '\0')
    //     {
    //         args[j] = NULL;
    //         if(j > 0 && args[j - 1][0] == '&' && args[j - 1][1] == '\0')
    //         {
    //             args[j - 1] = NULL;
    //             return 0;
    //         }
    //         else return 1;
    //     }
    //     else
    //     {
    //         if(i == 0 || *(input + i - 1) == '\0')
    //        {
    //            args[j++] = input + i;
    //        }
    //     }
    //     i++;
    // }
}

//detect > and < and |
void process_args(char *args[], int *in, int *out, int *pipe)
{
    for(int i = 0; i < MAX_LINE/2 + 1; i++){
        if(!args[i]) break;
        if(strcmp(args[i], "<") == 0){
            *in = i;
            args[i] = NULL;
        }else if(strcmp(args[i], ">") == 0){
            *out = i;
            args[i] = NULL;
        }else if(strcmp(args[i], "|") == 0){
            *pipe = i;
            args[i] = NULL;
        }
    }
}
