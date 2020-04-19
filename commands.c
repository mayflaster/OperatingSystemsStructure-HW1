//		commands.c
//********************************************
#include "commands.h"

#define MAX_PATH_SIZE 255
char lastDirectory [MAX_PATH_SIZE] = "EMPTY";
char historyArr [50][MAX_LINE_SIZE];

int counter = 0; //count the num of commands in historyArr (max=50)
Job fgJob; //global job struct for the running in foreground job
int counter_of_jobs=0; //counts the number jobs the was running in background since the beginning of the program
extern int Fg_pid;



/*
 add the new command to the history array
*/
void updateHistArray (char * command){
    if (counter < 50){
        strcpy(historyArr[counter],command);
        counter ++;
    }
    else {
        for(int i=49; i>0 ;i--){
            strcpy(historyArr[i-1],historyArr[i]);
        }
        strcpy(historyArr[49],command);
    }
}

//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd(Job jobs[], char* lineSize, char* cmdString)
{
    char* cmd;
    char* args[MAX_ARG];
    char pwd[MAX_LINE_SIZE];
    char* delimiters = " \t\n";
    int i = 0, num_arg = 0;
    bool illegal_cmd = FALSE; // illegal command
    cmd = strtok(lineSize, delimiters);
    if (cmd == NULL)
        return 0;
    args[0] = cmd;
    for (i=1; i<MAX_ARG; i++)
    {
        args[i] = strtok(NULL, delimiters);
        if (args[i] != NULL)
            num_arg++;
    }
    updateHistArray(cmdString);
/*************************************************/
// Built in Commands PLEASE NOTE NOT ALL REQUIRED
// ARE IN THIS CHAIN OF IF COMMANDS. PLEASE ADD
// MORE IF STATEMENTS AS REQUIRED
/*************************************************/
    if (!strcmp(cmd, "cd") )
    {
        if (num_arg!=1){
            illegal_cmd=TRUE;
        }
        char temp [MAX_PATH_SIZE];
        strcpy(temp,getcwd(temp,MAX_PATH_SIZE));
        //temp = getcwd(temp,MAX_LINE_SIZE);
        if ( !strcmp(args[1],"-")){
            if (!strcmp(lastDirectory,"EMPTY")){
                return 1;
            }
            chdir(lastDirectory);
            printf("%s\n",lastDirectory);
            strcpy(lastDirectory,temp);
        }
        else if (chdir(args[1]) < 0 ){
            printf("smash error: >  \"%s\" - path not found\n",args[1] );
            return 1;
        }
        else{
            strcpy(lastDirectory,temp);
        }
    }
        /*************************************************/
    else if (!strcmp(cmd, "pwd"))
    {
        if (num_arg!=0){
            illegal_cmd = TRUE;
        }
        else {
            strcpy(pwd,getcwd(pwd,MAX_LINE_SIZE));
            //pwd = getcwd(pwd,MAX_LINE_SIZE);
            if (pwd == NULL){
                perror("smash");
                return 1;
            }
            else{
                printf("%s\n",pwd);
                //updateHistArray(cmd);
            }
        }

    }

    else if (!strcmp(cmd, "jobs"))
    {

        if (num_arg != 0){
            illegal_cmd = TRUE;
        }
        else {
            removeDoneJobs(jobs);
            for ( int i=1; i<=counter_of_jobs; i++){
                for (int j =0; j<MAX_PROCCESS_NUM;j++){
                    if (jobs[j].serialNum == i){
                        printf("[%d] %s : %d %d secs", i,jobs[j].name,jobs[j].pid, (int)(time(NULL)-jobs[j].timeCreated));
                        if (jobs[j].isPaused == TRUE){
                            printf(" (Stopped)");
                        }
                        printf("\n");
                    }
                }
            }
        }
    }
        /*************************************************/
    else if (!strcmp(cmd, "showpid"))
    {
        if (num_arg > 0){
            illegal_cmd=TRUE;
        }
        printf ("smash pid is %d \n", (int) getpid());
    }
        /*************************************************/
    else if (!strcmp(cmd, "fg"))
    {
        if (num_arg == 1){
            removeDoneJobs(jobs);
            int jobIndex = EMPTY;
            for (int i = 0; i<MAX_PROCCESS_NUM;i++){ //search job with input serial
                if (jobs[i].serialNum==atoi(args[1])){ //add check if number ?
                    jobIndex=i;
                }
            }
            if (jobIndex == EMPTY) { // not found any job
                illegal_cmd=TRUE;
            }
            else {
                printf("%s\n",jobs[jobIndex].name);
                if(jobs[jobIndex].isPaused == TRUE){
                        if (kill(jobs[jobIndex].pid,SIGCONT)){
                            jobs[jobIndex].isPaused=FALSE;
                            printf("smash > signal SIGCONT was sent to pid %d", (int)jobs[jobIndex].pid);
                        }
                        else {
                            perror("smash");
                            return 1;
                        }
                    }
                    Fg_pid = jobs[jobIndex].pid;
                    fgJob.pid =Fg_pid;
                    fgJob.timeCreated=time(NULL);
                    jobs[jobIndex].serialNum=EMPTY;
                    strcpy(fgJob.name,jobs[jobIndex].name);
                     waitpid(jobs[jobIndex].pid, NULL, 0);
                     Fg_pid=EMPTY;
                }
            }
            else if (num_arg == 0){
                removeDoneJobs(jobs);
                int maxSerial = jobs[0].serialNum;
                int maxIndex=0;
                for (int i = 1; i< MAX_PROCCESS_NUM;i++){
                    if (maxSerial<=jobs[i].serialNum){ //find max serial number
                        maxIndex=i;
                        maxSerial=jobs[i].serialNum;
                    }
                }
                if (maxSerial == EMPTY) {
                    return 1;
                    //illegal_cmd=TRUE;
                }
                else {
                    printf("%s\n",jobs[maxIndex].name);
                    if(jobs[maxIndex].isPaused == TRUE){
                            if (!kill(jobs[maxIndex].pid,SIGCONT)){
                                jobs[maxIndex].isPaused=FALSE;
                                printf("smash > signal SIGCONT was sent to pid %d\n", (int)jobs[maxIndex].pid);
                            }
                            else {
                                perror("smash");
                                return 1;
                            }
                        }
                    Fg_pid = jobs[maxIndex].pid; // maybe change to struct with all the information
                    fgJob.pid =Fg_pid;
                    fgJob.timeCreated=time(NULL);
                    jobs[maxIndex].serialNum=EMPTY; // after job is finished, remove from job list
                    strcpy(fgJob.name,jobs[maxIndex].name);
                    waitpid(jobs[maxIndex].pid, NULL, 0) ;
                    }
                      Fg_pid=EMPTY;
                }
                else{
                    illegal_cmd=TRUE;
                }
    }
                /*************************************************/
            else if (!strcmp(cmd, "bg"))
            {
                if (num_arg == 1){
                    removeDoneJobs(jobs);
                    int jobIndex = EMPTY;
                    for (int i = 0; i<MAX_PROCCESS_NUM; i++){
                        if (jobs[i].serialNum == atoi (args[1]) ){
                            jobIndex = i;
                        }
                    }
                    if (jobIndex == EMPTY){ // if we did not find job with the command number
                        illegal_cmd =TRUE;
                    } else{
                        if (jobs[jobIndex].isPaused == TRUE){ // what we do else ? (if job not paused)
                            if (!kill(jobs[jobIndex].pid,SIGCONT)){
                                jobs[jobIndex].isPaused=FALSE;
                                printf("smash > signal SIGCONT was sent to pid %d\n", (int)jobs[jobIndex].pid);
                            }
                            else {
                                perror("smash");
                                return 1;
                            }
                        }
                    }

                }
                else if (num_arg == 0){
                    removeDoneJobs(jobs);
                    int lastPausedindex = EMPTY;
                    time_t lastPauseTime = EMPTY;
                    for (int i=0; i<MAX_PROCCESS_NUM;i++){
                        if (jobs[i].isPaused == TRUE && jobs[i].lastPause > lastPauseTime){
                            lastPausedindex=i;
                            lastPauseTime=jobs[i].lastPause;
                        }
                    }
                    if (lastPausedindex != EMPTY){
                        if (!kill(jobs[lastPausedindex].pid,SIGCONT)){
                            jobs[lastPausedindex].isPaused=FALSE;
                            printf("smash > signal SIGCONT was sent to pid %d\n", (int)jobs[lastPausedindex].pid);
                        }
                        else {
                            perror("smash");
                            return 1;
                        }
                    }
                }
                else { // if we got here, we did not found paused job
                    illegal_cmd = TRUE;
                }
            }
                /*************************************************/
            else if (!strcmp(cmd, "quit"))
            {
                if (num_arg == 0){
                    exit(0);
                }
                else if (num_arg == 1 && !strcmp (args[1], "kill")){
                    removeDoneJobs(jobs);
                    for (int j = 0; j < MAX_PROCCESS_NUM; ++j) {
                        if(jobs[j].serialNum != EMPTY){
                            if (!waitpid(jobs[j].pid, NULL, WNOHANG)){//check if proccess not done yet
                                printf("[%d] %s - Sending SIGTERM..." ,jobs[j].serialNum,jobs[j].name);
                                if (kill(jobs[j].pid,SIGTERM)!=0){
                                    perror("smash");
                                }
                                for (int k = 0; k <5 ; ++k) { //wait 5 seconds
                                    if (!waitpid(jobs[j].pid, NULL, WNOHANG)){
                                        sleep(1);
                                    }
                                    else break;
                                }
                                if (!waitpid(jobs[j].pid, NULL, WNOHANG)){ //if job  not sone
                                    if (kill(jobs[j].pid,SIGKILL)!=0){
                                        perror("smash");
                                    }
                                    else{
                                        printf("(5 sec passed) Sending SIGKILL..." );
                                    }
                                }
                                while (1) {
                                    if (waitpid(jobs[j].pid, NULL, WNOHANG) != 0) {
                                        break;
                                    }
                                }
                                printf("Done.\n");

                            }
                        }
                    }
                    exit(0);
                }
                else{
                    illegal_cmd = TRUE;
                }
            }
                /*************************************************/
            else if (!strcmp(cmd, "history"))
            {
                if (num_arg > 0){
                    illegal_cmd=TRUE;
                }
                else {
                    for (i=0;i<counter;i++){
                        printf("%s\n", historyArr[i]);
                    }
                }
            }
                /*************************************************/
            else if (!strcmp(cmd, "mv"))
            {
                if (num_arg != 2){
                    illegal_cmd = TRUE;
                }
                else {
                    if (!rename(args[1], args[2])){
                        printf("%s has been renamed to %s\n", args[1],args[2]);
                    }
                    else{
                        perror("smash");
                        return 1;
                    }

                }
            }
                /*************************************************/
            else if (!strcmp(cmd, "kill"))
            {
                if (num_arg != 2){
                    illegal_cmd=TRUE;
                }
                else {
                    removeDoneJobs(jobs);
                    //calculate number (arg[0])
                    int signalNumber;
                    int  x  = (-1) * atoi (args[1]);
                    if (x != 0 || !strcmp("-0",args[1])){
                        signalNumber=x;
                        int jobIndex = EMPTY;
                        for ( int i =0; i<MAX_PROCCESS_NUM;i++){
                            if (jobs[i].serialNum == atoi(args[2])){
                                jobIndex=i;
                            }
                        }
                        if ( jobIndex == EMPTY) {
                            printf ("smash error: > kill %s – job does not exist\n" , args[2]);
                        }
                        else if (signalNumber < 0 || signalNumber > 31){
                            printf ("smash error: > kill %s – cannot send signal\n" , args[2]);
                        }
                        else {
                            if (kill(jobs[jobIndex].pid,signalNumber)!=0){
                                perror("smash");
                                return 1;
                            }
                            else {
                                printf("smash > signal %d was sent to pid %d\n",signalNumber,jobs[jobIndex].pid );
                            }
                        }
                    }
                    /*else if (!strcmp("-0",args[1])){
                        signalNumber = 0;
                    }*/
                    else {
                        illegal_cmd=TRUE;
                    }

                }
            }
                /*************************************************/
                /*************************************************/
            else // external command
            {
                ExeExternal(args, cmdString);
                return 0;
            }
            if (illegal_cmd == TRUE)
            {
                printf("smash error: > \"%s\"\n", cmdString);
                return 1;
            }
            //else {

         //   }
            return 0;
        }
//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************
        void ExeExternal(char *args[MAX_ARG], char* cmdString)
        {
            int pID;

            switch(pID = fork())
            {
                case -1:
                    perror("smash");
                    exit(1);
                    break;
                case 0 :
                    //updateHistArray(cmdString);
                    // Child Process
                    setpgrp(); // changes the group id of the child
                    execvp(args[0], args); // first element  is the cmd name, second is the arguments
                    perror("smash");// execvp fails
                    exit(1);
                    break;

                default:

                    Fg_pid = pID;
                    fgJob.pid =pID;
                    fgJob.timeCreated=time(NULL);
                    strcpy(fgJob.name,cmdString);
                    waitpid(pID, NULL,0); // maybe zero needs to be changed to options
                    Fg_pid = EMPTY;

                    return;

            }
        }
//**************************************************************************************
// function name: ExeComp
// Description: executes complicated command
// Parameters: command string
// Returns: 0- if complicated -1- if not
//**************************************************************************************
        int ExeComp(char* lineSize)
        {
            //char ExtCmd[MAX_LINE_SIZE+2];
           // char *args[MAX_ARG];
            if ((strstr(lineSize, "|")) || (strstr(lineSize, "<")) || (strstr(lineSize, ">")) || (strstr(lineSize, "*")) || (strstr(lineSize, "?")) || (strstr(lineSize, ">>")) || (strstr(lineSize, "|&")))
            {
                return 0;
            }
            return -1;
        }
//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
        int BgCmd(char* lineSize, Job jobs[])
        {
            char Command[MAX_LINE_SIZE];
            char cmdString[MAX_LINE_SIZE];
            cmdString[strlen(lineSize)-1]='\0';
            strcpy(cmdString, lineSize);
            cmdString[strlen(lineSize)-1]='\0';
            //strcpy(commandBeforeChange,lineSize);
            char* delimiters = " \t\n";
            char *args[MAX_ARG];
            int pid;
            if (lineSize[strlen(lineSize)-2] == '&')
            {
                lineSize[strlen(lineSize)-2] = '\0';
               // char name[MAX_NAME_SIZE];
                int i = 0, num_arg = 0;
                //	bool illegal_cmd = FALSE; // illegal command
                strcpy(Command,strtok(lineSize, delimiters));
                //Command = strtok(lineSize, delimiters);
                if (Command == NULL)
                    return 0;
                args[0] = Command;
                for (i=1; i<MAX_ARG; i++)
                {
                    args[i] = strtok(NULL, delimiters);
                    if (args[i] != NULL)
                        num_arg++;

                }
                switch(pid = fork())
                {
                    case -1:
                        perror("smash");
                        exit(1);
                        break;
                    case 0 :
                        // Child Process
                        setpgrp(); // changes the group id of the child
                        execvp(args[0], args); // first element  is the cmd name, second is the arguments
                        perror("smash");// execvp fails
                        exit(1);
                        break;

                    default: //parent code - insert to jobs array the child proccess
                        removeDoneJobs(jobs);
                        counter_of_jobs++;
                        //printf("inserting jobs\n");
                        Job newJob;
                        //strcpy(newJob.name,Command);
                        newJob.pid = pid;
                        newJob.timeCreated = time(NULL);
                        newJob.serialNum=counter_of_jobs;
                        newJob.isPaused = FALSE;
                        newJob.lastPause=0;
                        for (int i = 0; i<MAX_PROCCESS_NUM; i++){
                            if (jobs[i].serialNum == EMPTY){
                                jobs[i]=newJob;
                                // printf("done inserting jobs. i =%d\n",i );
                                strcpy(jobs[i].name,Command);

                                break;
                            }
                        }
                        updateHistArray(cmdString);
                }
                return 0;

            }
            return -1;
        }

