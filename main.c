#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#define NUMBER_OF_TEAMS 4
#define THREADS_PER_TEAM 3

void *running() {
    sleep(10);
    pthread_exit(0);
}

void sig_func(int signum) {
    printf("I am Thread %d and I caught Signal number %d.\n",(int)pthread_self(),signum);
}

int main() {
    
    pthread_t tid[NUMBER_OF_TEAMS][THREADS_PER_TEAM];
    signal(SIGINT,sig_func);
    signal(SIGABRT,sig_func);
    signal(SIGILL,sig_func);
    
    for(int i=0;i<NUMBER_OF_TEAMS;i++) {
        for(int j=0;j<THREADS_PER_TEAM;j++) {
            pthread_create(&tid[i][j],0,running,NULL);
        }
    }
    
    sleep(3);
    
    for(int i=0;i<NUMBER_OF_TEAMS;i++) {
        for(int j=0;j<THREADS_PER_TEAM;j++) {
            pthread_kill(tid[i][j],SIGINT);
            pthread_kill(tid[i][j],SIGABRT);
            pthread_kill(tid[i][j],SIGILL);
        }
    }
    
    sleep(3);
    
    for(int i=0;i<NUMBER_OF_TEAMS;i++) {
        for(int j=0;j<THREADS_PER_TEAM;j++) {
            pthread_join(tid[i][j],NULL);
        }
    }
}
