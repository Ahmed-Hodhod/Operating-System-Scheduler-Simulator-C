#pragma once

#include "headers.h"

struct stack{
    int size;       // actual used sized
    int capacity;   // maximum size available for usage
    process **array;
} stack;

void stackInitialize(int inputSize) {

    if(!inputSize || inputSize<1) inputSize = 200;
    if(inputSize<200) inputSize = 200;

    stack.array = malloc(inputSize*sizeof(process));
    stack.size = 0;
    stack.capacity = inputSize;

}

process* stackInsert(process* p) {

    if(stack.size == stack.capacity) {
        stack.capacity = stack.capacity * 2;
        stack.array = realloc(stack.array,stack.capacity*sizeof(process));
    }
    process* to_insert = (process*) malloc(sizeof(process));
    *to_insert = *p;

    stack.array[stack.size++] = to_insert;
    return to_insert;

}

bool stackRemove(process* p) {
    int id = p->id;
    process* remove = NULL;

    for(int i=0;i<stack.size;i++) {
        if(stack.array[i]->id == id) {
            remove = stack.array[i];
            stack.array[i] = stack.array[stack.size-1];
            stack.size--;
            break;
        }
    }
    if(remove) {
        free(remove);
        return 1;
    }
    return 0;
}

void stackRefresh() {

    for(int i=0;i<stack.size;i++) {
        if(stack.array[i]->status == 1 || stack.array[i]->status==2) { //running process
            stack.array[i]->remainingtime--;
        }
        else if(stack.array[i]->status == 3) { //waiting process
            stack.array[i]->waitingtime++;
        }
        else if(stack.array[i]->status == 4) {
            stackRemove(stack.array[i]);
        }
    }

}

process* stackGetProcess(int id) {
    process *p = NULL;
    
    for(int i=0;i<stack.size;i++) {
        if(stack.array[i]->pid == id) {
            p = stack.array[i];
            break;
        }
    }

    // if no process with such ID exists, NULL is returned (must check on NULL)
    return p;
}

bool stackFree() {

    for(int i=0;i<stack.size;i++) free(stack.array[i]);

    free(stack.array);

    return 1;
}