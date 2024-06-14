#include "module_com.h"

#define MODE_PATH "/sys/class/miniproj_class/miniproj_device/mode"
#define FREQ_PATH "/sys/class/miniproj_class/miniproj_device/freq"
#define TEMP_PATH "/sys/class/miniproj_class/miniproj_device/temp"
#define MODE_AUTO 1
#define MODE_MANUAL 0
#define MSG_LEN 30

static void module_read(char* path, int* val) {
    int fd;
    char msg[MSG_LEN];
    fd = open(path, O_RDWR);
    read(fd, msg, MSG_LEN);
    close(fd);

    sscanf(msg, "%d", val);
} 

static void module_write(char* path, int val) {
    int fd;
    char msg[MSG_LEN];
    snprintf(msg, MSG_LEN, "%d", val);
    
    fd = open(path, O_WRONLY);
    write(fd, msg, strlen(msg));
    close(fd);
} 

void module_write_mode(int mode){
    module_write(MODE_PATH, mode);
}

void module_read_mode(int* mode){
    module_read(MODE_PATH, mode);
}

void module_write_freq(int freq){
    module_write(FREQ_PATH, freq);
}

void module_read_freq(int* freq){
    module_read(FREQ_PATH, freq);
}

void module_read_temp(int* temp){
    module_read(TEMP_PATH, temp);
}