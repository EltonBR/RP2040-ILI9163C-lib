#ifndef ILI9163C_H
#define ILI9163C_H
#include "ili9163c.c"
#include <stdint.h>

void ili9163c_send_command(uint8_t command);
void ili9163c_send_data(uint8_t data);
void ili9163c_send_data16(uint16_t data16);
void ili9163c_init();
void ili9163c_set_pixel(uint16_t x, uint16_t y, uint16_t color);
void ili9163c_draw_filled_rect(uint16_t x, uint16_t y, uint8_t width,uint8_t height, uint16_t fillcolor);
void ili9163c_fill_screen(uint16_t color);

#endif // ILI9163C_H