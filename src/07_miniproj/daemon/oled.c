#include "oled.h"

#define LINE_MAX 20
#define MANU "manu"
#define AUTO "auto"

void init_oled() {
    if (ssd1306_init() != 0) {
        exit(EXIT_FAILURE);
    }
}

void print_oled(int mode, int temp, int freq) {
    ssd1306_set_position (0,0);
    ssd1306_puts("CSEL1 - SP.07");
    print_oled_mode(mode);
    ssd1306_set_position (0,2);
    ssd1306_puts("--------------");

    print_oled_temp(temp);
    print_oled_freq(freq);
    ssd1306_set_position (0,5);
    ssd1306_puts("Duty: 50%");
}

void print_oled_mode(int mode) {
    char line[LINE_MAX];
    snprintf(line, LINE_MAX, "  Mode - %s", mode ? AUTO : MANU);
    ssd1306_set_position (0,1);
    ssd1306_puts(line);
}

void print_oled_temp(int temp) {
    char line[LINE_MAX];
    snprintf(line, LINE_MAX, "Temp: %d'C", temp);

    ssd1306_set_position (0,3);
    ssd1306_puts(line);
}

void print_oled_freq(int freq) {
    char line[LINE_MAX];
    snprintf(line, LINE_MAX, "Freq: %dHz", freq);

    ssd1306_set_position (0,4);
    ssd1306_puts(line);
}