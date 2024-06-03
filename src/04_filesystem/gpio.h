#pragma once 

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include "file_descriptors.h"

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

int create_config_gpios(struct file_descriptors* dfs);