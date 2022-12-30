#include "headers.h"

/* Modify this file as needed*/
// Variables needed for the time info
int remaining_time,run_time,wait_time = 0,start_time,sleep_time = 0;

// Overwriting the SIGTSTP
void stopProcess(int signum){
    sleep_time = getClk();
    raise(SIGSTOP);

    // assuring the handler is set
    signal(SIGTSTP,stopProcess);
}

// Overwriting the SIGCONT
void resumeProcess(int signum){
    wait_time += getClk() - sleep_time;

    // assuring the handler is set
    signal(SIGCONT,resumeProcess);
}

int main(int agrc, char *argv[])
{
    // Setting handlers
    signal(SIGTSTP,stopProcess);
    signal(SIGCONT,resumeProcess);
    
    initClk();
    //TODO The process needs to get the remaining time from somewhere
    start_time = getClk();
    run_time = atoi(argv[1]);
    printf("Runtime %d\n",run_time);
    fflush(stdout);
    remaining_time = run_time;
    printf("remaining %d\n",remaining_time);
    fflush(stdout);
    while (remaining_time > 0)
    {
        //printf("I am process, remaining %d\n",remaining_time);
        remaining_time = run_time - (getClk() - start_time - wait_time);
    }
    // testing termination from inside process
    int end = getClk();
    

    destroyClk(false);
    printf("Exit\n");
    fflush(stdout);
    exit(0);
    return 0;
}
