#include "headers.h"

int burstTime,startTime,waitingTime, Q_ID_SMP;


void recieveProcessParameters(int Q_ID_SMP)
{
    ProcessParametersBuff messageReceived;

    //recieve only the messages of mtype getpid()%10000
    int rec_val = msgrcv(Q_ID_SMP, &messageReceived, sizeof(messageReceived.parameters), getpid()%10000, !IPC_NOWAIT);

    //Handle unexpected errors by notifying the user
    if (rec_val == -1)
        perror("Error in receiving the process parametrs from the message Q");

    burstTime=messageReceived.parameters.burstTime; //running time
    startTime=messageReceived.parameters.startTime;
    waitingTime=messageReceived.parameters.waitingTime;
    printf("bT: %d, ST: %d, WT: %d\n",burstTime,startTime,waitingTime );
    
}
void continueMe(int signum)
{
    printf("I will now continue \n");
    recieveProcessParameters(Q_ID_SMP);
    signal(SIGCONT, continueMe);   
}
int main(int agrc, char * argv[])
{

    Q_ID_SMP = msgget(Q_ID_SMP_KEY, 0666 | IPC_CREAT); 
    //Handle unexpected errors by notifying the user and shutting down
    if (Q_ID_SMP == -1)
    {
        perror("Error in create the message queue of schedulerIsMessagingProcess");
        exit(-1);
    }
    //If the data is not sent from the scheduler yet, sleep
    recieveProcessParameters(Q_ID_SMP); 
    //as soon as I recieve my parameters,I set them and begin the loop
    

    printf("I am a process that got forked by the scheduler with these parameters: \n");
    printf("%d,%d,%d\n",burstTime,startTime,waitingTime);
    signal(SIGCONT, continueMe);
    initClk();
    
    // the loop continues till I am either finished or preempted
    while (burstTime-getClk()+startTime+waitingTime > 0);
    
    destroyClk(false);
    
    return 0;
}
