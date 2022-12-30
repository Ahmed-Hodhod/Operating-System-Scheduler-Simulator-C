#ifndef CIRCULARQ_H
#define CIRCULARQ_H

#include "headers.h"

typedef struct circularQ
{
  int front;
  int back;
  int size;     // maximum capacity of Q
  int occupied; // actual number of entries
  process **entries;
} circularQ;

// Default intializer
// circularQ circularQueueDefault = {0, 0, 0, 0, NULL};

// A function the returns the next index
int circularQForward(circularQ *queue, int i)
{
  return (i + queue->size - 1) % queue->size;
}

void circularQprint(circularQ *queue)
{
  int size = queue->occupied;
  for (int i = 0; i < size; i++)
  {
    printf("%d\n", queue->entries[i]->id);
    fflush(stdout);
  }
}

// A function the returns the previous index
int circularQBackward(circularQ *queue, int i)
{
  return (i + 1) % queue->size;
}

// A function that creats a queue for the entries
//-1 for faild attempt and 0 for success
int circularQInit(circularQ *queue, int size)
{
  if (!queue || !size)
    return -1;
  if (queue->entries)
    return -1;

  queue->size = size;
  // calloc for dynamic allocation and intializing all entries with (0)
  queue->entries = (process **)calloc(queue->size, sizeof(process *));
  return 0;
}

// A function that frees the dynamically allocated memory
//-1 for faild attempt and 0 for success
int circularQFree(circularQ *queue)
{
  if (!queue)
    return -1;
  if (!queue->entries)
    return -1;
  // free() is to free the memory allocation
  free(queue->entries);
  return 0;
}

// A function that returns a pointer to the front entry
// returns NULL on failure
process *circularQFront(circularQ *queue)
{
  if (!queue)
    return NULL;
  if (!queue->entries)
    return NULL;

  // Queue is empty
  if (!queue->occupied)
    return NULL;

  return queue->entries[queue->front];
}

// A function that enqueues and returns a pointer to the new front entry
// returns NULL on failure
int circularQEnqueue(circularQ *queue, process *p)
{
  if (!queue)
    return -1;
  if (!queue->entries)
    return -1;

  // Queue is full
  if (queue->occupied == queue->size)
    return -1;
  queue->entries[queue->back] = p;
  // Move back circularly
  queue->back = circularQBackward(queue, queue->back);
  queue->occupied = queue->occupied < queue->size ? queue->occupied + 1 : queue->size;

  return 0;
}

// A function that dequeues and returns a pointer to the new front entry
// returns NULL on failure
process *circularQDequeue(circularQ *queue)
{
  if (!queue)
    return NULL;
  if (!queue->entries)
    return NULL;

  // Queue is empty
  if (!queue->occupied)
    return NULL;

  // Remove front element
  process *p = queue->entries[queue->front];
  // Move back circularly
  queue->front = circularQBackward(queue, queue->front);
  queue->occupied = queue->occupied ? queue->occupied - 1 : 0;

  return p;
}

// A function that removes a process entry
// returns NULL on failure
process *circularQRemove(circularQ *queue, process *p)
{
  if (!queue)
    return NULL;
  if (!queue->entries)
    return NULL;

  // Queue is empty
  if (!queue->occupied)
    return NULL;

  // Searching for P on step before back
  int i = circularQForward(queue, queue->back);
  for (; i != queue->back; i = circularQForward(queue, i))
  {
    if (queue->entries[i] == p)
      break;
  }
  if (i == queue->back)
    return NULL;

  for (; i != queue->back; i = circularQBackward(queue, i))
  {
    queue->entries[i] = queue->entries[circularQBackward(queue, i)];
  }

  // Move forward circularly
  queue->back = circularQForward(queue, queue->back);
  queue->occupied = queue->occupied ? queue->occupied - 1 : 0;
  return p;
}

#endif