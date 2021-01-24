#!bash/bin
LIBNAME = libcli
SHARED = $(LIBNAME).so
C_SRCS = $(wildcard *.c)
OBJS = $(C_SRCS:.c = .o)
#CC = aarch64-linux-gnu-gcc
CC = gcc
CFLAGS = -g -Wall -fPIC
LDFLAGS = -lpthread -pthread 

C_OBJS = $(C_SRCS:.c=.o)

all: $(SHARED)

$(SHARED): $(C_OBJS) 
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LDFLAGS)
	
clean:
	$(RM) $(C_OBJ) server $(SHARED) *.o
	
.PHONY:clean all
