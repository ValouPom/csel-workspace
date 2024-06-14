#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

void module_write_mode(int mode);
void module_read_mode(int* mode);
void module_write_freq(int freq);
void module_read_freq(int* freq);
void module_read_temp(int* temp);