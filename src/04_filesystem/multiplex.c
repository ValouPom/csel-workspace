#include "multiplex.h"

static int add_event(int epfd, int fd, struct epoll_event *event)
{
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, event);
    if (ret == -1) {
        perror("Error epoll create");
        return 1;  // TODO add macro
    }
    return 0;
}

int multiplex_create_epoolfd(struct file_descriptors* dfs) {
    dfs->epfd = epoll_create1(0);
    if (dfs->epfd == -1) {
        perror("Error epoll create");
        return 1; // TODO add macro
    }
    return 0;
}

int multiplex_config_events(struct file_descriptors* dfs, struct epoll_event* events) {
    events[0] = (struct epoll_event){
    .events = EPOLLIN | EPOLLOUT,
    .data.fd = dfs->tfd,
    };
    int ret = add_event(dfs->epfd, dfs->tfd, &events[0]);
    if (ret != 0) return 1;

    events[1] = (struct epoll_event){
    .events = EPOLLPRI, // add interrupt on input
    .data.fd = dfs->k1,
    };
    ret = add_event(dfs->epfd, dfs->k1, &events[1]);
    if (ret != 0) return 1;

    events[2] = (struct epoll_event){
    .events = EPOLLPRI, // add interrupt on input
    .data.fd = dfs->k2,
    };
    ret = add_event(dfs->epfd, dfs->k2, &events[2]);
    if (ret != 0) return 1;

    events[3] = (struct epoll_event){
    .events = EPOLLPRI, // add interrupt on input
    .data.fd = dfs->k3,
    };
    ret = add_event(dfs->epfd, dfs->k3, &events[3]);
    if (ret != 0) return 1;

    return 0;
}