#include "gpio.h"


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
    f = open(cfg_pin_value_path, O_RDWR);
    return f;
}

int create_config_gpios(struct file_descriptors* dfs) {
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

    dfs->led = open_gpio(&led_gpio);
    if (dfs->led == -1) {
        perror("Cannot open gpio");
        return 1;
    }

    dfs->k1 = open_gpio(&k1_gpio);
    if (dfs->k1 == -1) {
        perror("Cannot open gpio k1");
        return 1;
    }

    dfs->k2 = open_gpio(&k2_gpio);
    if (dfs->k2 == -1) {
        perror("Cannot open gpio k2");
        return 1;
    }

    dfs->k3 = open_gpio(&k3_gpio);
    if (dfs->k3 == -1) {
        perror("Cannot open gpio k3");
        return 1;
    }

    return 0;
}