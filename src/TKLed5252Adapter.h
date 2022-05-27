#ifndef TKLed5252Adapter_h
#define TKLed5252Adapter_h

#include <Arduino.h>
#include "LvglUartDisplayAdapter.h"

#define Led5252_16x16_WIDTH (16)
#define Led5252_16x16_HEIGHT (16)
#define Led5252_16x16_BPP (2) 

#define PACKET_HEADER (0xAA)
#define PACKET_BODY (0xBB)
#define PACKET_END (0xCC)

//CMDS
#define CMD1_LEDSCR (1)
#define CMD1_DEV_CTRL (2)

//For CMD1_LEDSCR
#define CMD2_DISPLAY  (1)
#define CMD2_LIGHT    (2)
#define CMD2_DISPLAY_RAW  (3)

//For CMD1_DEV_CTRL
#define CMD2_SLEEP  (1)
#define CMD2_WAKEUP  (2)

/*!
 * @brief 5252 驱动的 16x16 LED 串口像素屏 LVGL 驱动
 */
class TKLed5252Adapter : public LvglUartDisplayAdapter {

public:
    TKLed5252Adapter(int16_t screenWidth, int16_t screenHeight, int8_t bpp, HardwareSerial* uart);
    virtual ~TKLed5252Adapter();
    using LvglUartDisplayAdapter::getScreenWidth;
    using LvglUartDisplayAdapter::getScreenHeight;
    using LvglUartDisplayAdapter::getColorBit;
    static TKLed5252Adapter* getInstance() {return instance;};

protected:
    static TKLed5252Adapter* instance;
    uint8_t* buf;

    void flushFrame();
    void sendUartCmd(uint8_t cmd1, uint8_t cmd2, uint16_t len, uint8_t* data);
    static void lvgl_cb( lv_disp_drv_t *, const lv_area_t *, lv_color_t * );
    void display( lv_disp_drv_t *, const lv_area_t *, lv_color_t * );
};
#endif // TKLed5252Adapter_h
