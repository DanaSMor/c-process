
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <signal.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <cstdio>
#include <iostream>
#include "md5.h"

#define SIZE 20
#define ENCRYPTSIZE 32
char secret[33];
int pid;

void sigqut(int sig_num)
{
    exit(0);
}

void sigint(int sig_num)
{
    char check = secret[0];
    int i = 0;
    while (check)
    {
        check = secret[++i];
    }
    if (i == 32)
    {
        printf("encrypted by process %d : %s \n", getpid(), secret);
        kill(pid, SIGQUIT);
    }
}

int main()
{
    signal(SIGINT, sigint);
    char msg[SIZE];
    char buffer[SIZE];
    int pipeParent[2], pipeChild[2], i;

    int checkPipeP = pipe(pipeParent);
    int checkPipeC = pipe(pipeChild);

    if (checkPipeP < 0 || checkPipeC < 0)
    {
        printf("Unable open pipe \n");
        exit(1);
    }

    if ((pid = fork()) < 0)
    {
        perror("Unable fork \n");
        exit(1);
    }

    if (pid)//father
    {
        printf("write something: ");
        fgets(msg, SIZE, stdin);
        write(pipeParent[1], msg, SIZE);
        read(pipeChild[0], secret, 33);

        pause();
    }
    else //son
    {
        signal(SIGQUIT, sigqut);
        read(pipeParent[0], buffer, SIZE);
        write(pipeChild[1],md5(buffer).c_str(), ENCRYPTSIZE);
        sleep(2);
        kill(getppid(), SIGINT);
        pause();
    }

    return 0;
}
