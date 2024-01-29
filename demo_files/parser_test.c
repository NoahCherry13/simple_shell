#include <stdio.h>
#include <string.h>
#include "myshell_parser.h"
#include "stddef.h"
#include <stdlib.h>

void get_args(char *token, const char *delim)
{
  char *arg_saveptr = NULL;
  char *arg_token = strtok_r(token, delim, &arg_saveptr);

  while (arg_token != NULL){
    arg_token = strtok_r(NULL, " -", &arg_saveptr);
    printf("ARGUMENTS: %s\n", arg_token);
  }
  free(arg_token);
}

void check_redirect(char *token, const char *delim)
{
  char *redirect_saveptr = NULL;
  char *redirect_token = strtok_r(token, delim, &redirect_saveptr);
  char *arg_token = strdup(redirect_token);
  //get_args(arg_token, " -");
  redirect_token = strtok_r(NULL, " -&<>", &redirect_saveptr);
  get_args(arg_token, " -");
  if (delim[0] == '<') {
    printf("REDIRECTING IN: %s\n", redirect_token);
  } else {
    printf("REDIRECTING OUT: %s\n", redirect_token);
  }
  free(arg_token);
  /*
  while (redirect_token != NULL){
    redirect_token = strtok_r(NULL, " -&<>", &redirect_saveptr);
    printf("ARGUMENTS: %s\n", redirect_token);
  }
  */
}

int main(int argc, char* argv[])
{
  printf("COMMAND: %s\n", argv[1]);
  const char pipe_delim[] = "|";
  const char redirect_in_delim[] = "<";
  const char redirect_out_delim[] = ">";
  char *pipe_saveptr = NULL;
  char *token = strtok_r(argv[1], pipe_delim, &pipe_saveptr);

  //const char arg_delim[] = " ";
  //char *redirect_in_saveptr = NULL;
  //char *redirect_out_saveptr = NULL;
  //char *arg_delim_saveptr = NULL;
  while (token != NULL) {
    printf("Pipe Split: %s\n", token);
    char *rin_cpy = strdup(token);
    char *rout_cpy = strdup(token);

    check_redirect(rin_cpy, redirect_in_delim);
    check_redirect(rout_cpy, redirect_out_delim);

    /*
     *char *redirect_in_token = strtok_r(rin_cpy, redirect_in_delim, &redirect_in_saveptr);
      *char *redirect_out_token = strtok_r(rout_cpy, redirect_out_delim, &redirect_out_saveptr);
      *while (redirect_in_token != NULL){
      *printf("REDIRECTING IN: %s\n", redirect_in_token);
      *redirect_in_token = strtok_r(NULL, redirect_in_delim, &redirect_in_saveptr);
      *}
      *while (redirect_out_token != NULL){
      *printf("REDIRECTING OUT: %s\n", redirect_out_token);
      *redirect_out_token = strtok_r(NULL, redirect_out_delim, &redirect_out_saveptr);
      *  }
    */

    //free(redirect_in_token);
    //free(redirect_out_token);

    free(rin_cpy);
    free(rout_cpy);
    token = strtok_r(NULL, pipe_delim, &pipe_saveptr);
  }
  
  return 0;
}
