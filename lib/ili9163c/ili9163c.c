#include "hardware/gpio.h"
#include "hardware/spi.h"
#include <stdio.h>
#include "pico/time.h"
#define ILI9163C_DC_PIN 16
#define ILI9163C_RST_PIN 17
#define ILI9163C_SPI_PORT spi0
#define ILI9163C_SPI_SCLK_PIN 18
#define ILI9163C_SPI_MOSI_PIN 19

#ifndef ILI9163C_PORTRAIT_MODE //LANDASCAPE MODE
#define ILI9163C_LANDSCAPE_MODE
#define ILI9163C_RGB_ORDER 0b10110000
#define ILI9163C_SCREEN_W 160
#define ILI9163C_SCREEN_H 128
#endif

#ifdef ILI9163C_PORTRAIT_MODE
#define ILI9163C_RGB_ORDER 0b11000000
#define ILI9163C_SCREEN_W 128
#define ILI9163C_SCREEN_H 160
#endif

#define CMD_COLUMN_ADDRESS_SET 0x2A
#define CMD_PAGE_ADDRESS_SET 0x2B
#define CMD_MEMORY_WRITE 0x2C
#define CMD_SLEEP_OUT 0x11
#define CMD_INTERFACE_PIXEL_FORMAT_16BIT_PER_PIXEL 0x3A
#define CMD_MEMORY_DATA_ACCESS_CONTROL 0x36
#define CMD_DISPLAY_ON 0x29


void ili9163c_send_command(uint8_t command) {
    gpio_put(ILI9163C_DC_PIN, 0);
    spi_write_blocking(ILI9163C_SPI_PORT, &command, 1);
}

void ili9163c_send_data(uint8_t data) {
    gpio_put(ILI9163C_DC_PIN, 1);
    spi_write_blocking(ILI9163C_SPI_PORT, &data, 1);
}

void ili9163c_init() {
    gpio_init(ILI9163C_DC_PIN);
    gpio_set_dir(ILI9163C_DC_PIN, GPIO_OUT);

    gpio_init(ILI9163C_RST_PIN);
    gpio_set_dir(ILI9163C_RST_PIN, GPIO_OUT);

    spi_init(ILI9163C_SPI_PORT, 60e6);
    gpio_set_function(ILI9163C_SPI_SCLK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(ILI9163C_SPI_MOSI_PIN, GPIO_FUNC_SPI);

    gpio_put(ILI9163C_RST_PIN, 1);
    sleep_us(1000);
    gpio_put(ILI9163C_RST_PIN, 0);
    sleep_us(1000);
    gpio_put(ILI9163C_RST_PIN, 1);
    sleep_us(1);

    ili9163c_send_command(CMD_SLEEP_OUT);
    sleep_us(1);
    ili9163c_send_command(CMD_INTERFACE_PIXEL_FORMAT_16BIT_PER_PIXEL);
    ili9163c_send_data(0x05);
    ili9163c_send_command(CMD_MEMORY_DATA_ACCESS_CONTROL);
    ili9163c_send_data(ILI9163C_RGB_ORDER);
    ili9163c_send_command(CMD_DISPLAY_ON);
}

void ili9163c_set_pixel(uint16_t x, uint16_t y, uint16_t color) {
    uint8_t high_byte = (color >> 8) & 0xFF;
    uint8_t low_byte = color & 0xFF;
    
    ili9163c_send_command(CMD_COLUMN_ADDRESS_SET); // Column Address Set
    ili9163c_send_data(x >> 8);
    ili9163c_send_data(x & 0xFF);
    ili9163c_send_data(x >> 8);
    ili9163c_send_data(x & 0xFF);

    ili9163c_send_command(CMD_PAGE_ADDRESS_SET); // Page Address Set
    ili9163c_send_data(y >> 8);
    ili9163c_send_data(y & 0xFF);
    ili9163c_send_data(y >> 8);
    ili9163c_send_data(y & 0xFF);
    ili9163c_send_command(CMD_MEMORY_WRITE); // Memory Write
    ili9163c_send_data(high_byte);
    ili9163c_send_data(low_byte);
}


void ili9163c_draw_rect_filled(uint16_t x, uint16_t y, uint8_t width,uint8_t height, uint16_t fillcolor) {
    uint8_t high_byte = (fillcolor >> 8) & 0xFF;
    uint8_t low_byte = fillcolor & 0xFF;

    ili9163c_send_command(CMD_PAGE_ADDRESS_SET); // Column Address Set
    ili9163c_send_data(y >> 8);
    ili9163c_send_data(y & 0xFF);
    ili9163c_send_data((y+height-1) >> 8);
    ili9163c_send_data((y+height-1) & 0xFF);

    ili9163c_send_command(CMD_COLUMN_ADDRESS_SET); // Page Address Set
    ili9163c_send_data(x >> 8);
    ili9163c_send_data(x & 0xFF);
    ili9163c_send_data((x+width-1) >> 8);
    ili9163c_send_data((x+width-1) & 0xFF);

    ili9163c_send_command(0x2C); // Memory Write
    uint16_t total_pixels = width*height;
    for (uint16_t i = 0; i < total_pixels; i++) {
        ili9163c_send_data(high_byte);
        ili9163c_send_data(low_byte);
    }
}


void ili9163c_fill_screen(uint16_t color) {
    uint8_t high_byte = (color >> 8) & 0xFF;
    uint8_t low_byte = color & 0xFF;

    ili9163c_send_command(CMD_COLUMN_ADDRESS_SET);
    ili9163c_send_data(0x00);
    ili9163c_send_data(0x00);
    ili9163c_send_data(0x00);
    ili9163c_send_data(ILI9163C_SCREEN_W);

    ili9163c_send_command(CMD_PAGE_ADDRESS_SET);
    ili9163c_send_data(0x00);
    ili9163c_send_data(0x00);
    ili9163c_send_data(0x00);
    ili9163c_send_data(ILI9163C_SCREEN_H);

    ili9163c_send_command(0x2C);

    for (uint16_t i = 0; i < ILI9163C_SCREEN_W*ILI9163C_SCREEN_H; i++) {
        ili9163c_send_data(high_byte);
        ili9163c_send_data(low_byte);
    }
}


void ili9163c_vertical_scroll(uint16_t start_row, uint16_t end_row, uint16_t scroll_lines) {
    ili9163c_send_command(0x33); // Vertical Scroll Definition
    ili9163c_send_data(start_row >> 8);
    ili9163c_send_data(start_row & 0xFF);
    ili9163c_send_data(end_row >> 8);
    ili9163c_send_data(end_row & 0xFF);
    ili9163c_send_data(scroll_lines >> 8);
    ili9163c_send_data(scroll_lines & 0xFF);

    ili9163c_send_command(0x37); // Vertical Scroll Start Address
    ili9163c_send_data(start_row >> 8);
    ili9163c_send_data(start_row & 0xFF);
}