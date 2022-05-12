#include "TK_Led5252_16x16.h"
#include "Log.h"

TK_Led5252_16x16::TK_Led5252_16x16(HardwareSerial* uart) {
    this->uart = uart;
}

void TK_Led5252_16x16::renderScanlinePart(int16_t scanline, int16_t xmin, int16_t xmax, const uint16_t* lineBuffer) {

    if ((scanline >= Led5252_16x16_HEIGHT) || (scanline < 0))
        return;
    if ((xmin < 0) || (xmax < 0) || (xmin >= Led5252_16x16_WIDTH) || (xmax >= Led5252_16x16_WIDTH) || (xmin > xmax))
        return;
        
    for (uint16_t x = 0; x <= xmax - xmin; x++) {
        // Put the data into framebuffer
        uint16_t idx = (scanline * Led5252_16x16_WIDTH + x) * Led5252_16x16_BPP;
        buf[idx] = lineBuffer[x] >> 8;
        buf[idx + 1] = lineBuffer[x];
    }

    if(scanline == Led5252_16x16_HEIGHT -1){
      // Last line, send the data to screen
      flushFrame();
      logDebug("Uart write done");
    }
}

void TK_Led5252_16x16::flushFrame(){
      sendUartCmd(CMD1_LEDSCR, CMD2_DISPLAY_RAW, sizeof(buf), buf);
}

void TK_Led5252_16x16::sendUartCmd(uint8_t cmd1, uint8_t cmd2, uint16_t len, uint8_t* data){
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

int16_t TK_Led5252_16x16::getScreenWidth() const{
  return Led5252_16x16_WIDTH;
}
int16_t TK_Led5252_16x16::getScreenHeight() const{
  return Led5252_16x16_HEIGHT;
}