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

static void event_button_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {
        static uint32_t cnt = 1;
        lv_obj_t * btn = lv_event_get_target(e);
        lv_obj_t * label = lv_obj_get_child(btn, 0);
        lv_label_set_text_fmt(label, "%"LV_PRIu32, cnt);
        cnt++;
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
    
    // Simple Example
    LV_IMG_DECLARE(vari_bg);
    lv_obj_t * wp = lv_img_create(lv_scr_act());
    lv_img_set_src(wp, &vari_bg);
    lv_obj_set_pos(wp, 0, 0);
    lv_obj_set_width(wp, LV_HOR_RES);

    lv_obj_t * central_btn = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(central_btn, event_button_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(central_btn, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t * label_click  = lv_label_create(central_btn);
    lv_label_set_text(label_click, "Click Me Please!");
    lv_obj_center(label_click);

    /*Handle LitlevGL tasks (tickless mode)*/
    while(1) {
        usleep(lv_timer_handler() * 1000);
    }

    return 0;
}
