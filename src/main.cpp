#include <ArduinoJson.h>
#include <WebServer.h>
#include <WebSocketsClient.h>
#include <WiFi.h>

// Wifi Credentials
const char *ssid = "";     // Wifi SSID
const char *password = ""; // Wifi Password

// Websocket server location
const int wsServerPort = 3001;                 // Websocket server port
const char *wsServerIp = "192.168.68.142";     // Websocket server's host ip
const char *wsServerPath = "/heaterGpioState"; // Websocket server path

// GPIO
const int thermoPin = 33;    // Thermostat GPIO pin
const int heaterPin = 32;    // Heater GPIO pin
WebSocketsClient webSocket;  // Websocket client instance
StaticJsonDocument<100> doc; // Allocate a static JSON document

void turnHeaterOn()
{
  digitalWrite(LED_BUILTIN, HIGH); // Led on
  digitalWrite(heaterPin, HIGH);   // Heat on
  Serial.println("Heater on");     // Print action log
}

void turnHeaterOff()
{
  digitalWrite(LED_BUILTIN, LOW); // Led off
  digitalWrite(heaterPin, LOW);   // Heat off
  Serial.println("Heater off");   // Print action log
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
  Serial.println("webSocketEvent type: " + String(type));
  if (type == WStype_TEXT)
  {
    DeserializationError error = deserializeJson(doc, payload); // deserialize incoming Json String
    if (error)
    { // Print erro msg if incoming String is not JSON formatted
      Serial.println("JSON DeserializationError: " + String(error.c_str()));
      return;
    }

    const boolean isOn = doc["isOn"];
    Serial.println("isOn: " + String(isOn));
    if (isOn == true)
      turnHeaterOn();
    else
      turnHeaterOff();
  }
  Serial.println("---");
}

void setup()
{
  Serial.begin(115200);       // Connect to Terminal
  WiFi.begin(ssid, password); // Connect to WiFi
  Serial.println("");
  Serial.print("WiFi Connecting");
  while (WiFi.status() != WL_CONNECTED) // Waiting on Wifi
  {
    Serial.print(".");
    delay(500);
  }

  Serial.println();                                      // Advance terminal line
  Serial.println("WiFi Connected!");                     //
  Serial.println("Local IP: " + String(WiFi.localIP())); // Print action log
  // delay(2000);                                           // Pause 2s

  webSocket.begin(wsServerIp, wsServerPort, wsServerPath); // Connect to ws server
  webSocket.onEvent(webSocketEvent);                       // Handle ws events
  webSocket.setReconnectInterval(5000);                    // Handle connection failure, retry every 5s

  pinMode(heaterPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println("---");
}
void loop()
{
  webSocket.loop(); // Keep the socket alive
}
