//#define ILI9163C_PORTRAIT_MODE
#include "ili9163c/ili9163c.h"
#include<stdlib.h>
int main() {
    ili9163c_init();
    ili9163c_fill_screen(0xF000);
    while(1) {
        ili9163c_draw_filled_rect(10, 0, 20, 120, 0x00FF);
        sleep_ms(10);
        ili9163c_set_pixel(random()%128, random()%128, random()%65000);
    }
    return 0;
}