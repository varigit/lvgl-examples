#include "lvgl/lv_conf.h"
#include "lvgl/lvgl.h"
#include "lvgl/lv_drivers/display/fbdev.h"
#include "lvgl/lv_drivers/display/drm.h"
#include "lvgl/lv_drivers/indev/evdev.h"
#include <unistd.h>
#include <stdio.h>

extern void lv_example_main_gui();

#define DISP_BUF_SIZE (128 * 1024)

static void fbdev_disp_drv_init(void)
{
    static lv_color_t buf1[DISP_BUF_SIZE];
    static lv_color_t buf2[DISP_BUF_SIZE];
    static lv_disp_draw_buf_t disp_buf;
    static lv_disp_drv_t disp_drv;
    uint32_t width, height, dpi;

    /*Linux frame buffer device init*/
    fbdev_init();
    fbdev_get_sizes(&width, &height, &dpi);

    /*Initialize a descriptor for the buffer*/
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, DISP_BUF_SIZE / (LV_COLOR_SIZE/8));

    /*Initialize and register a display driver*/
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf   = &disp_buf;
    disp_drv.flush_cb   = fbdev_flush;
    disp_drv.hor_res    = width;
    disp_drv.ver_res    = height;
    lv_disp_drv_register(&disp_drv);
}

static void evdev_indev_drv_init(char *evdev_name)
{
    static lv_indev_drv_t indev_drv_1;

    if (evdev_name == NULL) {
        /* fallback to a default */
        evdev_name = "/dev/input/event0";
    }
    evdev_set_file(evdev_name);

    lv_indev_drv_init(&indev_drv_1); /*Basic initialization*/
    indev_drv_1.type = LV_INDEV_TYPE_POINTER;

    /*This function will be called periodically (by the library) to get the mouse position and state*/
    indev_drv_1.read_cb = evdev_read;
    if (lv_indev_drv_register(&indev_drv_1) == NULL) {
        perror("lv_indev_drv_register: failed to register input device");
    }
}

int main(int argc, char **argv)
{
    /*LittlevGL init*/
    lv_init();

    /*Platform init*/
    fbdev_disp_drv_init();
    
    /*Input system init*/
    evdev_indev_drv_init(NULL);
    
    //Full Demo.
    lv_example_main_gui();

    /*Handle LitlevGL tasks (tickless mode)*/
    while(1) {
        usleep(lv_timer_handler() * 1000);
    }

    return 0;
}
