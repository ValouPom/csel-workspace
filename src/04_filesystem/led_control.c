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

/*
 * status led - gpioa.10 --> gpio10
 * power led  - gpiol.10 --> gpio362
 */
#define GPIO_EXPORT   "/sys/class/gpio/export"
#define GPIO_UNEXPORT "/sys/class/gpio/unexport"
#define GPIO_LED      "/sys/class/gpio/gpio10"
#define LED           "10"

#define GPIO_K1      "/sys/class/gpio/gpio0"
#define K1           "0"

#define GPIO_K2      "/sys/class/gpio/gpio2"
#define K2           "2"

#define GPIO_K3      "/sys/class/gpio/gpio3"
#define K3           "3"

enum gpio_dir {
    dir_out,
    dir_in,
};

enum gpio_edge {
    edge_none,
    edge_rising,
    edge_falling,
    edge_both,
};

struct gpio {
    enum gpio_dir dir;
    char* number;
    char* gpio_sys_path;
    bool blocking;
    enum gpio_edge edge;
};

static char* get_gpio_edge(enum gpio_edge edge) {
    switch (edge)
    {
    case edge_both:
        return "both";
    case edge_falling:
        return "falling";
    case edge_rising:
        return "rising";
    case edge_none:
    default:
        return "none";
    }
}

static int open_gpio(struct gpio* gpio)
{
    char cfg_pin_path[255] = {0};
    char cfg_pin_edge_path[255] = {0};
    char cfg_pin_value_path[255] = {0};
    char* gpio_edge;
    u_int32_t cfg_flag = 0;

    // unexport pin out of sysfs (reinitialization)
    int f = open(GPIO_UNEXPORT, O_WRONLY);
    write(f, gpio->number, strlen(gpio->number));
    close(f);

    // export pin to sysfs
    f = open(GPIO_EXPORT, O_WRONLY);
    write(f, gpio->number, strlen(gpio->number));
    close(f);

    // config pin direction
    snprintf(cfg_pin_path, sizeof(cfg_pin_path), "%s%s", gpio->gpio_sys_path, "/direction");
    f = open(cfg_pin_path, O_WRONLY);
    if (gpio->dir == dir_out) {
        write(f, "out", 3);
    } else if (gpio->dir == dir_in) {
        write(f, "in", 2);
    }
    close(f);

    // config pin edge
    snprintf(cfg_pin_edge_path, sizeof(cfg_pin_edge_path), "%s%s", gpio->gpio_sys_path, "/edge");
    f = open(cfg_pin_edge_path, O_WRONLY);
    gpio_edge = get_gpio_edge(gpio->edge);
    write(f, gpio_edge, sizeof(gpio_edge));
    close(f);

    // open gpio value attribute
    snprintf(cfg_pin_value_path, sizeof(cfg_pin_path), "%s%s", gpio->gpio_sys_path, "/value");
    if (gpio->blocking) {
        cfg_flag |= O_NONBLOCK;
    } 
    printf("%s\n", cfg_pin_value_path);
    f = open(cfg_pin_value_path, O_RDWR);
    return f;
}

static int add_event(int epfd, int fd, struct epoll_event *event)
{
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, event);
    if (ret == -1) {
        perror("Error epoll create");
        return 1;  // TODO add macro
    }
    return 0;
}

static void toggle_led(int led, int* k) {
    if (*k == 0)
        pwrite(led, "1", sizeof("1"), 0);
    else
        pwrite(led, "0", sizeof("0"), 0);
    *k = (*k + 1) % 2;
}

static int set_time(int tfd, struct itimerspec *timing, long freq) {
    timing->it_value.tv_sec  = 0;
    timing->it_value.tv_nsec = freq;
    timing->it_interval.tv_sec  = 0;
    timing->it_interval.tv_nsec = freq;

    if (timerfd_settime(tfd, 0, timing, NULL) == -1) {
        perror("Bad timer fd settime");
        return 1; // TODO add macro
    }
    return 0;
}

int main(int argc, char* argv[])
{
    int tfd;
    int epfd;
    //long duty   = 2;     // %
    long period = 1000;  // ms
    struct itimerspec  timing;

    if (argc >= 2) period = atoi(argv[1]);
    period *= 1000000;  // in ns

    long freq = period / 2;
    printf("Period %ld, freq: %ld\n", period, freq);

    struct gpio led_gpio = {
        .dir = dir_out,
        .gpio_sys_path = GPIO_LED,
        .number = LED,
        .blocking = false,
        .edge = edge_none,
    };

    struct gpio k1_gpio = {
        .dir = dir_in,
        .gpio_sys_path = GPIO_K1,
        .number = K1,
        .blocking = true,
        .edge = edge_rising,
    };

    struct gpio k2_gpio = {
        .dir = dir_in,
        .gpio_sys_path = GPIO_K2,
        .number = K2,
        .blocking = true,
        .edge = edge_rising,
    };

    struct gpio k3_gpio = {
        .dir = dir_in,
        .gpio_sys_path = GPIO_K3,
        .number = K3,
        .blocking = true,
        .edge = edge_rising,
    };

    int led = open_gpio(&led_gpio);
    if (led == -1) {
        perror("Cannot open gpio");
    }
    //int led = open_led();
    pwrite(led, "1", sizeof("1"), 0);

    int k1 = open_gpio(&k1_gpio);
    if (k1 == -1) {
        perror("Cannot open gpio k1");
    }

    int k2 = open_gpio(&k2_gpio);
    if (k2 == -1) {
        perror("Cannot open gpio k2");
    }

    int k3 = open_gpio(&k3_gpio);
    if (k3 == -1) {
        perror("Cannot open gpio k3");
    }

    struct timespec t1;
    clock_gettime(CLOCK_MONOTONIC, &t1);


    tfd = timerfd_create(CLOCK_REALTIME, 0);
    if (tfd == -1) {
        perror("Bad timer fd");
        return 1; // TODO add macro
    }

    if (set_time(tfd, &timing, freq) != 0) {
        return 1;
    }

    epfd = epoll_create1(0);
    if (epfd == -1) {
        perror("Error epoll create");
        return 1; // TODO add macro
    }

    struct epoll_event events[4];

    events[0] = (struct epoll_event){
    .events = EPOLLIN | EPOLLOUT,
    .data.fd = tfd,
    };
    int ret = add_event(epfd, tfd, &events[0]);
    if (ret != 0) return 1;

    events[1] = (struct epoll_event){
    .events = EPOLLPRI, // add interrupt on input
    .data.fd = k1,
    };
    ret = add_event(epfd, k1, &events[1]);
    if (ret != 0) return 1;

    events[2] = (struct epoll_event){
    .events = EPOLLPRI, // add interrupt on input
    .data.fd = k2,
    };
    ret = add_event(epfd, k2, &events[2]);
    if (ret != 0) return 1;

    events[3] = (struct epoll_event){
    .events = EPOLLPRI, // add interrupt on input
    .data.fd = k3,
    };
    ret = add_event(epfd, k3, &events[3]);
    if (ret != 0) return 1;

    int k = 0;
    while (1) {
        int nr = epoll_wait(epfd, events, 10, -1);
        if (nr == -1) {
            perror("Wait error");
            return -1;
        }

        for (int i = 0; i < nr; i++) {
            if (events[i].data.fd == tfd) {
                uint64_t exp;
                // clear timer event
                ssize_t s = read(tfd, &exp, sizeof(uint64_t));
                if (s == -1) {
                    perror("timer fd error");
                    return 1;
                }
                toggle_led(led, &k);
            }

            if (events[i].data.fd == k1) {
                char buf;
                pread(k1, &buf, 1, 0);
                freq += 10000000;
                set_time(tfd, &timing, freq);

                syslog(LOG_INFO, "Button k1: %c, freq: %ld\n", buf, freq);
            }

            if (events[i].data.fd == k2) {
                char buf;
                pread(k2, &buf, 1, 0);

                freq = period / 2;
                set_time(tfd, &timing, freq);

                syslog(LOG_INFO, "Button k2: %c, freq: %ld\n", buf, freq);
            }

            if (events[i].data.fd == k3) {
                char buf;
                pread(k3, &buf, 1, 0);
                freq -= 10000000;
                set_time(tfd, &timing, freq);

                syslog(LOG_INFO, "Button k3: %c, freq: %ld\n", buf, freq);
            }
        }
    }

    return 0;
}

