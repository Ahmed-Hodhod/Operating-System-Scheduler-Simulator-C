#include "headers.h"
#include "scheduler.h"
#include "stack.h"

int main(int argc, char *argv[])
{

    int algo = atoi(argv[1]);
    int msgq_id;
    schedulerInitialize(algo, &msgq_id);

    msgBuf msgq_buffer;
    int now, previous_time = -1;
    printf("msgq_id= %d\n", msgq_id);
    fflush(stdout);
    while (1)
    {
        now = getClk();

        /* receive all types of messages */

        if (getMessage(msgq_id, &msgq_buffer) == false)
        {
            // printf("Message not received\n");
        }
        else
        {

            CreateProcess(&msgq_buffer);
            while (getMessage(msgq_id, &msgq_buffer))
            {
                CreateProcess(&msgq_buffer);
            }
            if (now > previous_time)
            {
                stackRefresh();
            }
            schedulerIsForContextSwitch();
            if (now > previous_time)
            {
                previous_time = now;
            }
        }
        /* Each second update pcb and check for context switching */
        if (now > previous_time)
        {
            stackRefresh();
            schedulerIsForContextSwitch();
            previous_time = now;
        }
    }

    // TODO implement the scheduler :)
    // upon termination release the clock resources.

    destroyClk(true);
    return 0;
}
