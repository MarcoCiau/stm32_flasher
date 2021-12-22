#include "ESPUpdate.h"
#include <ESPDebug.h>
#include <FS.h>
const char *ssid = "Ciau";
const char *password = "EaSyNeTMilk2";
static ESPUpdate *self;
const char rootHTML[] PROGMEM =
    "<!DOCTYPE html><html><head><meta http-equiv=Content-Type content=\"text/html; charset=utf-8\"><title>ESP File Updater</title><meta name=viewport content=\"width=device-width, user-scalable=no\"><style type=text/css>body{font-family: Sans-serif; display: table-cell; vertical-align: middle;} html, body{height:100%%;} html{display: table; margin: auto;}</style></head><body><h3>Upload File</h3><small><p>Select .text file and press \"Upload\".</p></small><form enctype=multipart/form-data action=/upload method=POST><input type=file name=data><input type=submit value=Upload></form></body></html>";

#define FILE_NAME "/stm32Fw.bin"
static File fsUploadFile;
ESPUpdate::ESPUpdate(/* args */)
{
    server = NULL;
    self = this;
}

ESPUpdate::~ESPUpdate()
{
}

void ESPUpdate::connectToWiFi()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    debugSerial.println("");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    debugSerial.println("");
    debugSerial.print("Connected to ");
    debugSerial.println(ssid);
    debugSerial.print("IP address: ");
    debugSerial.println(WiFi.localIP());
}

void handleRoot()
{
    self->server->send(200, "text/plain", "ESP32 Flahser Dev Tool.");
}

void handleSPIFFS()
{
    String payload;
    Dir dir = SPIFFS.openDir("/");
    while (dir.next())
    {
        payload += dir.fileName() + (dir.isDirectory() ? " [DIR]" : String(" (") + dir.fileSize() + "b)");
        payload += "\n";
    }
    self->server->send(200, "text/plain", payload);
}

void handleBin()
{
    self->server->send(200, "text/html", FPSTR(rootHTML));
}

void handleFileUpload()
{
    if (self->server->uri() != "/upload")
        return;
    HTTPUpload &upload = self->server->upload();
    if (upload.status == UPLOAD_FILE_START)
    {
        SPIFFS.remove((char *)FILE_NAME);
        fsUploadFile = SPIFFS.open((char *)FILE_NAME, "w");
    }
    else if (upload.status == UPLOAD_FILE_WRITE)
    {
        if (fsUploadFile)
            fsUploadFile.write(upload.buf, upload.currentSize);
    }
    else if (upload.status == UPLOAD_FILE_END)
    {
        if (fsUploadFile)
            fsUploadFile.close();
    }
}

void ESPUpdate::begin()
{
    connectToWiFi();
    server = new ESP8266WebServer(80);
    server->on("/", HTTP_GET, handleRoot);
    server->on("/files", HTTP_GET, handleSPIFFS);
    server->on(
        "/upload", HTTP_POST, []()
        { self->server->send(200, "text/plain", "Uploaded OK"); },
        handleFileUpload);

    server->on("/bin", HTTP_GET, handleBin);
    ElegantOTA.begin(server); // Start ElegantOTA
    server->begin();
    debugSerial.println("HTTP server started");
}

void ESPUpdate::loop()
{
    if (server != NULL)
        server->handleClient();
}

ESPUpdate espOTA;