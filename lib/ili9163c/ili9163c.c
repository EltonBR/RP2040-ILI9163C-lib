#include "hardware/gpio.h"
#include "hardware/spi.h"
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

void ili9163c_send_data16(uint16_t data16) {
    uint8_t data1 = data16 >> 8;
    uint8_t data2 = data16 & 0xff;
    gpio_put(ILI9163C_DC_PIN, 1);
    spi_write_blocking(ILI9163C_SPI_PORT, &data1, 1);
    spi_write_blocking(ILI9163C_SPI_PORT, &data2, 1);
}

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

    spi_init(ILI9163C_SPI_PORT, 30e6);
    //spi_set_format(ILI9163C_SPI_PORT, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_LSB_FIRST);
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
    
    ili9163c_send_command(CMD_COLUMN_ADDRESS_SET);
    ili9163c_send_data16(x);
    ili9163c_send_data16(x);

    ili9163c_send_command(CMD_PAGE_ADDRESS_SET);
    ili9163c_send_data16(y);
    ili9163c_send_data16(y);

    ili9163c_send_command(CMD_MEMORY_WRITE);
    ili9163c_send_data16(color);

}


void ili9163c_draw_filled_rect(uint16_t x, uint16_t y, uint8_t width,uint8_t height, uint16_t fillcolor) {

    ili9163c_send_command(CMD_PAGE_ADDRESS_SET);
    ili9163c_send_data16(y);
    ili9163c_send_data16(y+height-1);
    ili9163c_send_command(CMD_COLUMN_ADDRESS_SET);
    ili9163c_send_data16(x);
    ili9163c_send_data16(x+width-1);

    ili9163c_send_command(CMD_MEMORY_WRITE);
    uint16_t total_pixels = width*height;
    for (uint16_t i = 0; i < total_pixels; i++) {
        ili9163c_send_data16(fillcolor);
    }
}

void ili9163c_fill_screen(uint16_t color) {

    ili9163c_send_command(CMD_COLUMN_ADDRESS_SET);
    ili9163c_send_data16(0x0000);
    ili9163c_send_data16(ILI9163C_SCREEN_W);

    ili9163c_send_command(CMD_PAGE_ADDRESS_SET);
    ili9163c_send_data16(0x0000);
    ili9163c_send_data16(ILI9163C_SCREEN_H);

    ili9163c_send_command(CMD_MEMORY_WRITE);

    for (uint16_t i = 0; i < ILI9163C_SCREEN_W*ILI9163C_SCREEN_H; i++) {
        ili9163c_send_data16(color);
    }
}