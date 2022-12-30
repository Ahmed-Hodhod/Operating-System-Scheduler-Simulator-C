#include <math.h>
#include "priQ.h"
#include "headers.h"
#include "ml.c"


int main()
{
    // priQ **free_list = (priQ **)calloc(sizeof(priQ *), 11);

    // add this line to the scheduler 
    memList * free_list[11];

    // seprate this into an initialize function 
    for (int i = 0; i < 11; i++)
    {
        memList mlist = (memList){
            .head = NULL,
            .tail = NULL,
            .index = 3,
        };
        free_list[i] = &mlist;  
    }


    // test with a process
    process p;
    p.id = 4;
    p.arrival = 4;
    p.runtime = 2;
    p.remainingtime = 2;
    p.priority = 0; 
    p.memsize = 256;
    p.memStart = 0;

    
    // initially, there is only one hole [add to initialize]
    AddHole(free_list[10], ); 
    priQenqueue(free_list[10], &p);
    
    // use priQ of proccesses for now, consider priority = address

    int list_index = ceil(log2(p.memsize));
    int memorySlot = pow(2, (int)list_index);

    priQ *list = free_list[list_index];
    int i = list_index;

    while (list->size == 0 && i < 11)
    {
        // printf("%d\n", i);
        list = free_list[++i];
    }

    // now list, holds the first available hole, list_index holds the index
    // printf("log of 512: %d %d \n", list_index, i);
    int slot = pow(2, i);
    process *m;
    int k = i;
    for (int j = 0; j < i - list_index; j++)
    {
        // slot /= 2;
        // printf("slot to be divided : %d\n", slot);
        m = priQpeek(free_list[k]);
        priQremove(free_list[k], m);

        // divide it into two equally sized blocks

        // memStart is not correct [ due to using pointers ]
        process ppp = (process){
            .priority = m->memStart,
            .memStart = m->memStart,
        };
        process pp = (process){
            .priority = m->memStart + slot,
            .memStart = m->memStart + slot,
        };

        priQenqueue(free_list[k - 1], &ppp);
        priQenqueue(free_list[k - 1], &pp);
        k--;
    }
    // now, k holds the index of the block i want to use

    // allocate the kth block
    m = priQpeek(free_list[k]);
    priQremove(free_list[k], m);
    p.memStart = m->priority; // priority represents the location
    // becaue of the error in setting the starting time, set it manually here

    // deallocation algorithm
    // assume the deallocated process keeps the size associated to it
    // enqueue this process into the suitable list based on its memStart value [ address]
    p.memStart = 256;
    p.memsize = 256;
    p.priority = 256;

    // remove this when you have your list
    m = priQpeek(free_list[8]);
    m->memStart = 0;
    m->memsize = 256;
    m->priority = 0;
    printf("first : %d\n", m->memStart);

    // first, find the suitable block

    // remove the below line
    process kk = (process){
        .priority = 256,
        .memsize = 256,
        .memStart = 256,
    };

    priQenqueue(free_list[(int)log2(p.memsize)], &kk);

    // remove the below line
    process kkk = (process){
        .priority = 512,
        .memsize = 512,
        .memStart = 512,
    };

    priQenqueue(free_list[(int)log2(p.memsize)], &kkk);

    // Now, you have two holes having 256[ 0->255->511]

    // m = priQpeek(free_list[(int)log2(p.memsize)]);
    // printf("second : %d\n", m->memStart);

    // Loop over the [(int)log2(p.memsize)]th  list and run the compaction algorithm


        return 0;
    }