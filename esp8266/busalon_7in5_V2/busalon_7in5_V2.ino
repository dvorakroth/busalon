/**
 *  @filename   :   epd7in5-demo.ino
 *  @brief      :   7.5inch e-paper display demo
 *  @author     :   Yehui from Waveshare
 *
 *  Copyright (C) Waveshare     July 10 2017
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

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <SPI.h>
#include "epd7in5_V2.h"
#include "config.h"


const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;

Epd epd;
IPAddress myIP;       // IP address in your local wifi net

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
 
  // Connect to WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("\r\nIP address: ");
  Serial.println(myIP = WiFi.localIP());
}

void loop() {
  // put your main code here, to run repeatedly:

  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Wifi not connected; skipping loop()");
    return;
  }

      Serial.println("Initializing e-Paper");
  if (epd.Init() != 0) {
            Serial.println("e-Paper init failed");
            return;
        }

  Serial.println("Getting remote page...");
  HTTPClient http;
  http.begin(STOP_ENDPOINT STOP_CODE);
  int httpCode = http.GET();

  Serial.print("Resulting HTTP Code: ");
  Serial.println(httpCode);
  if (httpCode == 200) {
    int content_length = http.getSize();
    Serial.print("Got payload of length ");
    Serial.println(content_length);

    if (content_length == 48000) {
      WiFiClient *c = http.getStreamPtr();
      epd.DisplayStream(c, false);
      Serial.println("After DisplayStream");
      http.end();
      epd.DisplayStreamFinish();
      epd.Sleep();
    }
  } else {
    http.end();
    Serial.print("Got http code: ");
    Serial.println(httpCode);
    
    // TODO: show error on screen
    if (httpCode == -1) {
      // hey remember that time during may 2020 when the endpoint was down? so yeah
      epd.Clear();
    }
  }

  
    delay(50000);
}
