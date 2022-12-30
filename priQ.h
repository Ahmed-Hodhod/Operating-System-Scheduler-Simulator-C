#pragma once
#include "headers.h"

typedef struct priQ {
  int capacity;
  int size;

  process **queue;
} priQ;

/***** UTILS *****/
int parent(int i) {
  return (i-1)/2;
}

int left(int i) {
  return 2*i + 1;
}

int right(int i) {
  return 2*i + 2;
}

bool comparisonSJF(process *x, process* y) {
  if(x->runtime < y->runtime) {
    return true;
  }
  return false;
}

bool comparison(process *x, process* y) {
  //printf("I am flag %d\n",SJFflag);
  if(SJFflag == 1) {
    if(x->remainingtime < y->remainingtime) {
    return true;
    }
    return false;
  }
  if(x->priority < y->priority) {
    return true;
  }
  return false;
}

void swap(process** x, process** y) {
  process* temp = *x;
  *x = *y;
  *y = temp;
}


//TODO - print

void priQprint(priQ *all_process) {
  int size = all_process->size;
  for(int i=0;i<size;i++) {
    printf("%d ",all_process->queue[i]->id);
    fflush(stdout);
  }
  printf("\n");
  fflush(stdout);
}

// 1 for success, 0 for failed (when no queue is passed)
bool priQcreate(priQ *all_process,int capacity) {
  if(!all_process) return 0;
  if(!capacity) capacity=50;

  all_process->size=0;
  all_process->capacity=capacity;
  all_process->queue = (process **)calloc(all_process->capacity, sizeof(process *)); //initialize to zero
  return 1;
}

process* priQpeek(priQ *all_process) {
  if(all_process->size==0) return NULL;
  return all_process->queue[0];
} 

void reheapDown(priQ* all_process, int i) {
  int *size = &all_process->size;
  bool left_ok = left(i) < *size && comparison(all_process->queue[left(i)],all_process->queue[i]);
  bool right_ok = right(i) < *size && comparison(all_process->queue[right(i)],all_process->queue[i]);

  while(i<*size && (left_ok || right_ok)) {
    if (left_ok && right_ok) {
      if (comparison(all_process->queue[left(i)],all_process->queue[right(i)])) {
        swap(&all_process->queue[i], &all_process->queue[left(i)]);
        i = left(i);
      }
      else {
       swap(&all_process->queue[i], &all_process->queue[right(i)]);
        i = right(i);
      }
    }
    else if (left_ok) {
        swap(&all_process->queue[i], &all_process->queue[left(i)]);
        i = left(i);
    }
    else {
        swap(&all_process->queue[i], &all_process->queue[right(i)]);
        i = right(i);
    }

    left_ok = left(i) < *size && comparison(all_process->queue[left(i)],all_process->queue[i]);
    right_ok = right(i) < *size && comparison(all_process->queue[right(i)],all_process->queue[i]);

  }
}

void reheapUp(priQ* all_process) {
  int *size = &all_process->size;

  int i = *size - 1;
  while (i != 0 && comparison(all_process->queue[i],all_process->queue[parent(i)])) {
   swap(&all_process->queue[i], &all_process->queue[parent(i)]);
    i = parent(i);
  }
}

process* priQdequeue(priQ *all_process) {
  if(!all_process || !all_process->queue) return NULL;
  int *size = &all_process->size;
  (*size--);
  process* removed = all_process->queue[0];
  all_process->queue[0] = all_process->queue[*size];
  // reheap down
  reheapDown(all_process,0);
  return removed;
}

bool priQenqueue(priQ* all_process, process* p) {
  if(all_process->capacity==all_process->size) return 0;
  int *size = &all_process->size;
  all_process->queue[*size] = p;
  (*size)++;
  // reheap up
  reheapUp(all_process);
  return 1;
}


bool priQremove(priQ* all_process, process* p) {
  int i;
  int *size = &all_process->size;

  for (i = 0; i < *size; i++) {
    if (all_process->queue[i] == p) {
      swap(&all_process->queue[i], &all_process->queue[*size - 1]);
      (*size)--;
      reheapDown(all_process, i);
      //break;
      return 1;
    }
  }
  return 0;
}

bool priQfree(priQ* all_process) {
  if(!all_process || !all_process->queue) return 0;
  free(all_process->queue);
  return 1;
}
