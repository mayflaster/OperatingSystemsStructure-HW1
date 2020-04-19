//
// Created by mayfl on 27/11/2019.
//

#include "signals.h"


// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */

//extern void removeDoneJobs(Job* jobs) ;
extern int counter_of_jobs; //counts the number of bg runnig jobs
extern int Fg_pid;
// ctrl c - stop the process in foreground (and remove from jobs!)
void ctrlChandler (){
    if (Fg_pid == EMPTY)
        return;
    if(!kill(Fg_pid,SIGINT)){
        printf("smash > signal SIGINT was sent to pid %d\n", Fg_pid);
        Fg_pid=EMPTY;
    }
    else {
        perror("smash");
    }
}

// ctrl z. pause and add to job list
void ctrlZhandler (){
    if (Fg_pid==EMPTY)
        return;
    if(!kill(Fg_pid,SIGTSTP)){
        printf("smash > signal SIGTSTP was sent to pid %d\n", Fg_pid);
        removeDoneJobs(jobs);
        counter_of_jobs++;
        //strcpy(newJob.name,Command);
        fgJob.pid = Fg_pid;
        fgJob.timeCreated = time(NULL);
        fgJob.serialNum=counter_of_jobs;
        fgJob.isPaused = TRUE;
        fgJob.lastPause=time(NULL);
        for (int i = 0; i<MAX_PROCCESS_NUM; i++){
            if (jobs[i].serialNum == EMPTY){
                jobs[i]=fgJob;
                strcpy(jobs[i].name,fgJob.name);
                break;
            }
        }
        Fg_pid=EMPTY;
    }
    else{
        perror("smash");

    }
}




