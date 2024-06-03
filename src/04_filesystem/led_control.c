/**
 * Copyright 2018 University of Applied Sciences Western Switzerland / Fribourg
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Project: HEIA-FR / HES-SO MSE - MA-CSEL1 Laboratory
 *
 * Abstract: System programming -  file system
 *
 * Purpose: NanoPi silly status led control system
 *
 * Autĥor:  Daniel Gachet
 * Date:    07.11.2018
 */
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
#include "gpio.h"
#include "file_descriptors.h"
#include "multiplex.h"
#include "control_time.h"

#define MAX_FREQ 980000000
#define FREQ_INC 30000000
#define MAX_ENVENTS 10

static void toggle_led(int led, int* k) {
    if (*k == 0)
        pwrite(led, "1", sizeof("1"), 0);
    else
        pwrite(led, "0", sizeof("0"), 0);
    *k = (*k + 1) % 2;
}

static void clear_btn_event(int fd, int tfd, struct itimerspec *timing, long freq) {
    char buf;
    pread(fd, &buf, 1, 0);
    set_time(tfd, timing, freq);
}

int main(int argc, char* argv[])
{
    struct file_descriptors dfs;
    long period = 1000;  // ms
    struct itimerspec  timing;
    struct epoll_event events[4];

    if (argc >= 2) period = atoi(argv[1]);
    period *= 1000000;  // in ns
    long freq = period / 2;

    if (create_config_gpios(&dfs) != 0) {
        return 1;
    }

    pwrite(dfs.led, "1", sizeof("1"), 0);

    if (create_time(&dfs) != 0) {
        return 1;
    }

    if (set_time(dfs.tfd, &timing, freq) != 0) {
        return 1;
    }

    if (multiplex_create_epoolfd(&dfs) != 0) {
        return 1;
    }
    multiplex_config_events(&dfs, events);

    int k = 0;
    while (1) {
        int nr = epoll_wait(dfs.epfd, events, MAX_ENVENTS, -1);
        if (nr == -1) {
            perror("Wait error");
            return -1;
        }

        for (int i = 0; i < nr; i++) {
            if (events[i].data.fd == dfs.tfd) {
                if (clear_event_time(dfs.tfd) != 0) {
                    return 1;
                }
                toggle_led(dfs.led, &k);
            }

            if (events[i].data.fd == dfs.k1) {
                freq += FREQ_INC;
                if (freq >= MAX_FREQ) {
                    syslog(LOG_WARNING, "Button k1, frequency too high\n");
                    freq = MAX_FREQ;
                } 

                clear_btn_event(dfs.k1, dfs.tfd, &timing, freq);
                syslog(LOG_INFO, "Button k1, freq: %ld\n", freq);
            }

            if (events[i].data.fd == dfs.k2) {
                freq = period / 2;
                clear_btn_event(dfs.k2, dfs.tfd, &timing, freq);
                syslog(LOG_INFO, "Button k2, freq: %ld\n", freq);
            }

            if (events[i].data.fd == dfs.k3) {
                freq -= FREQ_INC;
                if (freq <= 0) {
                    syslog(LOG_WARNING, "Button k3, frequency too low\n");
                    freq = 0;
                } 
                clear_btn_event(dfs.k3, dfs.tfd, &timing, freq);
                syslog(LOG_INFO, "Button k3, freq: %ld\n", freq);
            }
        }
    }

    return 0;
}

