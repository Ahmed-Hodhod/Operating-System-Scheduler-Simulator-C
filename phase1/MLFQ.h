#pragma once
#ifndef MLFQ_H
#define MLFQ_H

#include "priQ.h"

int MLFQ_quanta = 2;
int MLFQ_start;

bool MLFQ_addProcess(void *type, process *proc);
bool MLFQ_preempt(void *type);
process *MLFQ_getNextProcess(void *type);
bool MLFQ_removeProcess(void *type, process *proc);
bool MLFQ_free(void *type);

// Casting function
void MLFQcast(void *type)
{
    // queue = (priQ *) type;
}

// Initialzing the algorithm
bool MLFQ_init(scheduling_algo *curr_algo)
{
    if (!curr_algo)
        return 0;

    priQ *MLFQqueue = (priQ *)malloc(sizeof(priQ));
    // create_priQ(queue,100);
    priQcreate(MLFQqueue, 50);

    *curr_algo = (scheduling_algo){
        MLFQqueue,
        MLFQ_addProcess,
        MLFQ_preempt,
        MLFQ_getNextProcess,
        MLFQ_removeProcess,
        MLFQ_free,
    };
    return 1;
}

// Inserting a process to the priQ
bool MLFQ_addProcess(void *type, process *p)
{
    priQ *queue = (priQ *)type;
    bool success = priQenqueue(queue, p);
    return success;
}

// Returns the right process to run
process *MLFQ_getNextProcess(void *type)
{

    priQ *queue = (priQ *)type;
    process *nextProcess = priQpeek(queue);
    return nextProcess;
}

// Checks if the process should preempt
bool MLFQ_preempt(void *type)
{
    int clk = getClk();
    if (curentProcess == NULL)
    {
        MLFQ_start = clk;
        return true;
    }

    priQ *queue = (priQ *)type;
    // should be ==
    if (clk - MLFQ_start >= MLFQ_quanta)
    {
        MLFQ_start = clk;
        priQremove(queue, curentProcess);
        if (curentProcess->priority != 10)
            curentProcess->priority++;
        if (curentProcess->remainingtime > 0)
            priQenqueue(queue, curentProcess);
        return (queue->capacity > 1) && (curentProcess->remainingtime > 0);
    }

    return false;
    // priQ *queue = (priQ *)type;
    // return curentProcess != priQpeek(queue);
}

// Removes the process from the pri queue
bool MLFQ_removeProcess(void *type, process *p)
{
    priQ *queue = (priQ *)type;

    bool success = priQremove(queue, p);
    return success;
}

// Frees the alocated resources created by the algorithm
bool MLFQ_free(void *type)
{

    priQ *queue = (priQ *)type;
    bool succes = priQfree(queue);
    return succes;
}

#endif