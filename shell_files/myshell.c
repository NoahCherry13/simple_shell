#include "myshell_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stddef.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_INPUT 512

int get_num_commands(struct pipeline *pl){
  int num_commands = 1;
  struct pipeline_command *cmd = pl->commands;
  while (cmd->next != NULL){
    cmd = cmd->next;
    num_commands++;
  }
  return num_commands;
}

void mod_pipes(int sib_status, int pipe_num, int **pipefd)
{
  switch(sib_status){
  case 0:                               //only child
    //close(pipefd[pipe_num][0]);
    //close(pipefd[pipe_num][1]);
    break;
  case 1:                               //first sibling
    close(pipefd[pipe_num][0]);
    dup2(pipefd[pipe_num][1], 1); 
    break;
  case 2:                               //middle child
    dup2(pipefd[pipe_num][1], 1);
    break;
  case 3:                               //last child
    close(pipefd[pipe_num][1]);
    dup2(pipefd[pipe_num][0], 0);
    break;
  default:
    break;
  }
}

pid_t fork_child(char *args[], int sib_status, int pipe_num, int **fd_arr)
{
  pid_t cpid;
  cpid = fork();
  if (cpid < 0) {
    perror("fork failed\n");
    exit(-1);
  } else if (cpid == 0 && sib_status) {
    mod_pipes(sib_status, pipe_num, fd_arr);
  }
  return cpid;
}

int **create_fd(int num_pipe){
  int **fd_array;
  fd_array = (int **)malloc(sizeof(num_pipe * sizeof(int *)));
  for (int i = 0; i < num_pipe; i ++){
    fd_array[i] = (int *)malloc(2*sizeof(int));
  }
  return fd_array;
}

void free_fd(int **fd, int num_pipe)
{
  for (int i = 0; i < num_pipe; i++){
    free(fd[i]);
  }
  free(fd);
}

int main(int argc, char* argv[])
{
  int num_commands;
  int status;
  int spawn = 0;
  int num_pipes;
  int **fd_arr;
  int cpid;
  char input[MAX_INPUT];
  struct pipeline *pl;
  struct pipeline_command *pipeline_index = NULL;


  while (fgets(input, MAX_INPUT, stdin) != NULL){

    pl = pipeline_build(input); 
    num_commands = get_num_commands(pl);
    num_pipes = num_commands - 1;
    fd_arr = create_fd(num_pipes);
    pipeline_index = pl->commands;
    while(pipeline_index->next != NULL){
      pipeline_index = pipeline_index->next;
      cpid = fork_child(,0, num_pipes, fd_arr);
      spawn++;
    }
    if(cpid != 0){
      while(spawn){
	wait(&status);
	spawn--;
	printf("CHILD MURDERED (brutally)\n");
      }
    } else {
      execvp(pl->commands->command_args[0], pl->commands->command_args);
    }
     
    
  }
 
  
  //create fd array
   
  free_fd(fd_arr, num_pipes);
}
