#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>

#define FIFO_PATH "/tmp/fan-csel"
#define AUTO_MODE 1
#define MANU_MODE 0
#define MSG_LEN 20

static void help(const char* argv[])
{
    printf("Usage: %s <command> [args]\n\n", argv[0]);
    printf("command:\n");
    printf("\tauto:\t\tautomatic mode\n");
    printf("\tmanu <freq>:\tmanual mode with frequency\n");
}

static int create_pipe(int* fd) {
    *fd = open(FIFO_PATH, O_RDWR);
    if (*fd == -1) {
        perror("open fifo");
        return 1;
    }
    return 0;
}

static void write_pipe(int fd, char* msg, int mode, int freq) {
    if (snprintf(msg, MSG_LEN, "%d;%d", mode, freq) <= 0) {
        perror("Cannot format message");
        exit(EXIT_FAILURE);
    }

    if (write(fd, msg, strlen(msg)) == -1) {
        perror("Cannot write the message");
        exit(EXIT_FAILURE);
    }
}

static void auto_fn() {
    char msg[MSG_LEN] = {0};
    int fd;

    if (create_pipe(&fd) != 0) {
        exit(EXIT_FAILURE);
    }

    write_pipe(fd, msg, AUTO_MODE, 0);
    printf("Set auto mode\n");
}

static void manu_fn(int freq) {
    char msg[MSG_LEN] = {0};
    int fd;

    if (create_pipe(&fd) != 0) {
        exit(EXIT_FAILURE);
    }

    write_pipe(fd, msg, MANU_MODE, freq);
    printf("Set manu mode with %d Hz\n", freq);
}

int main(int argc, char const *argv[])
{
    if (argc < 2) {
        help(argv);
        return 1;
    }

    if (strcmp(argv[1], "auto") == 0) {
        auto_fn();
    } else if (strcmp(argv[1], "manu") == 0) {
        int value;
        if (argc != 3) {
            printf("Bad usage of manu command\n");
            help(argv);
            return 1;
        }

        if (sscanf(argv[2], "%d", &value) != 1) {
            printf("Bad frequency\n");
            return 1;
        }

        manu_fn(value);
    } else {
        printf("Unknown command: %s\n", argv[1]);
        return 1;
    }
}

