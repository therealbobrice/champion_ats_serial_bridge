/*
#include <Arduino.h>


void setup() {
  Serial.begin(9600); // Initialize serial communication with baud rate 115200
}

void loop() {
  Serial.println("hello world"); // Send "hello world" over serial
  delay(2000); // Wait for 2 seconds
}
*/

/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/solved-reconnect-esp32-to-wifi/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "secrets.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Replace with your network credentials (STATION)
const char* ssid = SSID;
const char* password = PASSWORD;
const char* atsAddress = ATSADDRESS;
const char* command = COMMAND;


char buf[32];

unsigned long previousMillis = 0;
unsigned long interval = 30000;


void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}
void displaytext(const char *s, int x = 0, int y = 0, bool cleardisplay = true, int color = WHITE, int textsize = 1)
{
  if (cleardisplay)
  {
    display.clearDisplay();
  }
  display.setTextSize(textsize);
  display.setTextColor(color);
  display.setCursor(x, y);
  display.println(s);
  display.display();
}

/* Empty implementation
void getData() {
  Serial.println("get_data called");
  displaytext("get_data called");
  sprintf(buf, "value = %d",2);
  displaytext(buf,0,7,false);
}
*/

void getData() {
  if (WiFi.status() == WL_CONNECTED) {
    displaytext("Getting ATS Data");
    HTTPClient http;
    WiFiClient client;
    http.begin(client, atsAddress);

    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      String payload = http.getString();
      int len = payload.length(); // figure out where to replace characters to remove closing '}'
      payload[len -2] = ','; // change '}' to ',' to add RSSI to array
      payload[len -1] = '"'; // actual last character is ' ', change to '"' for RSSI
      sprintf(buf, "RSSI\": \"%d\"}", WiFi.RSSI()); // create string `RSSI": <val>}` for JSON, treat <val> like string

      Serial.print(payload); // print payload to Serial0 but NO newline
      Serial.println(buf); //print extra RSSI info to Serial0 with newline
      displaytext("Received ATS Data");
    } else {
      Serial.print("Error getting data. HTTP response code: ");
      Serial.println(httpResponseCode);
      sprintf(buf, "HTTP Error %d", httpResponseCode);
      displaytext(buf);
    }
    http.end();
  } else {
    Serial.println("Not connected to WiFi");
  }
}

void setup() {
  Serial.begin(19200);
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
  displaytext("Connecting to wifi...");
  initWiFi();
  Serial.print("RSSI: ");
  Serial.println(WiFi.RSSI());
}

void loop() {
  unsigned long currentMillis = millis();
  // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
  if ((currentMillis - previousMillis >=interval)) {
    if ((WiFi.status() != WL_CONNECTED)) {
    Serial.print(millis());
    Serial.println("Reconnecting to WiFi...");
    displaytext("Reconnecting...");
    WiFi.disconnect();
    WiFi.reconnect();

  } else {
    //Serial.print("RSSI: ");
    //Serial.println(WiFi.RSSI());
    displaytext("Connected to");
    displaytext(ssid,0,9,false);
    sprintf(buf,"RSSI: %d",WiFi.RSSI());
    displaytext(buf,0,18,false);
    //writeText("hello");
  }
  previousMillis = currentMillis;
  }
  //from chatgpt
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input.equals(command)) {
      getData();
    }
  }
}
