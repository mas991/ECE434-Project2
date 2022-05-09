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
    
    void (*oldHandlers[8])(int);
    
    oldHandlers[0] = signal(SIGINT,sig_func);
    oldHandlers[1] = signal(SIGABRT,sig_func);
    oldHandlers[2] = signal(SIGILL,sig_func);
    oldHandlers[3] = signal(SIGCHLD,SIG_IGN);
    oldHandlers[4] = signal(SIGSEGV,SIG_IGN);
    oldHandlers[5] = signal(SIGFPE,SIG_IGN);
    oldHandlers[6] = signal(SIGHUP,SIG_IGN);
    oldHandlers[7] = signal(SIGTSTP,SIG_IGN);
    
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
            pthread_kill(tid[i][j],SIGCHLD);
            pthread_kill(tid[i][j],SIGSEGV);
            pthread_kill(tid[i][j],SIGFPE);
            pthread_kill(tid[i][j],SIGHUP);
            pthread_kill(tid[i][j],SIGTSTP);
        }
    }
    
    sleep(3);
    
    for(int i=0;i<NUMBER_OF_TEAMS;i++) {
        for(int j=0;j<THREADS_PER_TEAM;j++) {
            pthread_join(tid[i][j],NULL);
        }
    }
    
    signal(SIGINT,oldHandlers[0]);
    signal(SIGABRT,oldHandlers[1]);
    signal(SIGILL,oldHandlers[2]);
    signal(SIGCHLD,oldHandlers[3]);
    signal(SIGSEGV,oldHandlers[4]);
    signal(SIGFPE,oldHandlers[5]);
    signal(SIGHUP,oldHandlers[6]);
    signal(SIGTSTP,oldHandlers[7]);
    
    exit(1);
}
