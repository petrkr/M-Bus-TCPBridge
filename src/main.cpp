/*
  Arduino SDK 3.3.x
  ESP IDF 5.5.x
*/
#include <board_config.h>
#include <WiFi.h>
#include <log4mcu.h>
#include <secrets.h>
#include "OtaControl.h"


static log4mcu::Logger& logger = log4mcu::Logger::get("PortfolioESPlink");
static log4mcu::SerialLogAppender serialAppender(Serial);

NetworkServer server(1234);

void WiFiEvent(WiFiEvent_t event){
    switch(event) {
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
            Serial.println("STA Connected");
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            Serial.print("STA IP: ");
            Serial.println(WiFi.localIP());
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
            Serial.print("STA IPv6: ");
            Serial.println(WiFi.linkLocalIPv6());
            Serial.println(WiFi.globalIPv6());
            break;
        default:
            break;
    }
}

void setup() {
  //Serial.setTxTimeoutMs(0); // Does not block Tx if USB-CDC is used and not connected
  Serial.begin(115200);
  delay(250);

  log4mcu::Logger::setAppender(&serialAppender);
  log4mcu::Logger::setGlobalMinLevel(log4mcu::LogLevel::Debug);

  logger.info("M-Bus TCP Bridge - (c) 2026 by Petr Kracik");

  logger.debug("Initializing M-Bus serial");
  Serial2.begin(2400, SERIAL_8E1, MBUS_RX, MBUS_TX);

  logger.debug("Initializing Wifi subsystem");
  WiFi.onEvent(WiFiEvent);
  WiFi.enableIPv6();
  WiFi.mode(WIFI_STA);

  logger.info("Connecting to wifi...");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  
  logger.info("Connected to WiFi");

  otaControlEnable();

  server.begin();
  logger.info("M-Bus TCP Server started");
}

void loop() {
 NetworkClient client = server.accept();

 if (client) {                             // if you get a client,
    Serial.print("New Client.");           // print a message out the serial port
    logger.info("New client connected ");
    logger.info(String(client.remoteIP()).c_str());        // print a message out the serial port
    String currentLine = "";               // make a String to hold incoming data from the client
    while (client.connected()) {           // loop while the client's connected
      if (client.available()) {            // if there's bytes to read from the client,
        char c = client.read();
        Serial2.write(c);
        logger.debug("TCP READ: ");
        Serial.println(c, HEX);
      }
 
      if (Serial2.available()) {
          char sr = Serial2.read();
          logger.debug("SER READ: ");
          Serial.println(sr, HEX);
          client.write(sr);
        }
    }
    // close the connection:
    client.stop();
    logger.info("Client Disconnected.");
  }
}
