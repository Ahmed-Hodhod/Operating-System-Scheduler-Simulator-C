#pragma once

#include "priQ.h"

//priQ* queue;

bool HPFaddProcess(void *type, process* p);
bool HPFpreempt(void *type);
process* HPFgetNextProcess(void *type);
bool HPFremoveProcess(void *type, process* p);
bool HPFfree(void *type);

void HPFcast(void *type) {
    //queue = (priQ *) type;
}

bool HPFInitialize(scheduling_algo* current_algorithm) {
    if(!current_algorithm) return 0;

    priQ* HPFqueue = (priQ*)malloc(sizeof(priQ));
    priQcreate(HPFqueue, 50);

    *current_algorithm = (scheduling_algo){
        HPFqueue,
        HPFaddProcess,
        HPFpreempt,
        HPFgetNextProcess,
        HPFremoveProcess,
        HPFfree,
        
    };

    return 1;
}

bool HPFaddProcess(void *type, process* p) {
    priQ *queue = (priQ *)type;
    bool success = priQenqueue(queue,p);
    return success;
}

process* HPFgetNextProcess(void *type) {
    priQ *queue = (priQ *)type;
    process* nextProcess = priQpeek(queue);
    return nextProcess;
}

bool HPFpreempt(void *type) {
    priQ *queue = (priQ *)type;
    if(priQpeek(queue)==NULL)
     return true;
    return curentProcess != priQpeek(queue);
}

bool HPFremoveProcess(void *type, process* p) {
    priQ *queue = (priQ *)type;
    bool success = priQremove(queue,p);
    return success;
}

bool HPFfree(void *type) {
    priQ *queue = (priQ *)type;
    bool succes = priQfree(queue);
    return succes;
}
