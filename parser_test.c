#include <stdio.h>
#include <string.h>
#include "myshell_parser.h"
#include "stddef.h"

int main()
{
  char string[] = "";
  char *token = strtok(string, " ");
  
  while (token) {
    puts(token);
    token = strtok(NULL, " ");
  }
  
  return 0;
}

