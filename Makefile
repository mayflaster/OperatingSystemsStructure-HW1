# Makefile for the smash program
CC = gcc
CFLAGS = -g -Wall
CCLINK = $(CC)
OBJS = smash.o commands.o signals.o job.o
RM = rm -f
# Creating the  executable
smash: $(OBJS)
	$(CCLINK) -o smash $(OBJS)
# Creating the object files
commands.o: commands.c commands.h job.h
smash.o: smash.c commands.h 
signals.o: signals.c signals.h job.h
job.o: job.c job.h

# Cleaning old files before new make
clean:
	$(RM) $(TARGET) *.o *~ "#"* core.*

