#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>


char line[1024];  /* character array to read the line */
char* args[100];  /* array of strings contains the command and the arguments if exists */
int Is_BackGround = 0; /* flag to check if command contains & at end of it */

void TakeCommand()  /* function to read the command */
{
    fgets(line, 1024, stdin); /* take the command in line */
    int len = strlen(line);  /* size of line */
    line[len - 1] = '\0'; /* remove trailing spaces or new line */
}

int CheckIfBackground (char* LastCmd)  /* function to check & case that take the last argument of the command as parameter*/
{
    if (LastCmd[strlen(LastCmd) - 1] == '&') /* check the last character of it return 1 if contains else will return 0*/
    {
        return 1;
    }
    return 0;
}


void splitTheCommand() /* split the read line into array of arguments */
{
    int i = 0;
    int x;
    args[i] = strtok(line, " \n"); /* splitting the line by spaces */

    while (args[i] != NULL)
    {
        args[++i] = strtok(NULL, " \n");
    }

    x = i - 1; /* index of last argument */
    Is_BackGround = CheckIfBackground(args[x]); /* check the existence of & at end of it */
    if (Is_BackGround) /* if contains & remove it to make it work well */
    {
        args[x] = NULL;
    }
}

void writeToLog()  /* writing into log file when process is terminated */
{
    FILE *logfile;
    logfile = fopen("log.txt", "a");  /* open file to append on it */
    fprintf(logfile, "Child process was terminated\n"); /* write into file */
    fclose(logfile);  /* closing the file */
}

void MakeProcess()
{
    pid_t pid;  /* process id variable */
    pid = fork(); /* save the forking result into pid */

    if (pid == 0) /* it means that it is ready make new child process */
    {
       int xp =  execvp(args[0], args); /* execute the command */
       if (xp < 0){  /* check if error occurs while executing */
        printf("\n Error : execvp failed \n");
       }
    }
    else if (pid < 0) /* error while forking child process */
    {
        printf("\nError forking child process\n");
    }
    else if (Is_BackGround) /* if command contains & make the program do not wait for it and take another commands */
    {
        signal(SIGCHLD, writeToLog); /* signal handler , signal received when child process dies then write it into the log file */
    }
    else
    {
        waitpid(pid, NULL, 0); /* for the parent , wait while completion of the process */
    }

}

int main()
{
    printf("MY Simple Unix Shell\n"); //prompt message
    while (1)
    {
        printf("\nShellCommand -> ");
        TakeCommand();  /* read the command */
        splitTheCommand();  /* split the command into array of arguments */

        if (strcasecmp(line,"exit") == 0)   /* check if it is exit command then close the shell if so*/
        {
            exit(0);
        }

        else if(strcasecmp(line,"cd")==0) /* check if it is change directory command */
        {
            chdir(args[1]);
        }
        else
        {
            //splitTheCommand();  /* split the command into array of arguments */
            MakeProcess();  /* make the process of the command */
        }
    }
    return 0;
}
