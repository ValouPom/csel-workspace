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

#define ARRAY_SIZE 100

int custom_write(int fd, char* buf) {
    ssize_t len = strlen(buf);
    ssize_t count = write(fd, buf, len);
    if (count == -1 || count != len) {
        perror("Error in writing");
        return -1;
    }
    return 0;
}

int custom_read(int fd, char* buf, size_t size) {
    while(1) {
        ssize_t nr = read(fd, buf, size);
        printf("Size %ld\n", nr);
        if (nr > 0) {
            break;
        }

        if (nr == -1) {
            perror("Error in lecture");
            if (errno == EINTR) continue;

            return -1;
        }
    }
    return 0;
}

void child_fn(int fd) {
    char* hello = "hello";
    char* exitCh  = "exit";
    cpu_set_t set;

    CPU_ZERO(&set);
    CPU_SET(1, &set);
    int ret = sched_setaffinity(0, sizeof(set), &set);

    if (ret == -1) {
        perror("Cannot set core");
    }

    printf("write hello\n");

    if (custom_write(fd, hello) != 0) {
        perror("Error when writting");
        return;
    }
    printf("has write hello\n");
    sleep(1);
    if (custom_write(fd, hello) != 0) {
        perror("Error when writting");
        return;
    }
    sleep(1);
    if (custom_write(fd, exitCh) != 0) {
        perror("Error when writting");
        return;
    }
}

void parent_fn(int fd) {
    bool is_terminating = true;
    cpu_set_t set;

    CPU_ZERO(&set);
    CPU_SET(0, &set);
    int ret = sched_setaffinity(0, sizeof(set), &set);

    if (ret == -1) {
        perror("Cannot set core");
    }

    while (is_terminating) {
        char buff[ARRAY_SIZE] = {0};
        if (custom_read(fd, buff, sizeof(buff)) != 0) {
            perror("error when reading");
            return;
        }

        if (strcmp(buff, "exit") == 0) {
            break;
        }

        printf("Msg from child: %s\n", buff);
        memset(buff, 0, ARRAY_SIZE);
    }
    printf("terminate\n");
}

void catch_sign(int sig) {
    printf("Signal captured %d\n", sig);
}

void create_signal(int sign) {
    struct sigaction act = {
        .sa_handler = catch_sign,
    };
    int err = sigaction(sign, &act, NULL);
    if (err == -1) {
        perror("Cannot create signal");
    }
}

int main()
{
    printf("Hello world\n");

    create_signal(SIGHUP);
    create_signal(SIGINT);
    create_signal(SIGABRT);
    create_signal(SIGTERM);

    int fd[2];
    int err = socketpair(AF_UNIX, SOCK_STREAM, 0, fd);
    if (err == -1) {
        perror("socketpair error");
    }

    pid_t pid = fork();
    if (pid == 0) {
        // child
        child_fn(fd[0]);
    }
    else if (pid > 0) {
        parent_fn(fd[1]);
    }
    else {
        perror("Cannot fork");
    }

    close(fd[0]);
    close(fd[1]);

    return 0;
}
