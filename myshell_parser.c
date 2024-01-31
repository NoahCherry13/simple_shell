#include "myshell_parser.h"
#include "stddef.h"
#include <stdio.h>
#include <string.h>







void check_redirect(char *token, const char *delim)
{
  char *redirect_saveptr = NULL;
  char *redirect_token = strtok_r(NULL, " <>", &redirect_save_ptr);

  if (delim[0] == '<') {
    
  } else {
    
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
    char *rin_cpy = strdup(token);
    char *rout_cpy = strdup(token);
    char *arg_cpy = strdup(token);

    check_redirect(rin_cpy, redirect_in_delim);
    check_redirect(rout_cpy, redirect_out_delim);
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
