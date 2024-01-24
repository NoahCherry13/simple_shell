#include <stdio.h>
#include <string.h>
#include "myshell_parser.h"
#include "stddef.h"
#include <stdlib.h>

/*
 * Noah Cherry
 * January 23 2022
 * EC440
 * Support and testing for provided data structures    
 */

void link_pipe(struct pipeline *pl)
{
  int n = 100;
  struct pipeline_command *prev_command_ptr;

  for(int i = 0; i < n; i++){
    struct pipeline_command *cmd = malloc(sizeof(struct pipeline_command));
    cmd->command_args[0] = "ls";
    
    //set the next pointer of the previous command to the current command
    if (i == 0) {
      pl->commands = cmd;
    } else {
      prev_command_ptr->next = cmd;
    }
    prev_command_ptr = cmd;    
  }
  return;
}

void free_pipe(struct pipeline *pl)
{
  struct pipeline_command *free_ptr;
  
  link_pipe(&pl);
  
  //clear pointers
  for(int i = 0; i < 100; i++){
    free_ptr = pl->commands;
    pl.commands = pl->commands->next;
    free(free_ptr);
  }
  return;
}

int main()
{
  struct pipeline pl;
  struct pipeline_command *free_ptr;

  //connect n commands in a pipe
  link_pipe(&pl);
  
  //free mem in pipe
  free_pipe(&pl);

  return 0;
}
