#include "module_com.h"

#define MODE_PATH "/sys/kernel/led_blink_control/mode"
#define FREQ_PATH "/sys/kernel/led_blink_control/frequency"
#define MODE_AUTO "automatic"
#define MODE_MANUAL "manual"
#define MSG_LEN 30

void module_write_mode(int mode){
    int fd;
    char msg[MSG_LEN];
    snprintf(msg, MSG_LEN, "%s", mode ? MODE_AUTO : MODE_MANUAL);
    
    fd = open(MODE_PATH, O_WRONLY);
    write(fd, msg, strlen(msg));
    close(fd);
}

void module_read_mode(int* mode){
    int fd;
    char msg[MSG_LEN];
    fd = open(MODE_PATH, O_RDWR);
    read(fd, msg, MSG_LEN);
    close(fd);

    if (msg[0] == MODE_AUTO[0]) {
        *mode = 1;
    }
    else {
        *mode = 0;
    }
}

void module_write_freq(int freq){
    int fd;
    char msg[MSG_LEN];
    snprintf(msg, MSG_LEN, "%d", freq);
    
    fd = open(FREQ_PATH, O_WRONLY);
    write(fd, msg, strlen(msg));
    close(fd);
}

void module_read_freq(int* freq){
    int fd;
    char msg[MSG_LEN];
    fd = open(FREQ_PATH, O_RDWR);
    read(fd, msg, MSG_LEN);
    close(fd);

    sscanf(msg, "%d", freq);
}