#include "myshell_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stddef.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_INPUT 512
#define ONLY      0
#define FIRST     1
#define MIDDLE    2
#define LAST      3

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
  case FIRST:                               //only child
    //close(pipefd[pipe_num][0]);
    //close(pipefd[pipe_num][1]);
    break;
  case ONLY:                               //first sibling
    close(pipefd[pipe_num][0]);
    dup2(pipefd[pipe_num][1], 1); 
    break;
  case MIDDLE:                               //middle child
    dup2(pipefd[pipe_num][1], 1);
    break;
  case LAST:                               //last child
    close(pipefd[pipe_num][1]);
    dup2(pipefd[pipe_num][0], 0);
    break;
  default:
    break;
  }
}

pid_t fork_child(int sib_status, int pipe_num, int **fd_arr)
{
  pid_t cpid;
  cpid = fork();
  if (cpid < 0) {
    perror("fork failed\n");
    exit(-1);
  } else if (cpid == 0) {
    if (sib_status)mod_pipes(sib_status, pipe_num, fd_arr);
  }
  return cpid;
}

int **create_fd(int num_pipe){
  int **fd_array;
  fd_array = (int **)malloc(sizeof(num_pipe * sizeof(int *)));
  for (int i = 0; i < num_pipe; i ++){
    fd_array[i] = (int *)malloc(2*sizeof(int));
    pipe(fd_array[i]);
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
  int child_number = 0;
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

    if(pl->commands == NULL){
      continue;
    } else if (pl->commands->next == NULL){
      cpid = fork_child(ONLY, child_number, fd_arr);
      if (cpid) child_number ++;
    } else {
      pipeline_index = pl->commands;
      cpid = fork_child(FIRST, child_number, fd_arr);
      if (cpid) child_number ++;
      while(!cpid && pipeline_index->next != NULL){
	pipeline_index = pipeline_index->next;
	if(pipeline_index->next == NULL){
	  cpid = fork_child(LAST, child_number, fd_arr);
	  if (cpid) child_number ++;
	} else {
	  cpid = fork_child(MIDDLE, child_number, fd_arr);
	  if (cpid) child_number ++;
	}
	spawn++;
      }
    }


    if(cpid != 0){
      while(spawn){
	wait(&status);
	spawn--;
	printf("CHILD MURDERED (brutally)\n");
      }
    } else {
      pipeline_index = pl->commands;
      for(int i = 1; i < child_number; i++){
	pipeline_index = pipeline_index->next;
      }
      execvp(pipeline_index->command_args[0], pipeline_index->command_args);
    }
    child_number = 0;
  }
 
  
  //create fd array
  free_fd(fd_arr, num_pipes);
}
