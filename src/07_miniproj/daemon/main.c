#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <stdbool.h>
#include <syslog.h>
#include <limits.h>
#include <signal.h>
#include "gpio.h"
#include "file_descriptors.h"
#include "multiplex.h"
#include "control_time.h"
#include "oled.h"
#include "module_com.h"

#define FIFO_PATH "/tmp/fan-csel"
#define MSG_LEN 20
#define READ_TIME 500000000

static void clear_btn_event(int fd) {
    char buf;
    pread(fd, &buf, 1, 0);
}

static void set_led(int fd, int value) {
    if (value)
        pwrite(fd, "1", sizeof("1"), 0);
    else
        pwrite(fd, "0", sizeof("0"), 0);
}

static void check_btn(int fd, int* value) {
    char buf[4];

    ssize_t nr = pread(fd, buf, sizeof(buf), 0);
    if (nr == -1) {
        perror("Cannot read btn");
    }

    *value = atoi(buf);
}

static void create_mkfio() {
    if (mkfifo(FIFO_PATH, 0666) == -1) {
        perror("mkfifo cannot create");
        exit(EXIT_FAILURE);
    }
}

static int create_pipe(struct file_descriptors *dfs) {
    dfs->pipe = open(FIFO_PATH, O_RDWR);
    if (dfs->pipe == -1) {
        perror("open fifo");
        return 1;
    }
    return 0;
}

void catch_sign() {
    unlink(FIFO_PATH);
    exit(EXIT_SUCCESS);
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

int read_from_pipe(int fd, int* mode, int* freq) {
    char msg[MSG_LEN] = {0};
    read(fd, msg, sizeof(msg));
    if (sscanf(msg, "%d;%d", mode, freq) != 2) {
        perror("Cannot parse message from pipe");
        return 1;
    }
    return 0;
}

int main()
{
    struct file_descriptors dfs;
    struct epoll_event events[5];
    struct itimerspec  timing;
    int freq = 0, mode = 0;

    create_mkfio();

    init_oled();
    create_signal(SIGINT);
    create_signal(SIGABRT);
    create_signal(SIGTERM);

    if (create_config_gpios(&dfs) != 0) {
        return 1;
    }

    if (create_pipe(&dfs) != 0) {
        return 1;
    }

    if (create_time(&dfs) != 0) {
        return 1;
    }

    if (set_time(dfs.tfd, &timing, READ_TIME) != 0) {
        return 1;
    }

    if (multiplex_create_epoolfd(&dfs) != 0) {
        return 1;
    }
    multiplex_config_events(&dfs, events);
    
    module_read_freq(&freq);
    module_read_mode(&mode);
    print_oled(mode, 20, freq);
    while (1) {
        int nr = epoll_wait(dfs.epfd, events, 10, -1);
        if (nr == -1) {
            perror("Wait error");
            return -1;
        }

        for (int i = 0; i < nr; i++) {

            if (events[i].data.fd == dfs.k1) {
                int value = 0;
                check_btn(dfs.k1, &value);
                set_led(dfs.led, value);

                if (value) {
                    freq += 1;
                    module_write_freq(freq);
                }
                clear_btn_event(dfs.k1);
                syslog(LOG_INFO, "Button k1");
            }

            if (events[i].data.fd == dfs.k2) {
                int value = 0;
                check_btn(dfs.k2, &value);
                set_led(dfs.led, value);
                if (value && freq > 0) {
                    freq -= 1;
                    module_write_freq(freq);
                }
                clear_btn_event(dfs.k2);
                syslog(LOG_INFO, "Button k2\n");
            }

            if (events[i].data.fd == dfs.k3) {
                int value = 0;
                check_btn(dfs.k3, &value);
                set_led(dfs.led, value);

                if (value) {
                    mode = !mode;
                    module_write_mode(mode);
                }
                clear_btn_event(dfs.k3);
                syslog(LOG_INFO, "Button k3\n");
            }

            if (events[i].data.fd == dfs.pipe) {
                int mode_pipe, freq_pipe = 0;

                read_from_pipe(dfs.pipe, &mode_pipe, &freq_pipe);
                printf("mode: %d, freq: %d\n", mode_pipe, freq_pipe);
                if (mode_pipe == 0) {
                    freq = freq_pipe;
                    module_write_freq(freq);
                }
                mode = mode_pipe;
                module_write_mode(mode);
                syslog(LOG_INFO, "Pipe\n");
            }

            if (events[i].data.fd == dfs.tfd) {
                if (clear_event_time(dfs.tfd) != 0) {
                    return 1;
                }
            }
        }
        print_oled_mode(mode);
        print_oled_freq(freq);
    }

    return 0;
}
