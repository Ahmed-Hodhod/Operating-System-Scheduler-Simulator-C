#include "headers.h"

void clearResources(int);
int clk_pid;
int scheduler_pid;
int stat_loc;

void forkClkAndScheduler()
{

    clk_pid = fork();
    if (clk_pid < 0)
    {
        printf("Error while forking \n");
        fflush(stdout);
        exit(1);
    }
    // Child(clock proc)
    else if (clk_pid == 0)
    {
        execl("clk.out", "clk.out", (char *)NULL);
    }
    // Parent(process generator)
    else
    {
        scheduler_pid = fork();
        if (scheduler_pid < 0)
        {
            printf("Error while forking \n");
            fflush(stdout);
            exit(1);
        }
        // Child(Scheduler proc)
        else if (scheduler_pid == 0)
        {
            char selectedAlgorithmChar[5];
            sprintf(selectedAlgorithmChar, "%d", selected_algo);
            execl("scheduler.out", "scheduler.out", selectedAlgorithmChar, (char *)NULL);
        }
    }
}
void getAlgorithmFromUser()
{
    printf("Enter the required algorithm \n");
    printf("1. SJF\n2. HPF\n3. RR\n4. MLFQ\n");
    fflush(stdout);

    do
    {
        scanf("%d", &selected_algo);
        if (selected_algo < 1 || selected_algo > 4)
        {
            printf("Invalid choice! Please enter a number from 1 to 4\n");
            fflush(stdout);
        }
    } while (selected_algo < 1 || selected_algo > 4);

    if (selected_algo == 1)
        SJFflag = 1;
}

int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    FILE *file = fopen("processes.txt", "r");

    char line[30];
    int size;
    int d;
    int a;
    // read the number of generated processes
    fscanf(file, "%d", &size);
    process *processes[size];

    // ignore the header line
    fscanf(file, "%s %s %s %s %s", line, line, line, line, line);

    // read each process
    int next_process = 0;
    while (next_process < size)
    {

        int id, arrival, runtime, priority, memsize; // used in reading the input file

        fscanf(file, "%d %d %d %d %d", &id, &arrival, &runtime, &priority, &memsize);

        process *new_process = (process *)malloc(sizeof(process));
        *new_process = (process){
            .arrival = arrival,
            .id = id,
            .runtime = runtime,
            .remainingtime = runtime, // initially, remaining time = needed run time
            .waitingtime = 0,         // initially, waiting time = 0
            .priority = priority,
            .StartedBefore = 0, // initially, process has not started before
            .status = 3,
            .memsize = memsize,
        };
        processes[next_process] = new_process;
        next_process++;
    }

    // TODO: parse the cmd line arguments
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.

    // 3. Initiate and create the scheduler and clock processes.

    getAlgorithmFromUser();

    // system("./scheduler & "); // TODO: You can either pass args or utilize IPCs

    //  4. Send the information to the scheduler at the appropriate time.

    // create the message queue
    key_t key_id;
    int msgq_id, send_val;

    key_id = ftok("keyfile", 65);
    msgq_id = msgget(key_id, 0666 | IPC_CREAT);

    if (msgq_id == -1)
    {
        perror("Error in create msgq_id");
        exit(-1);
    }
    forkClkAndScheduler();
    initClk();

    printf("Message Queue ID = %d\n", msgq_id);
    fflush(stdout);

    // end of message queue initialization

    int next_scheduled = 0; // next process to schedule
    while (true)
    {

        int x = getClk();
        if (next_scheduled == size)
        {
            // system("fg");
            break;
        };
        while (next_scheduled < size)
        {
            // printf("%d", processes[next_scheduled].arrival);

            if (processes[next_scheduled]->arrival == x)
            {
                // send to the queue

                // char str[] = "\nMessage from sender to receiver..";
                msgBuf message;

                message.mtype = 1; /* arbitrary value */
                // strcpy(message.mtext, str);
                message.proc = *processes[next_scheduled];
                send_val = msgsnd(msgq_id, &message, sizeof(message.proc), IPC_NOWAIT);

                if (send_val == -1)
                    perror("Errror in send send_val");
                // end of send
                next_scheduled++;
            }
            else
                break;
        }

        printf("current time is %d\n", x);
        sleep(1);
    }
    printf("Waiting for scheduler to finish\n");
    fflush(stdout);
    int temp_pid = wait(&stat_loc);
    // 5. Clear clock resources
    destroyClk(true);
}

void clearResources(int signum)
{
    destroyClk(true);
    key_t key_id;

    key_id = ftok("keyfile", 65);

    int msgq_id = msgget(key_id, 0666 | IPC_CREAT);
    msgctl(msgq_id, IPC_RMID, (struct msqid_ds *)0);

    exit(0);
    // TODO Clears all resources in case of interruption
}
