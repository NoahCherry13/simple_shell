#include <stdio.h>
#include <string.h>

int main()
{
  char string[] = "hello                           a world";
  char *token;

  token = strtok(string, " ");

  while (token) {
    puts(token);
    token = strtok(NULL, " ");
  }
  
  return 0;
}

