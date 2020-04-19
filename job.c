//
// Created by mayfl on 28/11/2019.
//

#include "job.h"

void removeDoneJobs(Job jobs[]) {
    if(jobs == NULL) {
        return;
    }
    int  pid;
    for(int i = 0; i < MAX_PROCCESS_NUM; i++) {
        if(jobs[i].serialNum== EMPTY) {
            continue;
        }
        pid = jobs[i].pid;
        if (waitpid((pid), NULL, WNOHANG)!= 0) { // child process has finished
            jobs [i].serialNum = EMPTY; //Delete the process from the array by setting -1 to serial number
        }
    }
}