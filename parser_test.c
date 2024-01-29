#include <stdio.h>
#include <string.h>
#include "myshell_parser.h"
#include "stddef.h"

/*
void parse_redirect(char *command, char *inner_delim, char *outer_delim)
{
  return;
}
*/


int main(int argc, char* argv[])
{
  const char pipe_delim[] = "|";
  const char redirect_in_delim[] = "<";
  const char redirect_out_delim[] = ">";
  //const char arg_delim[] = " ";

  char *token;
  char *pipe_saveptr = NULL;
  char *redirect_in_saveptr = NULL;
  char *redirect_out_saveptr = NULL;
  //char *arg_delim_saveptr = NULL;

  token = strtok_r(argv[1], pipe_delim, &pipe_saveptr);

  while (token != NULL) {
    printf("Pipe Split: %s\n", token);

    char *redirect_in_token = strtok_r(token, redirect_in_delim, &redirect_in_saveptr);
    char *redirect_out_token = strtok_r(token, redirect_out_delim, &redirect_out_saveptr);
    while (redirect_in_token != NULL){
      printf("REDIRECTING IN: %s\n", redirect_in_token);
      redirect_in_token = strtok_r(NULL, redirect_in_delim, &redirect_in_saveptr);
    }

    while (redirect_out_token != NULL){
      printf("REDIRECTING OUT: %s\n", redirect_out_token);
      redirect_out_token = strtok_r(NULL, redirect_out_delim, &redirect_out_saveptr);
    }

    token = strtok_r(NULL, pipe_delim, &pipe_saveptr);
  }
  return 0;
}
