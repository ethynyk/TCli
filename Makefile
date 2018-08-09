#!bash/bin
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c = .o)
CC = gcc
INCLUDES = -I./
LIBS = -L./
CCFLAGS = -g -Wall -glibc
LDFLAGS = -lpthread -pthread 

server : $(OBJS)
	$(CC) $^ -o $@ $(INCLUDES) $(LIBS) $(LDFLAGS)

%.o : %.c
	$(CC) -c $< $(CCFLAGS)

all:$(server)
clean:
	$(RM) $(OBJ) server
.PHONY:clean all
