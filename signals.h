//
// Created by mayfl on 27/11/2019.
//


#ifndef _SIGS_H
#define _SIGS_H
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "job.h"

#define MAX_PROCCESS_NUM 100
#define EMPTY -1
extern Job jobs[];
extern int Fg_pid;
extern Job fgJob;



/**
 * handler of ctrlZ.
 * sends SIGTSTP to the process that running in foreground.
 * also adds it to jobs array (as a new proccess with new time and serial number)
 * if there is no process running, the program is not affected
 */
void ctrlZhandler ();
/**
 *  handler of ctrlC.
 *  sends SIGINT to the process that running in foreground.
 *  if there is no process running, the program is not affected
 */
void ctrlChandler ();


#endif




