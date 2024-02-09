#include "myshell_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stddef.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

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

void mod_pipes(int sib_status, int command_index, int **pipefd, int command_count)
{

  if(sib_status == FIRST){
    for(int i = 0; i < command_count; i ++){
      if(i == 0){
	dup2(pipefd[i][1], 1);
	close(pipefd[i][0]);
      }else{
	close(pipefd[i][0]);
	close(pipefd[i][1]);
      }
    }
  }else if (sib_status == MIDDLE){
    for(int i = 0; i < command_count; i ++){
      if(i == command_index-1){
	dup2(pipefd[i][0], 0);
	close(pipefd[i][1]);
      }else if(i == command_index){
	dup2(pipefd[i][1],1);
	close(pipefd[i][0]);
      }else{
	close(pipefd[i][0]);
	close(pipefd[i][1]);
      }
    }
  }else if (sib_status == LAST){
    for(int i = 0; i < command_count; i ++){
      if(i == command_index-1){
	dup2(pipefd[i][0], 0);
	close(pipefd[i][1]);
      }else{
	close(pipefd[i][0]);
	close(pipefd[i][1]);
      }
    }
  }
}

void redirect_child(struct pipeline *pl, int **fd, int command_num){
  struct pipeline_command *command_index = pl->commands;
  for(int i = 0; i < command_num; i ++){
    command_index = command_index->next;
  }
  if(command_index->redirect_in_path != NULL){
    int file = open(command_index->redirect_in_path, O_RDWR | O_CREAT, 0777);
    dup2(file, 0);
  }

  if(command_index->redirect_out_path != NULL){
    int file = open(command_index->redirect_out_path, O_RDWR | O_CREAT, 0777);
    dup2(file, 1);
  }
}

pid_t fork_child(int sib_status, int pipe_num, int **fd_arr, int num_pipes)
{
  pid_t cpid;
  cpid = fork();
  if (cpid < 0) {
    perror("fork failed\n");
    exit(-1);
  } else if (cpid == 0) {
    if (sib_status !=0 )mod_pipes(sib_status, pipe_num, fd_arr, num_pipes);
  }
  return cpid;
}

int **create_fd(int num_pipes){
  int **fd_array;
  fd_array = (int **)malloc(sizeof(num_pipes * sizeof(int *)));
  for (int i = 0; i < num_pipes; i ++){
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


void handle_child(int sig){
  int status;
  wait(&status);
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
    if(pl->is_background){
      struct sigaction sa;
      sa.sa_handler = &handle_child;
      sigaction(SIGCHLD, &sa, NULL);
      printf("is bg\n");
    }

    num_commands = get_num_commands(pl);
    num_pipes = num_commands - 1;
    fd_arr = create_fd(num_pipes);
    //if no command continue
    if(pl->commands == NULL){
      continue;
    } else if (pl->commands->next == NULL){
      //only child
      cpid = fork_child(ONLY, child_number, fd_arr, num_pipes);
      if (cpid){
	child_number ++;
	spawn ++;
      } 
    
    } else {
      //not only child, start must be first child
      pipeline_index = pl->commands;
      cpid = fork_child(FIRST, child_number, fd_arr, num_pipes);
      if (cpid){
	child_number ++;
	spawn ++;
      }
      //not only child and not first
      while(cpid && pipeline_index->next != NULL){
	pipeline_index = pipeline_index->next;
	if(pipeline_index->next == NULL){
	  cpid = fork_child(LAST, child_number, fd_arr, num_pipes);
	  if (cpid){
	    child_number ++;
	    spawn ++;
	  }
	} else {
	  cpid = fork_child(MIDDLE, child_number, fd_arr, num_pipes);
	  if (cpid){
	    child_number ++;
	    spawn ++;
	  }
	}
      }
    }

    //close the fd in the parent
    if(cpid != 0){
      for(int i = 0; i < num_pipes; i++){
	close(fd_arr[i][0]);
	close(fd_arr[i][1]);
      }
      if(!pl->is_background){
	printf("is fg\n");
	while(spawn){
	  wait(&status);
	  spawn--;
	  //printf("CHILD MURDERED (brutally)\n#Children Left: %d\n", spawn);
	}
      }
    } else {
      pipeline_index = pl->commands;
      for(int i = 0; i < child_number; i++){
	pipeline_index = pipeline_index->next;
      }
      redirect_child(pl, fd_arr, child_number);
      execvp(pipeline_index->command_args[0], pipeline_index->command_args);
    }
    child_number = 0;
  }
 
  
  //create fd array
  free_fd(fd_arr, num_commands);
}
