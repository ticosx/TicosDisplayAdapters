#ifndef TK_Led5252_16x16_h
#define TK_Led5252_16x16_h

#include <Arduino.h>
#include "XRender.h"
#include "HardwareSerial.h"

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
 * @brief The XRenderer of 16x16 LED screen
 */
class TK_Led5252_16x16 : public XRenderer {

public:
    TK_Led5252_16x16(HardwareSerial* uart);
    virtual void renderScanlinePart(int16_t scanline, int16_t xmin, int16_t xmax, const uint16_t* lineBuffer);

    virtual int16_t getScreenWidth() const;
    virtual int16_t getScreenHeight() const;

protected:
    HardwareSerial* uart;
    uint8_t buf[Led5252_16x16_WIDTH * Led5252_16x16_HEIGHT * Led5252_16x16_BPP];

    void flushFrame();
    void sendUartCmd(uint8_t cmd1, uint8_t cmd2, uint16_t len, uint8_t* data);
};
#endif // TK_Led5252_16x16_h
