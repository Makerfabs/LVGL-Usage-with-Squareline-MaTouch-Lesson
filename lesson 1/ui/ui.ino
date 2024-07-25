//Makerfabs MA1989BRK board-template main source file

#include <Arduino_GFX_Library.h>
#define TOUCH_MODULES_CST_SELF
#include <TouchLib.h>
#include <lvgl.h>
#include <ui.h>

/*Don't forget to set Sketchbook location in File/Preferences to the path of your UI project (the parent foder of this INO file)*/

#define LED_PIN  2
int LED_state=0;

enum BoardConstants {
    LVGL_BUFFER_RATIO = 6,
    PIN_LCD_BL=38,
    PIN_LCD_D0=39, PIN_LCD_D1=40, PIN_LCD_D2=41, PIN_LCD_D3=42, PIN_LCD_D4=45, PIN_LCD_D5=46, PIN_LCD_D6=47, PIN_LCD_D7=48,
    PIN_LCD_RST=1, PIN_LCD_CS=6, PIN_LCD_DC=7, //RS
    PIN_LCD_WR=8, PIN_LCD_RD=9,
    PIN_IIC_SCL=17, PIN_IIC_SDA=18,  PIN_TOUCH_INT=16, PIN_TOUCH_RES=21,
    PIN_BUTTON_1=0, PIN_BUTTON_2=14, PIN_BAT_VOLT=4
};


static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 170;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf [screenWidth * screenHeight / LVGL_BUFFER_RATIO];


Arduino_DataBus *bus = new Arduino_ESP32PAR8( PIN_LCD_DC, PIN_LCD_CS, PIN_LCD_WR, PIN_LCD_RD,
                                              PIN_LCD_D0, PIN_LCD_D1, PIN_LCD_D2, PIN_LCD_D3, PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7 );

Arduino_GFX *gfx = new Arduino_ST7789( bus, 5 /* RST */, 0 /* rotation */, true /* IPS */, 170 /* width */, 320 /* height */, 35 /* col offset 1 */,
                                       0 /* row offset 1 */, 35 /* col offset 2 */, 0 /* row offset 2 */ );

TouchLib touch( Wire, PIN_IIC_SDA, PIN_IIC_SCL, CTS820_SLAVE_ADDRESS, PIN_TOUCH_RES );


void setup ()
{
    pinMode( LED_PIN, OUTPUT );
    
    pinMode( PIN_LCD_BL, OUTPUT );
    digitalWrite( PIN_LCD_BL, HIGH );

    pinMode( PIN_LCD_RST, OUTPUT );
    digitalWrite( PIN_LCD_RST, HIGH );


    pinMode( PIN_TOUCH_RES, OUTPUT );
    digitalWrite( PIN_TOUCH_RES, LOW );
    delay( 500 );
    digitalWrite( PIN_TOUCH_RES, HIGH );

    Wire.begin( PIN_IIC_SDA, PIN_IIC_SCL );
    if ( !touch.init() ) {
        Serial.println("Touch IC not found");
    }


    gfx->begin();
    gfx->setRotation( 1 );


    String LVGL_Arduino = "Hello Arduino! ";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();


    lv_init();
    lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * screenHeight / LVGL_BUFFER_RATIO );

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


    xTaskCreatePinnedToCore( Task_TFT, "Task_TFT", 20480, NULL, 3, NULL, 0 );
}


void loop ()
{
  if(LED_state==0){digitalWrite( LED_PIN, LOW );}
  else {digitalWrite( LED_PIN, HIGH );}
}


// -------------------------------------------------------------------------------------

void Task_TFT (void *pvParameters)
{
    while (1)
    {
        lv_timer_handler();
        vTaskDelay(10);
    }
}


//------------------------------------------------------------------------

/* Display flushing */
void my_disp_flush (lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

#if (LV_COLOR_16_SWAP != 0)
    gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t*) &color_p->full, w, h);
#else
    gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t*) &color_p->full, w, h);
#endif

    lv_disp_flush_ready(disp);
}


/*Read the touchpad*/
void my_touchpad_read (lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    uint16_t touchX = 0, touchY = 0;

    if ( touch.read() ) {
        data->state = LV_INDEV_STATE_PR;

        uint8_t n = touch.getPointNum();
        for (uint8_t i = 0; i < n; i++)
        {
            TP_Point t = touch.getPoint( i );
            int temp_x = t.y;
            int temp_y = map( t.x, 0, 150, 170, 0 );

            // USBSerial.println(temp_x);
            // USBSerial.println(temp_y);

            data->point.x = temp_x;
            data->point.y = temp_y;
            break;
        }
    }
    else {
        data->state = LV_INDEV_STATE_REL;
    }
}
