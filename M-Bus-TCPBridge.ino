/*
  Arduino SDK 3.3.x
  ESP IDF 5.5.x
*/

#include <WiFi.h>

const char* ssid = "";
const char* password = "";

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
  Serial.begin(115200);
  Serial2.begin(2400, SERIAL_8E1, 36, 4);
 
  WiFi.onEvent(WiFiEvent);
  WiFi.enableIPv6();
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println("Connected to WiFi");

  server.begin();
  Serial.println("Server started");
}

void loop() {
 NetworkClient client = server.available();

 if (client) {                             // if you get a client,
    Serial.print("New Client.");           // print a message out the serial port
    Serial.println(client.remoteIP());     // print a message out the serial port
    String currentLine = "";               // make a String to hold incoming data from the client
    while (client.connected()) {           // loop while the client's connected
      if (client.available()) {            // if there's bytes to read from the client,
        char c = client.read();
        Serial2.write(c);
        Serial.write("TCP READ: ");
        Serial.println(c, HEX);
      }
 
      if (Serial2.available()) {
          char sr = Serial2.read();
          Serial.write("SER READ: "); Serial.println(sr, HEX);
          client.write(sr);
        }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
