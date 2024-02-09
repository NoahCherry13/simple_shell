#include "myshell_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int main(void)
{
  struct pipeline* my_pipeline = pipeline_build("ls arg1 < file1 > file2 -l| cat helloworld\n");
  
  // Test that a pipeline was returned
  assert(my_pipeline != NULL);
  assert(!my_pipeline->is_background);
  assert(my_pipeline->commands != NULL);
  
  // Test the parsed args
  assert(strcmp("ls", my_pipeline->commands->command_args[0]) == 0);
  assert(strcmp(my_pipeline->commands->command_args[1], "arg1") == 0);
  assert(strcmp(my_pipeline->commands->command_args[2], "-l") == 0);
  assert(strcmp("cat", my_pipeline->commands->next->command_args[0]) == 0);
  assert(strcmp(my_pipeline->commands->next->command_args[1], "helloworld") == 0);
    
  // Test the redirect state
  assert(strcmp(my_pipeline->commands->redirect_in_path, "file1") == 0);
  assert(strcmp(my_pipeline->commands->redirect_out_path, "file2") == 0);
  
  // Test that there are multiple parsed command in the pipeline
  assert(my_pipeline->commands->next != NULL);
  
  // keep going... what should we be testign next?
  pipeline_free(my_pipeline);
}
