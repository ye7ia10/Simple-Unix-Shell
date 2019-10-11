#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>


char line[1024];
char* args[100];
int Is_BackGround = 0;

void TakeCommand()
{
    fgets(line, 1024, stdin);
    int len = strlen(line);
    line[len - 1] = '\0';
}

int CheckIfBackground (char* LastCmd)
{
    if (LastCmd[strlen(LastCmd) - 1] == '&')
    {
        return 1;
    }
    return 0;
}

void splitTheCommand()
{
    int i = 0;
    int x;
    args[i] = strtok(line, " \n");

    while (args[i] != NULL)
    {
        args[++i] = strtok(NULL, " \n");
    }

    x = i - 1;
    Is_BackGround = CheckIfBackground(args[x]);
    if (Is_BackGround)
    {
        args[x] = NULL;
    }
}

void writeToLog()
{
    FILE *logfile;
    logfile = fopen("log.txt", "a");
    fprintf(logfile, "Child process was terminated\n");
    fclose(logfile);
}

void MakeProcess()
{
    pid_t pid;
    pid = fork();

    if (pid == 0)
    {
        execvp(args[0], args);
    }
    else if (pid < 0)
    {
        printf("\nError forking child process\n");
    }
    else if (Is_BackGround)
    {
        signal(SIGCHLD, writeToLog);
    }
    else
    {
        waitpid(pid, NULL, 0);
    }

}
int main()
{
    printf("MY Simple Unix Shell\n");
    while (1)
    {
        printf("\nShellCommand -> ");
        TakeCommand();
        if (strcasecmp(line,"exit") == 0)
        {
            exit(0);
        }
        else if(strcasecmp(line,"cd")==0)
        {
            chdir(args[1]);
        }
        else
        {
            splitTheCommand();
            MakeProcess();
        }
    }
    return 0;
}
