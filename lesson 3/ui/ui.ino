//Makerfabs E32S3RGB40 board-template main source file

#include <Arduino_GFX_Library.h>
#include <lvgl.h>
#include <ui.h>
#include <Wire.h>
#include "AS5600.h"

#define SDA_PIN 17
#define SCL_PIN 18

AMS_5600 ams5600;
/*Don't forget to set Sketchbook location in File/Preferences to the path of your UI project (the parent foder of this INO file)*/


enum BoardConstants { GFX_BL=-1, LVGL_BUFFER_RATIO=6 };


static const uint16_t screenWidth = 480;
static const uint16_t screenHeight = 480;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf;

Arduino_ESP32RGBPanel *bus = new Arduino_ESP32RGBPanel(
  1 /* CS */, 12 /* SCK */, 11 /* SDA */,
  45 /* DE */, 4/* VSYNC */, 5 /* HSYNC */, 21 /* PCLK */,
  39 /* R0 */, 40 /* R1 */, 41 /* R2 */, 42 /* R3 */, 2 /* R4 */,
  0 /* G0/P22 */, 9 /* G1/P23 */, 14 /* G2/P24 */, 47 /* G3/P25 */, 48 /* G4/P26 */, 3 /* G5 */,
  6 /* B0 */, 7 /* B1 */, 15 /* B2 */, 16 /* B3 */, 8 /* B4 */
);

Arduino_ST7701_RGBPanel *gfx = new Arduino_ST7701_RGBPanel(
  bus, GFX_NOT_DEFINED /* RST */, 0 /* rotation */,
  true /* IPS */, 480 /* width */, 480 /* height */,
  st7701_type1_init_operations, sizeof(st7701_type1_init_operations),
  true /* BGR */);


#include "touch.h"


/* Display flushing */
void my_disp_flush (lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

#if (LV_COLOR_16_SWAP != 0)
    gfx->draw16bitBeRGBBitmap( area->x1, area->y1, (uint16_t*) &color_p->full, w, h );
#else
    gfx->draw16bitRGBBitmap( area->x1, area->y1, (uint16_t*) &color_p->full, w, h );
#endif

    lv_disp_flush_ready(disp);
}


/*Read the touchpad*/
void my_touchpad_read (lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    if ( touch_has_signal() ) {
        if ( touch_touched() ) {
            data->state = LV_INDEV_STATE_PR;
            /*Set the coordinates*/
            data->point.x = touch_last_x;
            data->point.y = touch_last_y;
        }
        else if ( touch_released() ) {
            data->state = LV_INDEV_STATE_REL;
        }
    }
    else {
        data->state = LV_INDEV_STATE_REL;
    }
}


void setup ()
{
    Serial.begin(115200); /* prepare for possible serial debug */

    Wire.begin(SDA_PIN, SCL_PIN);

    String LVGL_Arduino = "Hello Arduino! ";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

    // Init touch device
    touch_init();

    // Init Display
    gfx->begin();
    gfx->fillScreen(BLACK);

    pinMode(GFX_BL, OUTPUT);
    digitalWrite(GFX_BL, HIGH);

    lv_init();
    //screenWidth = gfx->width(); screenHeight = gfx->height();

#ifdef ESP32
    disp_draw_buf = (lv_color_t *) heap_caps_malloc( sizeof(lv_color_t) * screenWidth * LVGL_BUFFER_RATIO, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT );
#else
    disp_draw_buf = (lv_color_t *) malloc( sizeof(lv_color_t) * screenWidth * LVGL_BUFFER_RATIO );
#endif
    if (!disp_draw_buf) {
        Serial.println("LVGL disp_draw_buf allocate failed!");
    }
    else
    {
        lv_disp_draw_buf_init( &draw_buf, disp_draw_buf, NULL, screenWidth * LVGL_BUFFER_RATIO );

        /*Initialize the display*/
        static lv_disp_drv_t disp_drv;
        lv_disp_drv_init( &disp_drv );
        /*Change the following line to your display resolution*/
        disp_drv.hor_res = screenWidth;
        disp_drv.ver_res = screenHeight;
        disp_drv.flush_cb = my_disp_flush;
        disp_drv.draw_buf = &draw_buf;
        lv_disp_drv_register( &disp_drv );

        /*Initialize the (dummy) input device driver*/
        static lv_indev_drv_t indev_drv;
        lv_indev_drv_init( &indev_drv );
        indev_drv.type = LV_INDEV_TYPE_POINTER;
        indev_drv.read_cb = my_touchpad_read;
        lv_indev_drv_register( &indev_drv );

        ui_init();

        Serial.println( "Setup done" );
    }
}

/*******************************************************
/* Function: convertRawAngleToDegrees
/* In: angle data from AMS_5600::getRawAngle
/* Out: human readable degrees as float
/* Description: takes the raw angle and calculates
/* float value in degrees.
/*******************************************************/
float convertRawAngleToDegrees(word newAngle)
{
    /* Raw data reports 0 - 4095 segments, which is 0.087890625 of a degree */
    float retVal = newAngle * 0.087890625;
    return retVal;
}

void loop ()
{
    lv_timer_handler();
    delay(10); //together with LV_INDEV_DEF_READ_PERIOD it should be bigger than LV_DISP_DEF_REFR_PERIOD, to avoid button-press glitches (e.g. in Smart Gadget example)
    Serial.println(String(convertRawAngleToDegrees(ams5600.getRawAngle()), DEC));
    lv_img_set_angle(ui_Image1, convertRawAngleToDegrees(ams5600.getRawAngle())*10);
}
