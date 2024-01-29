!#/bin/bash

echo "______________TWO COMMANDS PIPE_________________"
./parser_test "command1|command2"
echo "______________ONE COMMAND ONE REDIRECT IN_________________"
./parser_test "command1 < file1"
echo "______________ONE COMMAND ONE REDIRECT OUT_________________"
./parser_test "command1 > file1"
echo "______________TWO COMMANDS PIPE REDIRECT IN REDIRECT OUT_________________"
./parser_test "command1 < file1|command2> file2"
echo "______________ONE COMMAND REDIRECT IN REDIRECT OUT_________________"
./parser_test "command1<file1>file2"
echo "______________TWO COMMANDS PIPE 1 REDIRECT IN 2 REDIRECT OUT_________________"
./parser_test "command1 < file1>file2|command2> file2"
