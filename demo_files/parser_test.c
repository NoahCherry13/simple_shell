#include <stdio.h>
#include <string.h>
#include "myshell_parser.h"
#include "stddef.h"
#include <stdlib.h>

void get_args(char *token, const char *delim)
{
  char *outer_saveptr = NULL;
  char *arg_saveptr = NULL;

  char *outer_token = strtok_r(token, "<>", &outer_saveptr);

  while (outer_token != NULL) {
    char *arg_token = strtok_r(outer_token, " -", &arg_saveptr);
    while (arg_token != NULL){
      printf("ARGS: %s\n", arg_token);
      arg_token = strtok_r(NULL, " -", &arg_saveptr);
    }
    outer_token = strtok_r(NULL, "<>", &outer_saveptr);
  }
  return;
}

void check_redirect(char *token, const char *delim)
{
  char *redirect_saveptr = NULL;
  char *redirect_token = strtok_r(token, delim, &redirect_saveptr);
  redirect_token = strtok_r(NULL, " &<>", &redirect_saveptr);

  if (delim[0] == '<') {
    printf("REDIRECTING IN: %s\n", redirect_token);
  } else {
    printf("REDIRECTING OUT: %s\n", redirect_token);
  }
}

int main(int argc, char* argv[])
{
  printf("COMMAND: %s\n", argv[1]);
  const char pipe_delim[] = "|";
  const char redirect_in_delim[] = "<";
  const char redirect_out_delim[] = ">";
  char *pipe_saveptr = NULL;
  char *token = strtok_r(argv[1], pipe_delim, &pipe_saveptr);

  while (token != NULL) {
    printf("Pipe Split: %s\n", token);
    char *rin_cpy = strdup(token);
    char *rout_cpy = strdup(token);
    char *arg_cpy = strdup(token);

    check_redirect(rin_cpy, redirect_in_delim);
    check_redirect(rout_cpy, redirect_out_delim);
    get_args(arg_cpy, " -");
    
    free(rin_cpy);
    free(rout_cpy);
    free(arg_cpy);
    
    token = strtok_r(NULL, pipe_delim, &pipe_saveptr);
  }
  
  return 0;
}
