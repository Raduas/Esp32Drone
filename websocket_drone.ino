/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-controls-servo-motor-via-web
 */
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include "index.h"
#include "droneDrive.h"
#include "droneControl.h"


const char* ssid = "PODA_8643";     // CHANGE IT
const char* password = "6E982DA3";  // CHANGE IT
unsigned long lastTime = 0;
int8_t power, yaw, pitch, roll;
const int8_t FL = 16; //PWN Pin for Front Left motor
const int8_t FR = 17; //PWN Pin for Front Right motor
const int8_t RL = 4; //PWN Pin for Rear Left motor
const int8_t RR = 2; //PWN Pin for Rear Right motor
int16_t pwmFL, pwmFR, pwmRL, pwmRR;

// setting PWM properties
const int freq = 5000;
const int resolution = 8;

Adafruit_MPU6050 mpu; //init IMU
AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);  // WebSocket server on port 81

void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] Received text: %s\n", num, payload);
      partitionInputString((char*)payload, &power, &yaw, &pitch, &roll);
      Serial.printf("Power: %i,Yaw: %i, Pitch %i Roll: %i\n ", power, yaw, pitch, roll);
    } 
}

void setup() {
  Serial.begin(115200);
 /*ledcSetup(0, freq, resolution);
  ledcAttachPin(FL, 0); ledcAttachPin(FR, 1); 
  ledcAttachPin(RL, 2); ledcAttachPin(RR, 3);*/
  pinMode(FL, OUTPUT); pinMode(FR, OUTPUT);
  pinMode(RL, OUTPUT); pinMode(RR, OUTPUT);
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  //mpu test
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");
  //setting mpu
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G); //Sett acc range 8G
  mpu.setGyroRange(MPU6050_RANGE_250_DEG); 
  mpu.setFilterBandwidth(MPU6050_BAND_10_HZ);

  // Initialize WebSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  // Serve a basic HTML page with JavaScript to create the WebSocket connection
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("Web Server: received a web page request");
    String html = HTML_CONTENT;  // Use the HTML content from the index.h file
    request->send(200, "text/html", html);
  });

  server.begin();
  Serial.print("ESP32 Web Server's IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() { 
webSocket.loop();

valsToPWM(power, yaw, pitch, roll, &pwmFL, &pwmFR, &pwmRL, &pwmRR);
delay(10);
Serial.printf("pwm %i",pwmRL);
/*ledcWrite(0, pwmFL);ledcWrite(1, pwmFR);
ledcWrite(3, pwmRL);ledcWrite(4, pwmRR);*/
analogWrite(FL, pwmFL); analogWrite(FR, pwmFR);  
analogWrite(RL, pwmRL); analogWrite(RR, pwmRR);
}
