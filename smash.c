/*	smash.c
main file. This file contains the main function of smash
*******************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "commands.h"
#include "signals.h"
//#include "signals.h"


#define MAX_LINE_SIZE 80
#define MAXARGS 20

//char* L_Fg_Cmd;
char lineSize[MAX_LINE_SIZE];
Job jobs[MAX_PROCCESS_NUM]; //This represents the list of jobs. Please change to a preferred type (e.g array of char*)
int Fg_pid = EMPTY; //pid of the process the runs in foreground
//**************************************************************************************



// function name: main
// Description: main function of smash. get command from user and calls command functions
//**************************************************************************************
int main(int argc, char *argv[])
{
    char cmdString[MAX_LINE_SIZE];


    //signal declaretions
    //NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
    /* add your code here */

    /************************************/
    //NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
    //set your signal handlers here
    /* add your code here */

    struct sigaction act1;
    act1.sa_handler = &ctrlChandler;
    sigaction(SIGINT,&act1,NULL);

    struct sigaction act2;
    act2.sa_handler = &ctrlZhandler;
    sigaction(SIGTSTP,&act2,NULL);


    /************************************/

    /************************************/
    // Init globals
    for (int i=0;i<MAX_PROCCESS_NUM;i++){
        jobs[i].pid = EMPTY;
        jobs[i].serialNum = EMPTY;
    }
  //  fgJob.serialNum = EMPTY;
    //Fg_pid = EMPTY;

    while (1)
    {
        printf("smash > ");
        fgets(lineSize, MAX_LINE_SIZE, stdin);
        strcpy(cmdString, lineSize);
        cmdString[strlen(lineSize)-1]='\0';
        // perform a complicated Command
        if(!ExeComp(lineSize)) continue;
        // background command
        if(!BgCmd(lineSize, jobs)) continue;
        // built in commands
        ExeCmd(jobs, lineSize, cmdString);

        /* initialize for next line read*/
        lineSize[0]='\0';
        cmdString[0]='\0';
    }
    return 0;
}

