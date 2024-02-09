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


//closes and dupes pipes 
void mod_pipes(int sib_status, int command_index, int **pipefd, int command_count)
{
  for(int i = 0; i < command_count; i ++){
    if(i == command_index-1||i == command_index) continue;
    close(pipefd[i][0]);
    close(pipefd[i][1]);
  }
    
  if(sib_status == FIRST){
    close(pipefd[0][0]);
    dup2(pipefd[0][1], 1);
  }
  if(sib_status == MIDDLE){
    printf("COMMAND_INDEX: %d\n", command_index);
    close(pipefd[command_index][0]);
    close(pipefd[command_index-1][1]);
    dup2(pipefd[command_index][1], 1);
    dup2(pipefd[command_index-1][0], 0);
  }
  if(sib_status == LAST){
    close(pipefd[command_index-1][1]);
    dup2(pipefd[command_index-1][0], 0);
  }
}

//handles duping files for redirects
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

//forks parent and calls mod_pipes
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

//function for allocating pipes
int **create_fd(int num_pipes){
  int **fd_array;
  fd_array = (int **)malloc(sizeof(num_pipes * sizeof(int *)));
  for (int i = 0; i < num_pipes; i ++){
    fd_array[i] = (int *)malloc(2*sizeof(int));
    pipe(fd_array[i]);
  }
  return fd_array;
}

//function for freeing the pipes
void free_fd(int **fd, int num_pipe)
{
  for (int i = 0; i < num_pipe; i++){
    free(fd[i]);
  }
  free(fd);
}

//function for allocating array of child pids
pid_t *make_child_pid(int num_commands){
  pid_t *cld_pid_arr = malloc(num_commands*sizeof(pid_t));
  return cld_pid_arr;
}

//SIGCLD function handler
void handle_child(int sig, siginfo_t *info, void *context){
  int status;
  waitpid(info->si_pid, &status, 0);
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
  pid_t *child_pid_arr = NULL;
  struct pipeline *pl;
  struct pipeline_command *pipeline_index = NULL;
  int n_flag = 0;

  //Checks if -n flag is enabled
  if(argc<2){
    n_flag = 0;
  }else{
    if(!strcmp(argv[1], "-n")){
      n_flag = 1;
    }
  }
  if(!n_flag) printf("my_shell$ ");

  //while loop for running the shell
  while (fgets(input, MAX_INPUT, stdin) != NULL){
    
    if(!strcmp(input, "\n")){
      if(!n_flag) printf("my_shell$ ");
      continue;
    }

    pl = pipeline_build(input);
    num_commands = get_num_commands(pl);
    num_pipes = num_commands - 1;
    fd_arr = create_fd(num_pipes);
    child_pid_arr = make_child_pid(num_commands);
    
    if(pl->is_background){
      struct sigaction sa;
      sa.sa_sigaction = &handle_child;
      sa.sa_flags = SA_NOCLDSTOP | SA_RESTART;
      sigaction(SIGCHLD, &sa, NULL);
    }

    //if no command continue
    if(pl->commands == NULL){
      continue;
    } else if (pl->commands->next == NULL){
      //only child
      cpid = fork_child(ONLY, child_number, fd_arr, num_pipes);
      if (cpid){
	child_pid_arr[child_number] = cpid;
	child_number ++;
	spawn ++;
      } 
    
    } else {
      //not only child, start must be first child
      pipeline_index = pl->commands;
      cpid = fork_child(FIRST, child_number, fd_arr, num_pipes);
      if (cpid){
	child_pid_arr[child_number] = cpid;
	child_number ++;
	spawn ++;
      }
      //not only child and not first
      while(cpid && pipeline_index->next != NULL){
	pipeline_index = pipeline_index->next;
	if(pipeline_index->next == NULL){
	  cpid = fork_child(LAST, child_number, fd_arr, num_pipes);
	  if (cpid){
	    child_pid_arr[child_number] = cpid;
	    child_number ++;
	    spawn ++;
	  }
	} else {
	  cpid = fork_child(MIDDLE, child_number, fd_arr, num_pipes);
	  if (cpid){
	    child_pid_arr[child_number] = cpid;
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
	
	for (int i = 0; i < num_commands; i++){
	  waitpid(child_pid_arr[i], &status, 0);
	}
      }
    } else {
      pipeline_index = pl->commands;
      for(int i = 0; i < child_number; i++){
	pipeline_index = pipeline_index->next;
      }
      redirect_child(pl, fd_arr, child_number);
      printf("COMMAND: %s\n", pipeline_index->command_args[0]);
      execvp(pipeline_index->command_args[0], pipeline_index->command_args);
    }
    child_number = 0;
    free(child_pid_arr);
    if(!n_flag)printf("my_shell$ ");
  }
 
  
  //create fd array
  free_fd(fd_arr, num_commands);
}
