
#include "control_time.h"

int set_time(int tfd, struct itimerspec *timing, long freq) {
    timing->it_value.tv_sec  = 0;
    timing->it_value.tv_nsec = freq;
    timing->it_interval.tv_sec  = 0;
    timing->it_interval.tv_nsec = freq;

    if (timerfd_settime(tfd, 0, timing, NULL) == -1) {
        perror("Bad timer fd settime");
        return 1;
    }
    return 0;
}

int create_time(struct file_descriptors* dfs) {
    dfs->tfd = timerfd_create(CLOCK_REALTIME, 0);
    if (dfs->tfd == -1) {
        perror("Bad timer fd");
        return 1; // TODO add macro
    }
    return 0;
}

int clear_event_time(int tfd) {
    uint64_t exp;
    // clear timer event
    ssize_t s = read(tfd, &exp, sizeof(uint64_t));
    if (s == -1) {
        perror("timer fd error");
        return 1;
    }
    return 0;
}