#pragma once

#include <time.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "file_descriptors.h"

int set_time(int tfd, struct itimerspec *timing, long freq);
int create_time(struct file_descriptors* dfs);
int clear_event_time(int tfd);