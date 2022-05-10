#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#define NUMBER_OF_TEAMS 4
#define THREADS_PER_TEAM 3

pthread_t tid[NUMBER_OF_TEAMS][THREADS_PER_TEAM]; //Array of TIDs
int signalNum[4]; //Signal caught by a team

void *running() { //Default function run by each thread
    pthread_t my_tid = pthread_self(); //Determine Team Number
    int teamNum = 0;
    
    for(int i=0;i<NUMBER_OF_TEAMS;i++) {
        for(int j=0;j<THREADS_PER_TEAM;j++) {
            if(tid[i][j] == my_tid) {
                teamNum = i+1;
            }
        }
    }
    
    sigset_t mask; //Determine which signals to block, depending on team number
    sigemptyset(&mask);
    
    if(teamNum == 1) {
        sigaddset(&mask,SIGCHLD);
        sigaddset(&mask,SIGSEGV);
        sigaddset(&mask,SIGFPE);
        sigaddset(&mask,SIGHUP);
        sigaddset(&mask,SIGTSTP);
    }
    else if(teamNum == 2) {
        sigaddset(&mask,SIGINT);
        sigaddset(&mask,SIGABRT);
        sigaddset(&mask,SIGILL);
        sigaddset(&mask,SIGHUP);
        sigaddset(&mask,SIGTSTP);
    }
    else if(teamNum == 3) {
        sigaddset(&mask,SIGABRT);
        sigaddset(&mask,SIGILL);
        sigaddset(&mask,SIGCHLD);
        sigaddset(&mask,SIGSEGV);
        sigaddset(&mask,SIGFPE);
    }
    else if(teamNum == 4) {
        sigaddset(&mask,SIGINT);
        sigaddset(&mask,SIGABRT);
        sigaddset(&mask,SIGCHLD);
        sigaddset(&mask,SIGFPE);
        sigaddset(&mask,SIGTSTP);
    }
    
    pthread_sigmask(SIG_BLOCK,&mask,NULL); //Block signals according to mask
    sleep(15); //wait for signals
    
    pthread_exit(0);
}

void sig_func(int signum) { // Called when a thread catches a signal
    pthread_t my_tid = pthread_self();
    int teamNum = 0;
    int threadNum = 0;

    for(int i=0;i<NUMBER_OF_TEAMS;i++) { // Determine team and thread number
        for(int j=0;j<THREADS_PER_TEAM;j++) {
            if(tid[i][j] == my_tid) {
                teamNum = i+1;
                threadNum = j+1;
            }
        }
    }
    
    if(teamNum == 0) {
        printf("I am the Main Thread and I caught Signal number %d.\n",signum);
        return;
    }

    signalNum[teamNum-1] = signum; // Notify team members
    for(int i=0;i<THREADS_PER_TEAM;i++) {
        if(i+1 != threadNum) {
            pthread_kill(tid[teamNum-1][i],SIGUSR1);
        }
    }
    
    printf("I am Thread %d (Team %d, Thread %d), and I caught Signal number %d.\n",(int)my_tid,teamNum,threadNum,signum);
}

void sig_func2(int signum) { // Called when a team member catches a signal
    pthread_t my_tid = pthread_self();
    int teamNum = 0;
    int threadNum = 0;
    
    for(int i=0;i<NUMBER_OF_TEAMS;i++) { // Determine team and thread number
        for(int j=0;j<THREADS_PER_TEAM;j++) {
            if(tid[i][j] == my_tid) {
                teamNum = i+1;
                threadNum = j+1;
            }
        }
    }
    
    printf("I am Thread %d (Team %d, Thread %d), and my Teammate caught Signal number %d.\n",(int)my_tid,teamNum,threadNum,signalNum[teamNum-1]);
}

void sig_func3(int sigNum) { // Called when signals are to be unblocked
    pthread_t my_tid = pthread_self();
    int teamNum = 0;
    
    for(int i=0;i<NUMBER_OF_TEAMS;i++) { // Determine team number
        for(int j=0;j<THREADS_PER_TEAM;j++) {
            if(tid[i][j] == my_tid) {
                teamNum = i+1;
            }
        }
    }
    
    sigset_t mask; //Determine signals to unblock, based upon team number
    sigemptyset(&mask);
    
    if(teamNum == 1) {
        sigaddset(&mask,SIGCHLD);
        sigaddset(&mask,SIGSEGV);
        sigaddset(&mask,SIGFPE);
        sigaddset(&mask,SIGHUP);
        sigaddset(&mask,SIGTSTP);
    }
    else if(teamNum == 2) {
        sigaddset(&mask,SIGINT);
        sigaddset(&mask,SIGABRT);
        sigaddset(&mask,SIGILL);
        sigaddset(&mask,SIGHUP);
    }
    else if(teamNum == 3) {
        sigaddset(&mask,SIGABRT);
        sigaddset(&mask,SIGILL);
        sigaddset(&mask,SIGCHLD);
        sigaddset(&mask,SIGSEGV);
        sigaddset(&mask,SIGFPE);
    }
    else if(teamNum == 4) {
        sigaddset(&mask,SIGINT);
        sigaddset(&mask,SIGABRT);
        sigaddset(&mask,SIGCHLD);
        sigaddset(&mask,SIGFPE);
        sigaddset(&mask,SIGTSTP);
    }
    
    pthread_sigmask(SIG_UNBLOCK,&mask,NULL); //Unblock signals
}

int main() {
    
    void (*oldHandlers[10])(int); // Customize signal handlers
    
    oldHandlers[0] = signal(SIGINT,sig_func);
    oldHandlers[1] = signal(SIGABRT,sig_func);
    oldHandlers[2] = signal(SIGILL,sig_func);
    oldHandlers[3] = signal(SIGCHLD,sig_func);
    oldHandlers[4] = signal(SIGSEGV,sig_func);
    oldHandlers[5] = signal(SIGFPE,sig_func);
    oldHandlers[6] = signal(SIGHUP,sig_func);
    oldHandlers[7] = signal(SIGTSTP,sig_func);
    oldHandlers[8] = signal(SIGUSR1,sig_func2);
    oldHandlers[9] = signal(SIGUSR2,sig_func3);
    
    for(int i=0;i<NUMBER_OF_TEAMS;i++) { // Create threads
        for(int j=0;j<THREADS_PER_TEAM;j++) {
            pthread_create(&tid[i][j],0,running,NULL);
        }
    }
    //mainThread = 1;
    sleep(3); // Give time for threads to set up
    
    // TEST SIGNALS HERE
    //Valid Signals
    pthread_kill(tid[0][0],SIGINT);
    pthread_kill(tid[1][1],SIGCHLD);
    pthread_kill(tid[2][2],SIGHUP);
    pthread_kill(tid[3][0],SIGILL);
    
    //Invalid Signals
    pthread_kill(tid[0][0],SIGCHLD);
    pthread_kill(tid[1][0],SIGHUP);
    pthread_kill(tid[2][0],SIGABRT);
    pthread_kill(tid[3][0],SIGINT);
    
    sleep(5); // Give time for user to enter custom signals 
    
    for(int i=0;i<NUMBER_OF_TEAMS;i++) { // Unblock signals
        for(int j=0;j<THREADS_PER_TEAM;j++) {
            pthread_kill(tid[i][j],SIGUSR2);
        }
    }
    
    sleep(3);
    
    // TEST SIGNALS HERE, IF DESIRED
    
    for(int i=0;i<NUMBER_OF_TEAMS;i++) { // Wait for threads to terminate
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
    signal(SIGUSR1,oldHandlers[8]);
    signal(SIGUSR2,oldHandlers[9]);
    
    
    exit(1);
}
