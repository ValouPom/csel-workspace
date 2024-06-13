#pragma once 

#include <sys/epoll.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include "file_descriptors.h"

int multiplex_create_epoolfd(struct file_descriptors* dfs);
int multiplex_config_events(struct file_descriptors* dfs, struct epoll_event* events);