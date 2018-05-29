CC = gcc
CFLAGS = -Wall -pedantic -g -lm
all : parseline

parseline : parseline.c parseline.h
	$(CC) $(CFLAGS) -o parseline parseline.c

handin : parseline Makefile
	handin getaylor-grader 357hw5-01 Makefile parseline.c parseline.h README

clean :
	rm parseline
