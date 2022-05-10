CC = gcc

all: Action1

Action1: Action1.c
    $(CC) -pthread -o Action1 Action1.c

clean:
    rm -f Action1
