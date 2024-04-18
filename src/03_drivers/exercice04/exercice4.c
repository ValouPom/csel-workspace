#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

static char* msg = "Hello world !\n";

int main(int argc, char const *argv[])
{
   int instance;
   int fd;
   char path[20];
   char readbuf[20];

    if (argc < 2) {
        printf("Bad args\n");
        return -1;
    }

    instance = atoi(argv[1]);

    sprintf(path, "/dev/mymodule%d", instance);
    fd = open(path, O_RDWR);

    if (fd < 0) {
        printf("Cannot open file\n");
    }

    write(fd, msg, strlen(msg));


    read(fd, readbuf, 20);

    printf("Message : %s\n", readbuf);
    
    return 0;
}
