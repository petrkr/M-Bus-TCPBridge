#include <WiFi.h>
#include "WiFiServer6.h"

const char* ssid = "";
const char* password = "";

WiFiServer6 server6(1234);

void WiFiEvent(WiFiEvent_t event){
    switch(event) {
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
            Serial.println("STA Connected");
            WiFi.enableIpV6();
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
            Serial.print("STA IPv6: ");
            Serial.println(WiFi.localIPv6());
            Serial.println(WiFi.globalIPv6());
            break;
        default:
            break;
    }
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(2400, SERIAL_8E1, 36, 4);
 
  WiFi.onEvent(WiFiEvent);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println("Connected to WiFi");

  server6.begin();
  Serial.println("Server started");
}

void loop() {
 WiFiClient client6 = server6.available();

 if (client6) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client6.connected()) {            // loop while the client's connected
      if (client6.available()) {             // if there's bytes to read from the client,
        char c = client6.read();
        Serial2.write(c);
        Serial.write("TCP READ: ");
        Serial.println(c, HEX);
      }
 
      if (Serial2.available()) {
          char sr = Serial2.read();
          Serial.write("SER READ: "); Serial.println(sr, HEX);
          client6.write(sr);
        }
    }
    // close the connection:
    client6.stop();
    Serial.println("Client Disconnected.");
  }
}
