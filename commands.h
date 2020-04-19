//
// Created by mayfl on 27/11/2019.
//




#ifndef _COMMANDS_H
#define _COMMANDS_H
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include "job.h"
#include <sys/wait.h>

#define MAX_PROCCESS_NUM 100
#define EMPTY -1
#define MAX_LINE_SIZE 80
#define MAX_ARG 20

int ExeComp(char* lineSize);
int BgCmd(char* lineSize, Job jobs[]);
int ExeCmd(Job jobs[], char* lineSize, char* cmdString);
void ExeExternal(char *args[MAX_ARG], char* cmdString);
#endif



