// #include <Arduino.h>
// #include <ESP8266WiFi.h>
// #include <WiFiClient.h>
// #include <ESP8266WebServer.h>
// #include <ElegantOTA.h>

// #include <ESPDebug.h>
// #include <stm32Programmer.h>
// #include <stm32ota.h>
// #include <FS.h>
// #define FILE_NAME "/stm32Fw.bin"
// const char* ssid = "Ciau";
// const char* password = "EaSyNeTMilk2";

// ESP8266WebServer server(80);
// static File fsUploadFile;
// #define MEDIUM_DENSITY_PID 0x410

// enum PROCESS {NONE = 0, INIT, SEND_SYNC, GET_PID, SEND_WRITE, PROG_ERR, PROG_OK};
// int stm32UpdateStep = NONE;

// unsigned long timer = 0;
// void init_wifi_server();


// const char rootHTML[] PROGMEM =
// "<!DOCTYPE html><html><head><meta http-equiv=Content-Type content=\"text/html; charset=utf-8\"><title>ESP File Updater</title><meta name=viewport content=\"width=device-width, user-scalable=no\"><style type=text/css>body{font-family: Sans-serif; display: table-cell; vertical-align: middle;} html, body{height:100%%;} html{display: table; margin: auto;}</style></head><body><h3>Upload File</h3><small><p>Select .text file and press \"Upload\".</p></small><form enctype=multipart/form-data action=/upload method=POST><input type=file name=data><input type=submit value=Upload></form></body></html>"
// ;

// void handleBin()
// {
//   server.send(200, "text/html", FPSTR(rootHTML));
// }

// void handleFileUpload()
// {
//   if (server.uri() != "/upload") return;
//   HTTPUpload& upload = server.upload();
//   if(upload.status == UPLOAD_FILE_START){
//     SPIFFS.remove((char*)FILE_NAME);
//     fsUploadFile = SPIFFS.open((char*)FILE_NAME, "w");
//   }
//   else if (upload.status == UPLOAD_FILE_WRITE) 
//   {
//     if (fsUploadFile) fsUploadFile.write(upload.buf, upload.currentSize);
//   } 
//   else if (upload.status == UPLOAD_FILE_END) 
//   {
//     if (fsUploadFile)fsUploadFile.close();
//   }
// }

// void handleSPIFFS() 
// {
//   String payload;
//   Dir dir = SPIFFS.openDir("/");
//   while (dir.next()) 
//   {
//     payload += dir.fileName() + (dir.isDirectory() ? " [DIR]" : String(" (") + dir.fileSize() + "b)");
//     payload += "\n";
//   }
//   server.send(200, "text/plain", payload);
// }

// void setup() {
//   // put your setup code here, to run once:
//   SPIFFS.begin();
//   debugSerialBegin(9600);
//   Serial.begin(115200, SERIAL_8E1);
//   init_wifi_server();
//   STM32Prog.begin();
//   timer =  millis();
//   STM32Prog.startNormalMode();
// }

// /* Enter STM32 in boot mode and send init command */
// void sync()
// {
//   STM32Prog.startNormalMode();
//   delay(250);
//   STM32Prog.startFlashMode();
//   delay(250);
//   Serial.write(STM32INIT);
// }

// void loop() {
//   server.handleClient();
//   /*
//   sync stm32
//   wait for ack
//   get pid to validate mcu
//   send write command
//   wait for ack
//   */
//   if (stm32UpdateStep == INIT)
//   {
//     sync();
//     stm32UpdateStep =  SEND_SYNC;
//   }

//   if (stm32UpdateStep == SEND_SYNC && waitingForACK()) stm32UpdateStep = GET_PID;
  
//   if (stm32UpdateStep == GET_PID)
//   {
//       stm32SendCommand(STM32ID);
//       bool resp = waitingForACK();
//       if (!resp) {
//         debugSerial.println("Error get id NACK!");
//         return;
//       }
//       unsigned char sbuf[5];
//       Serial.readBytesUntil(STM32ACK, sbuf, 4);
//       int deviceID = 0;
//       deviceID = sbuf[1];
//       deviceID = (deviceID << 8) + sbuf[2];
//       if (deviceID == MEDIUM_DENSITY_PID) debugSerial.println("STM32F10XX Medium Density MCU.");
//       stm32UpdateStep = SEND_WRITE;
//   }

//   if (stm32UpdateStep == SEND_WRITE)
//   {
//     int result = STM32Prog.program((char*)FILE_NAME);
//     debugSerial.print("Flash Update Status ");
//     debugSerial.println(result);
//     if (result == 4)
//     {
//       stm32UpdateStep = PROG_OK;
//       STM32Prog.startNormalMode();
//     } 
//     else stm32UpdateStep = PROG_ERR;
//   }

//   if (millis() - timer > 10000) {
//     if (stm32UpdateStep == NONE ) stm32UpdateStep = INIT;
//     timer = millis();
//   }
  
// }

// void init_wifi_server() {
//   WiFi.mode(WIFI_STA);
//   WiFi.begin(ssid, password);
//   debugSerial.println("");

//   // Wait for connection
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     debugSerial.print(".");
//   }
//   debugSerial.println("");
//   debugSerial.print("Connected to ");
//   debugSerial.println(ssid);
//   debugSerial.print("IP address: ");
//   debugSerial.println(WiFi.localIP());

//   server.on("/", []() {
//     server.send(200, "text/plain", "Hi! I am ESP8266.");
//   });

//     server.on("/upload", HTTP_POST, []() {
//     server.send(200, "text/plain", "Uploaded OK");
//   }, handleFileUpload);

//   server.on("/bin", HTTP_GET, handleBin);

//   server.on("/spiffs", HTTP_GET, handleSPIFFS);

//   ElegantOTA.begin(&server);    // Start ElegantOTA
//   server.begin();
//   debugSerial.println("HTTP server started");
// }