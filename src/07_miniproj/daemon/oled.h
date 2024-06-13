#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "ssd1306.h"

void init_oled();
void print_oled(int mode, int temp, int freq);
void print_oled_mode(int mode);
void print_oled_temp(int temp);
void print_oled_freq(int freq);