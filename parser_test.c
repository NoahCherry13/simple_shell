#include <stdio.h>
#include <string.h>
#include "myshell_parser.h"
#include "stddef.h"

int main()
{
  char string[] = "hello                           a world";
  char *token = strtok(string, " ");

  struct pipeline_command testpipe_command;
  testpipe_command.command_args[0] = "ls";
  testpipe_command.command_args[0] = "-a";
  testpipe_command.redirect_in_path = "in.txt";
  testpipe_command.redirect_out_path = "out.txt";

  puts(testpipe_command.command_args[0]);
  
  while (token) {
    puts(token);
    token = strtok(NULL, " ");
  }
  
  return 0;
}

