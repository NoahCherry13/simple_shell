#include "myshell_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stddef.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


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
    close(pipefd[pipe_num][0]);
    close(pipefd[pipe_num][1]);
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

pid_t fork_child(char *prog, char *const argv[], int sib_status, int pipe_num, int **fd_arr)
{
  pid_t cpid;
  cpid = fork();
  if (cpid < 0) {
    perror("fork failed\n");
    exit(-1);
  } else if (cpid == 0) {
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

int main (void)
{
  struct pipeline *pl = build_pipeline(argv);
  int num_commands = get_num_commands(pl);
  int num_pipe = num_commands - 1;
  // create fd array
  int **fd_array;
  fd_array = create_fd(num_pipe);
  free_fd(fd_array, num_pipe);
}
