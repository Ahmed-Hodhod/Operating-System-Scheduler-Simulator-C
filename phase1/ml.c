#include <stdio.h>
#include <limits.h>
#include "headers.h"
#include <math.h>

// Represent a node of the singly linked list

typedef struct
{
    int index; // slot size = 2^index
    struct node *head;
    struct node *tail;

} memList;

struct node
{
    int address;// represents the strating address of each hole 
    struct node *next;
};

int GetMinAddress(memList *memlist);
void AddHole(memList *mlist, int address);
int ExtractFirstAddress(memList *memlist);
void SortHoles(memList *memlist);

// AddHole() will add a new node to the list
void AddHole(memList *mlist, int address)
{
    // Create a new node
    struct node *newNode = (struct node *)malloc(sizeof(struct node));
    newNode->address = address;
    newNode->next = NULL;

    // Checks if the list is empty
    if (mlist->head == NULL)
    {
        // If list is empty, both head and tail will point to new node
        mlist->head = newNode;
        mlist->tail = newNode;
    }
    else
    {
        // newNode will be added after tail such that tail's next will point to newNode
        mlist->tail->next = newNode;
        // newNode will become new tail of the list
        mlist->tail = newNode;
    }
    SortHoles(mlist);
}

void deleteNode(memList *mlist, int address)
{
    // Store head node
    struct node *temp = mlist->head, *prev;

    // If head node itself holds the address to be deleted
    if (temp != NULL && temp->address == address)
    {
        mlist->head = temp->next; // Changed head
        free(temp);               // free old head
        return;
    }

    // Search for the address to be deleted, keep track of the
    // previous node as we need to change 'prev->next'
    while (temp != NULL && temp->address != address)
    {
        prev = temp;
        temp = temp->next;
    }

    // If address was not present in linked list
    if (temp == NULL)
        return;

    // Unlink the node from linked list
    prev->next = temp->next;

    free(temp); // Free memory
    SortHoles(mlist);
}

int GetMinAddress(memList *memlist)
{
    int min = INT_MAX;
    struct node *ptr = memlist->head;
    while (ptr != NULL)
    {
        if (min > ptr->address)
            min = ptr->address;
        ptr = ptr->next;
    }
    return min;
}

int ExtractFirstAddress(memList *memlist)
{
    int min = GetMinAddress(memlist);
    deleteNode(memlist, min);
    return min;
}

// display() will display all the nodes present in the list
void display(memList *mlist)
{
    // Node current will point to head
    struct node *current = mlist->head;
    if (mlist->head == NULL)
    {
        printf("List is empty \n");
        return;
    }
    while (current != NULL)
    {
        // Prints each node by incrementing pointer
        printf("%d ", current->address);
        current = current->next;
    }
    printf("\n");
}

void SortHoles(memList *mlist)
{
    int i, j, a;
    struct node *h = mlist->head;

    struct node *temp1;
    struct node *temp2;

    for (temp1 = h; temp1 != NULL; temp1 = temp1->next)
    {
        for (temp2 = temp1->next; temp2 != NULL; temp2 = temp2->next)
        {
            if (temp2->address < temp1->address)
            {
                a = temp1->address;
                temp1->address = temp2->address;
                temp2->address = a;
            }
        }
    }
}


// returns the starting address of the adjacent holes if exist. otherwise, returns -1
int compactList(memList *mlist)
{
    struct node *temp1;
        struct node *temp2;

    for (temp1 = mlist->head; temp1 != NULL; temp1 = temp1->next)
    {
       for (temp2 = temp1->next; temp2 != NULL; temp2 = temp2->next)
        {
            if (temp1->address % (int)(2 * pow(2, mlist->index)) ==0 && temp1->address + pow(2, mlist->index) == temp2->address)
            {
                // compact 
                return temp1->address; 
                //printf("yes:  %d, %d\n", temp1->address,temp2->address);
            }
        }

    }

    return -1; 
}

int main()
{
    memList mlist = (memList){
        .head = NULL,
        .tail = NULL,
        .index = 3,
    };

    //     // Adds address to the list

    AddHole(&mlist, 0);
    AddHole(&mlist, 256);

    AddHole(&mlist, 8);
    int c = compactList(&mlist);
    printf("compact at : %d \n", c ); 

    // int min = ExtractFirstAddress(&mlist);

    //      AddHole(5);
    //      AddHole(4);

    // deleteNode(&mlist, 2);

    //     // Displaying original list
    //     printf("Original list: \n");
    display(&mlist);
    // printf("min : %d\n", smallestElement(&mlist));

    //     // Original list : 9 7 2 5 4

    //     // SortHolesing list
    // SortHolesList(&mlist);

    //     // Displaying SortHolesed list
    //     printf("SortHolesed list: \n");
    //     display();
    //     // SortHolesed list : 2 5 7 9

    //     return 0;
}