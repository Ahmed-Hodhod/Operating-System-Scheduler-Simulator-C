#include <math.h>
#include "ml.h"

int AllocateMemory(memList *free_list, int memSize); 
void initializeMemoryManager(memList *free_list);
void DeallocateMemory(memList *free_list,int memStart, int memSize);

int BestSlot(int memSize); 

// 0 1 2 3 4  5  6  7   8   9   10
// 1 2 4 8 16 32 64 128 256 512 1024
void initializeMemoryManager(memList *free_list)
{
    // initialze each memory size list
    for (int i = 0; i < 11; i++)
    {
        memList mlist = (memList){
            .head = NULL,
            .tail = NULL,
            .index = i,
        };
        free_list[i] = mlist;
    }

    // initially, the whole memory is a hole [ 1024 bytes]
    AddHole(&free_list[10], 0);
}

int BestSlot(int memSize)
{
    int list_index = ceil(log2(memSize));
    return pow(2, (int)list_index);
}
int AllocateMemory(memList *free_list, int memSize)
{
    // find the appropriate memory slot 
    int list_index = ceil(log2(memSize));
    int memorySlot = pow(2, (int)list_index);

    int i = list_index;

    // find the least slot that suffices the memorySlot
    while (GetMinAddress(&free_list[i]) == -1)
        ++i;
    
    // if i > list_index, divide the larger slots 

    int slot = pow(2, i);
    int k = i;
    int addr = -1;
    //printf("got here\n");
    for (int j = 0; j < i - list_index; j++)
    {
        slot /= 2;
        // printf("slot to be divided : %d\n", slot);
        addr = ExtractFirstAddress(&free_list[k]);
        // divide it into two equally sized blocks

        AddHole(&free_list[k - 1], addr);
        AddHole(&free_list[k - 1], addr + slot);
        k--;
    }
    
    
    // TEST: 
    //display(&free_list[list_index]);
    
    addr= ExtractFirstAddress(&free_list[k]); 
    return addr; 
}

void DeallocateMemory(memList *free_list,int memStart, int memSize)
{
    int list_index = ceil(log2(memSize));
    AddHole(&free_list[list_index], memStart);
    // run the compaction algorithm [ make it circular]
    int JoinAddr; 
    int i = list_index; 
    while(true)
    {
         JoinAddr = compactList(&free_list[i]);
        if(JoinAddr != -1) 
            {
                AddHole(&free_list[i + 1], JoinAddr); 
                i++; 
            }
        else 
            break; 

        
    }

}

// int main()
// {
//     // priQ **free_list = (priQ **)calloc(sizeof(priQ *), 11);

//     // add this line to the scheduler
//     memList free_list[11];

//     initializeMemoryManager(free_list);
   

//    // TEST: Allocate some memory slots
//     // for (int i =0; i < 4; i ++)
//     // {
//     //     int addr =  AllocateMemory(free_list, 256); 
//     //     printf("%d  %d\n", i, addr); 
//     // // addr represents the starting address of a slot = 256 [ assgn it to a process]
//     // }

//     int addr = AllocateMemory(free_list, 8 );
     
//     DeallocateMemory(free_list, 0, 8);

//      addr = AllocateMemory(free_list, 4 );
//         DeallocateMemory(free_list, 0, 4);

//      addr = AllocateMemory(free_list, 128 );
//     printf("addr of process 3: %d\n", addr);

//     display(&free_list[7]); 

//     return 0;
// }