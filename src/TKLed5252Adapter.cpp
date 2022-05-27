#include "TKLed5252Adapter.h"
#include "Log.h"

TKLed5252Adapter* TKLed5252Adapter::instance = NULL;

TKLed5252Adapter::TKLed5252Adapter(int16_t screenWidth, int16_t screenHeight, int8_t bpp, HardwareSerial* uart) : 
  LvglUartDisplayAdapter(screenWidth, screenHeight, bpp, uart)
{
  // Release the origin instance
  if(instance!=NULL){
    delete(instance);
  }
  // TODO: If bpp is not 8/16/24/32, need to handle specially
  buf = (uint8_t*)malloc(sizeof(uint8_t) * screenWidth * screenHeight * bpp / 8);
  disp_cb = TKLed5252Adapter::lvgl_cb;
  instance = this;
}

TKLed5252Adapter::~TKLed5252Adapter(){
  if(buf){
    free(buf);
    buf = NULL;
  }
}

/*!
 *    @brief  LVGL 显示回调接口，详见：https://docs.lvgl.io/master/porting/display.html#examples
 */
void TKLed5252Adapter::lvgl_cb(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p){
  TKLed5252Adapter::getInstance()->display(disp, area, color_p);
}

/*!
 *    @brief  完成 LVGL 显示回调的实例方法
 */
void TKLed5252Adapter::display(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p){
  int16_t width = area->x2 - area->x1 + 1;
  int16_t height = area->y2 - area->y1 + 1;
  // TODO: If input color data (lv_color_t) length is different with the screen, need to do 
  // the color space switch
  int8_t pixelBytes = getColorBit() / 8;
  if(area->x1 == 0 && area->x2 == screenWidth - 1){
    // The whole line is displayed, we can transfer the area in one copy
    memcpy(&buf[area->y1 * screenWidth * pixelBytes], (void *)color_p, width * height * pixelBytes);
  } else {
    int8_t* pdata = (int8_t*)color_p;
    for(int y = area->y1; y <= area->y2; y++){
      memcpy(&buf[(y * screenWidth + area->x1) * pixelBytes], pdata, width * pixelBytes);
      pdata += (width * pixelBytes);
    }
  }
  flushFrame();
  lv_disp_flush_ready( disp );
}

/*!
 *    @brief  将一帧显示数据通过串口送出
 */
void TKLed5252Adapter::flushFrame(){
      sendUartCmd(CMD1_LEDSCR, CMD2_DISPLAY_RAW, screenWidth * screenHeight * bpp / 8, buf);
}

/**
 * @brief 根据协议发送串口数据
 *
 * @param cmd1: 一级命令
 * @param cmd2: 二级命令
 * @param len: 数据长度
 * @param data: 数据缓冲区
 */
void TKLed5252Adapter::sendUartCmd(uint8_t cmd1, uint8_t cmd2, uint16_t len, uint8_t* data){
  uint16_t totalLen = 9 + len;
  uint8_t xorData = 0;
  // UART package header
  uint8_t header[]={PACKET_HEADER, 0, (uint8_t)(totalLen & 0xff), (uint8_t)((totalLen & 0xff00) >> 8), 
    cmd1, cmd2, PACKET_BODY};
  int i;
  for(i = 0; i < sizeof(header); i++){
    xorData ^= header[i];
  }
  // Send header
  uart->write(header, sizeof(header));
  // Send data
  if(len > 0){
    // Raw data doesn't calculate xor
    if(cmd2 != CMD2_DISPLAY_RAW){
      for(i = 0; i < len; i++){
        xorData ^= data[i];
      }
    }
    uart->write(data, len);
  }
  // Send package end sign
  uart->write(PACKET_END);
  // And xor
  uart->write(PACKET_END ^ xorData);
  uart->flush();
}