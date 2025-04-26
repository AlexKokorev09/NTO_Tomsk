#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "iPhone (Alexandr)";
const char* password = "sasha070309";
const char* serverURL = "http://172.20.10.4/update";

struct SensorData {
    float temp;
    float hum;
    bool motion;
} sensors;

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while(WiFi.status() != WL_CONNECTED) delay(500);
}

void sendSensorData() {
    if(WiFi.status() != WL_CONNECTED) return;

    HTTPClient http;
    http.begin(serverURL);
    http.addHeader("Content-Type", "application/json");

    // Генерация тестовых данных
    DynamicJsonDocument doc(128);
    doc["temp"] = random(200, 300)/10.0;
    doc["hum"] = random(300, 700)/10.0;
    doc["motion"] = random(0, 2);

    String json;
    serializeJson(doc, json);
    
    int response = http.POST(json);
    if(response > 0) {
        Serial.printf("Response: %d\n", response);
    }
    http.end();
}

void loop() {
    static unsigned long lastSend = 0;
    if(millis() - lastSend > 5000) {
        sendSensorData();
        lastSend = millis();
    }
    delay(100);
}