

#pragma once

#include "headers.h"
#include "stack.h"
#include "SJF.h"
#include "hpf.h"
#include "MLFQ.h"
#include "RR.h"

int previousTimeUsage; // Time when we last utilized the CPU
int idleTime = 0;      // Time CPU was idle in, initially = 0

int waitingTime = 0; // waiting time of processes, initially = 0
int numProc = 0;     // number of processes, initially = 0
double avgWTA = 0;   // average weighted turnaround time, initially = 0

scheduling_algo algo;
FILE *outputStats;    // File pointer to the output file, where stats will be written
FILE *memOutputStats; // File pointer to the memory output file, where stats will be written

void schedulerIsForContextSwitch();
void schedulerAreDead(int SIGNUM);
void schedulerPreempt(process *proc);
void schedulerResume(process *proc);


void childAreDead(int SIGNUM)
{

    // getting finished process
    int procStatus;
    int procID = wait(&procStatus);
    if (WIFEXITED(procStatus))
    {
        int exitCode = WEXITSTATUS(procStatus);
    }
    printf("delete process %d\n", procID);
    fflush(stdout);

    process *proc = stackGetProcess(procID);
    algo.removeProcess(algo.type, proc); // removing process from algorithm queue

    numProc++; // increasing number of processes
    int turnaroundTime = getClk() - proc->arrival;
    double weightedTurnaroundTime = turnaroundTime / (double)proc->runtime;

    // double avgWeightedTurnaroundTime = avgWTA;  // temporary holder
    avgWTA = (weightedTurnaroundTime + (numProc - 1) * avgWTA) / numProc;
    // Method of calucation: similar to that of calculating the GPA
    // getting the total WTA by multiplying the number of processes (excluding the terminating one)
    // by the avgWTA. Then we add the new WTA, and divide by the total number of processes.

    waitingTime += proc->waitingtime;
    previousTimeUsage = getClk(); // updating the utilization time
    if (proc->remainingtime < 0)
        proc->remainingtime = 0;
    // Printing process info
    fprintf(outputStats, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n",
            getClk(), proc->id, proc->arrival, proc->runtime, proc->remainingtime, proc->waitingtime, turnaroundTime, weightedTurnaroundTime);
    fflush(outputStats);
    stackRemove(proc); // removing from process control block
    curentProcess = NULL;
    printf("delete process %d\n", procID);
    fflush(stdout);
    schedulerIsForContextSwitch();
}

bool schedulerInitialize(int algo_num, int *msgq_id)
{

    // Opening the file where we will output the process updates
    outputStats = fopen("scheduler.log", "w");

    // Printing the opening statement
    fprintf(outputStats, "#At time x process y state arr w total z remain y wait k\n");
    fflush(outputStats);

    /* sigaction to handle SIGCHLD signal
       whenever a process dies, it calls childAreDead
       (sigaction since SIGCHLD cannot be handled directly)
    */
    struct sigaction chld_handle;
    chld_handle.sa_handler = childAreDead;
    sigemptyset(&chld_handle.sa_mask);
    chld_handle.sa_flags = SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &chld_handle, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }

    signal(SIGINT, schedulerAreDead);

    initClk();
    previousTimeUsage = 0;

    // Calling the initializer of each algorithm
    // SJF
    if (algo_num == 1)
    {
        SJFflag = 1;
        SJFInitialize(&algo);
    }
    // HPF
    else if (algo_num == 2)
    {

        HPFInitialize(&algo);
    }
    // RR
    else if (algo_num == 3)
    {
        RRInitialize(&algo);
    }
    // multilevel
    else if (algo_num == 4)
    {
        MLFQ_init(&algo);
    }

    stackInitialize(200);

    // Creating the message queue
    key_t key_id = ftok("keyfile", 65);

    int msgq_idTemp = msgget(key_id, 0666 | IPC_CREAT);
    if (msgq_idTemp == -1)
    {
        printf("Error in create msgq_idTemp");
        fflush(stdout);
        return false;
    }
    *msgq_id = msgq_idTemp;

    return true;
}

bool getMessage(int msgq_id, msgBuf *message)
{

    // Doesn't wait for the process generator to send message
    int rec_val = msgrcv(msgq_id, message, sizeof(process), 0, IPC_NOWAIT);
    if (rec_val == -1)
    {
        /*     if (errno != ENOMSG) {
          perror("Error in message queue\n");
          exit(-1);
                } */
        return false;
    }
    return true;
}

void schedulerPreempt(process *proc)
{
    printf("Preempt \n");
    fflush(stdout);
    if (!proc)
        return;
    printf("Preempt 2 %d\n", proc->pid);
    fflush(stdout);
    // printf("Prempted process %d\n",curentProcess->id);
    proc->status = 3;         // Stopped process
    proc->StartedBefore = 1;  // Marking the process as ran before
    kill(proc->pid, SIGTSTP); // Stopping the process

    // Printing process info
    fprintf(outputStats, "At time %d process %d stopped arr %d total %d remain %d wait %d\n",
            getClk(), proc->id, proc->arrival, proc->runtime, proc->remainingtime, proc->waitingtime);

    fflush(outputStats);

    previousTimeUsage = getClk(); // starting from now, till another process utilizes the CPU
                                  // that will be the idle time.
}

void schedulerResume(process *proc)
{
    printf("resume\n");
    fflush(stdout);
    // Setting the current running process
    curentProcess = proc;
    if (!proc)
        return;
    printf("resume %d\n", proc->pid);
    fflush(stdout);
    proc->status = 2; // Marking the status as running

    // Continue the process, SIGCONT to update waiting times once the process is resumed
    kill(proc->pid, SIGCONT);

    // Printing process info

    // Remember:
    // int StartedBefore; // 1 means it was started before, 0 means it has never been started before

    bool FirstTime = false;       // a flag that shows whether process is resuming or starting
                                  // false means resuming
    if (proc->StartedBefore == 0) // then it is now starting
    {
        FirstTime = true;
    }

    fprintf(outputStats, "At time %d process %d %s arr %d total %d remain %d wait %d\n",
            getClk(), proc->id, (FirstTime) ? "started" : "resumed", proc->arrival, proc->runtime, proc->remainingtime, proc->waitingtime);
    fflush(outputStats);

    // Updating the idleTime for the scheduler
    idleTime += getClk() - previousTimeUsage;
    previousTimeUsage = getClk();
}

void schedulerIsForContextSwitch()
{
    printf("context\n");
    fflush(stdout);
    bool check = algo.preempt(algo.type);

    if (check)
    {
        schedulerPreempt(curentProcess);

        process *next = algo.getNextProcess(algo.type);
        schedulerResume(next);
    }
}
void CreateProcess(msgBuf *msg_buffer)
{

    int pid = fork();
    // Child (process)
    if (pid == 0)
    {
        char pRemainingTime[10];
        sprintf(pRemainingTime, "%d", msg_buffer->proc.remainingtime);
        execl("process.out", "process.out", pRemainingTime, (char *)NULL);
    }
    else
    {

        msg_buffer->proc.pid = pid;

        // Making the process stopped (once it enters) and check after if ready
        kill(pid, SIGTSTP);

        // insert in the stack
        process *stackProcessEntry = stackInsert(&msg_buffer->proc);

        // Using the chosen algorithm on the created process
        algo.addProcess(algo.type, stackProcessEntry);
        printf("Created process %d\n", msg_buffer->proc.id);
        fflush(stdout);
        schedulerIsForContextSwitch();
    }
}

void schedulerAreDead(int SIGNUM)
{
    printf("schedulerAreDead \n");

    // closing the opened file
    fclose(outputStats);
    printf("schedulerAreDead \n");
    fflush(stdout);
    // Calculating the utilization stats
    outputStats = fopen("schedulerPerf.perf", "w");
    double utilization = 1 - ((double)idleTime / getClk()); // calculates the utilization percentage
    double avgWaitingTime = (double)waitingTime / numProc;

    // Printing the utilization stats in another file
    fprintf(outputStats, "CPU utilization = %.2f%%\n", utilization * 100);
    fprintf(outputStats, "Avg WTA = %.2f\n", avgWTA);
    fprintf(outputStats, "Avg Waiting = %.2f\n", avgWaitingTime);
    fflush(outputStats);
    fclose(outputStats);

    // TODO: free the rest of the stuff here
    stackFree();
    algo.free(algo.type); // free current algorithm

    // Removing message queue
    key_t key_id = ftok("keyfile", 65);

    int msgq_id = msgget(key_id, 0666 | IPC_CREAT);
    msgctl(msgq_id, IPC_RMID, (struct msqid_ds *)0);

    // Installing the signal handler again to trap SIGINT again
    signal(SIGINT, schedulerAreDead);
}