#include <string.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
  char *token = NULL;
  char *saveptr = NULL;
  token = strtok_r(argv[1], " &<>|", &saveptr);
  if (!strcmp(saveptr,"&")) puts("FOUND &");
  puts(token);
  puts(saveptr)
  return 0;
}
