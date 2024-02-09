override CFLAGS := -std=gnu99 -O0 -Wall -Werror -g -fsanitize=undefined $(CFLAGS) -I.
override LDFLAGS := -fsanitize=undefined -fsanitize=leak $(LDLAGS)
CC = gcc

all: check

myshell_parser.o: myshell.c myshell_parser.h

myshell.o : myshell.c myshell_parser.h

myshell: myshell.o myshell_parser.o

shell_files=./myshell

.PHONY: clean check checkprogs all

checkprogs: $(shell_files)

check: checkprogs

clean:
	rm -f *~ *.o $(shell_files)
