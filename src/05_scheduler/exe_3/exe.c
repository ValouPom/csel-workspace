#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>

int main()
{

    //fork();

    int i = 0;
    while(1)
    {
        i++;
        if (i > 1000000)
            i = 0;   
    }
    printf("%d", i);

    return 0;
}
