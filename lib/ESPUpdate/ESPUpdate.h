#ifndef ESP_UPDATE_H
#define ESP_UPDATE_H
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ElegantOTA.h>
class ESPUpdate
{
private:
    /* data */
    void connectToWiFi();
public:
    ESP8266WebServer *server;
    ESPUpdate(/* args */);
    ~ESPUpdate();
    void begin();
    void loop();
};
extern ESPUpdate espOTA;
#endif