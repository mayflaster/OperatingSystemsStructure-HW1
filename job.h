//
// Created by mayfl on 28/11/2019.
//

#ifndef WET1OS_JOB_H
#define WET1OS_JOB_H

#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include "job.h"
#include <sys/wait.h>



#define MAX_NAME_SIZE 50
#define EMPTY -1
#define MAX_PROCCESS_NUM 100
typedef enum { FALSE , TRUE } bool;

typedef struct job {
    int serialNum;
    char name [MAX_NAME_SIZE];
    int pid;
    time_t timeCreated;
    time_t lastPause;
    bool  isPaused;
}Job;

/**
 * this function checks all the jobs array and check if a job is done.
 * if a job is done, removes it from the array.
 * @param jobs - array of running  jobs.
 */
void removeDoneJobs(Job jobs[]) ;



#endif //WET1OS_JOB_H
