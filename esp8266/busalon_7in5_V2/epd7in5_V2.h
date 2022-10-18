/**
 *  @filename   :   epd7in5.h
 *  @brief      :   Header file for e-paper library epd7in5.cpp
 *  @author     :   Yehui from Waveshare, with modifications from Amit Ron <ish@ish.works>
 *  
 *  Copyright (C) Waveshare     August 10 2017
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

 // 2020-02-22 ish -- adding a streaming if <WiFiClient.h> was included

#ifndef EPD7IN5_V2_H
#define EPD7IN5_V2_H

#include "epdif.h"

// Display resolution
#define EPD_WIDTH       800
#define EPD_HEIGHT      480

class Epd : EpdIf {
public:
    Epd();
    ~Epd();
    int  Init(void);
    void WaitUntilIdle(void);
    void Reset(void);
    void SetLut(void);
    void DisplayFrame(const unsigned char* frame_buffer);
    void SendCommand(unsigned char command);
    void SendData(unsigned char data);
    void Sleep(void);
    void Clear(void);
    void Displaypart(const unsigned char* pbuffer, unsigned long Start_X, unsigned long Start_Y,unsigned long END_X,unsigned long END_Y);
#ifdef wificlient_h

    // Amit's note: the esp8266 chip that waveshare sell for this display doesn't have enough ram to store the entire image lmao
    //              so i had to create these two methods to stream it directly from the server to the display
    void DisplayStream(WiFiClient *client, bool finish);
    void DisplayStreamFinish();
#endif
private:
    unsigned int reset_pin;
    unsigned int dc_pin;
    unsigned int cs_pin;
    unsigned int busy_pin;
    unsigned long width;
    unsigned long height;
};

#endif /* EPD7IN5_H */

/* END OF FILE */
