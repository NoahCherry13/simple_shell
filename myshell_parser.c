#include "myshell_parser.h"
#include "stddef.h"
#include <stdio.h>
#include <string.h>





void get_args(char *token, const char *delim, struct pipeline_command *cmd, int arg_num)
{
  char *outer_saveptr = NULL;
  char *arg_saveptr = NULL;
  char *outer_token = strtok_r(token, "<>", &outer_saveptr);

  while (outer_token != NULL) {
    char *arg_tokem = strtok_r(outer_token, " -", &outer_saveptr);
    arg_token = strtok_r(NULL, " -", &arg_saveptr);
    while (arg_token != NULL){
      cmd->command_args[arg_num] = arg_token;
      arg_token = strtok_r(NULL, " -", &arg_saveptr);
    }
    outer_token = strtok_r(NULL, "<>", &outer_saveptr);
  }
  return;
}

void check_redirect(char *token, const char *delim, struct pipeline_command *cmd)
{
  int arg_num;
  char *redirect_saveptr = NULL;
  char *redirect_token = strtok_r(token, "delim", &redirect_saveptr);
  redirect_token = strtok_r(NULL, " &<>", &redirect_saveptr);
  
  if (delim[0] == '<') {
    cmd->redirect_in_path = str_dup(redirect_token);
  } else {
    cmd->redirect_out_path = str_dup(redirect_token);
  }
}

struct pipeline *pipeline_build(const char *command_line)
{
  // TODO: Implement this function
  const char pipe_delim[] = "|";
  const char redirect_in_delim[] = "<";
  const char redirect_out_delim[] = ">";
  const char arg_delims[] = " -"; 

  struct pipeline pl;
  char *pipe_saveptr = NULL;
  char *token = strtok_r(command_line, pipe_delim, &pipe_saveptr);

  while (token != NULL) {
    arg_num = 0;
    char *rin_cpy = strdup(token);
    char *rout_cpy = strdup(token);
    char *arg_cpy = strdup(token);

    check_redirect(rin_cpy, redirect_in_delim, &arg_num);
    check_redirect(rout_cpy, redirect_out_delim, &arg_num);
    get_args(arg_cpy, arg_delim);

    free(rin_cpy);
    free(rout_cpy);
    free(arg_cpy);

    token = strtok_r(NULL, pipe_delim, &pipe_saveptr);
  }
  return NULL;
}

void pipeline_free(struct pipeline *pipeline)
{
	// TODO: Implement this function
}
