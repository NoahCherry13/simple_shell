#include "myshell_parser.h"
#include "stddef.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>




void get_args(char *token, const char *delim, struct pipeline_command *cmd, int *arg_num)
{
  char *outer_saveptr = NULL;
  char *arg_saveptr = NULL;
  char *outer_token = strtok_r(token, "<>", &outer_saveptr);
  int is_cmd = 1;
  
  while (outer_token != NULL) {
    char *arg_token = strtok_r(outer_token, " ", &arg_saveptr);

    if (is_cmd) {
      cmd->command_args[0] = strdup(arg_token);
    }
    
    arg_token = strtok_r(NULL, " ", &arg_saveptr);
    while (arg_token != NULL){
      cmd->command_args[*arg_num] = arg_token;
      *arg_num = *arg_num+1;
      arg_token = strtok_r(NULL, " ", &arg_saveptr);
    }
    outer_token = strtok_r(NULL, "<>", &outer_saveptr);
  }

  return;
}





void check_redirect(char *token, const char *delim, struct pipeline_command *cmd)
{
  char *redirect_saveptr = NULL;
  char *redirect_token = strtok_r(token, "delim", &redirect_saveptr);
  redirect_token = strtok_r(NULL, " &<>", &redirect_saveptr);

  if (redirect_token == NULL){
    cmd->redirect_in_path = NULL;
    cmd->redirect_out_path = NULL;
  } else if (delim[0] == '<') {
    cmd->redirect_in_path = redirect_token;
  } else {
    cmd->redirect_out_path = redirect_token;
  }
  return;
}





struct pipeline *pipeline_build(const char *command_line)
{
  // TODO: Implement this function
  const char pipe_delim[] = "|";
  const char redirect_in_delim[] = "<";
  const char redirect_out_delim[] = ">";
  const char arg_delims[] = " -"; 

  char *command_line_cpy = strdup(command_line);
  char *pipe_saveptr = NULL;
  struct pipeline *pl = malloc(sizeof(struct pipeline));
  int arg_num = 1;
  command_line_cpy[strlen(command_line_cpy)-1] = '\0';
  char *token = strtok_r(command_line_cpy, pipe_delim, &pipe_saveptr);

  
  while (token != NULL) {
    arg_num = 0;
    char *rin_cpy = strdup(token);
    char *rout_cpy = strdup(token);
    char *arg_cpy = strdup(token);
    struct pipeline_command *cmd = malloc(sizeof(struct pipeline_command));

    if(pl->commands == NULL){
      pl->commands = cmd;
    } else {
      struct pipeline_command *cmd_ptr = pl->commands;
      while(cmd_ptr->next != NULL){
	cmd_ptr = cmd_ptr->next;
      }
      cmd_ptr->next = cmd;
    }

    check_redirect(rin_cpy, redirect_in_delim, cmd);
    check_redirect(rout_cpy, redirect_out_delim, cmd);
    get_args(arg_cpy, arg_delims, cmd, &arg_num);

    free(rin_cpy);
    free(rout_cpy);
    free(arg_cpy);

    token = strtok_r(NULL, pipe_delim, &pipe_saveptr);
  }
  
  free(command_line_cpy);
  return pl;
}

void pipeline_free(struct pipeline *pipeline)
{
  struct pipeline_command *free_ptr;
  while (pipeline->commands->next != NULL) {
    free_ptr = pipeline->commands;
    pipeline->commands = pipeline->commands->next;
    free(free_ptr);
  }
  free(pipeline);
}
