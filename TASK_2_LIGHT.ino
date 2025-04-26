/*
  В скетче поочередно будут включаться светодиоды: в мониторе порта можно увидеть соответствующий канал
*/

#include <Wire.h>
#include <PCA9634.h>
#include <BH1750.h>
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "TP-Link_4F90";     // Название вашей WiFi сети
const char* password = "NTOContest202324";     // Пароль от WiFi

WebServer server(80);  // Веб-сервер на порту 80

String receivedData = "Нет данных";  // Переменная для хранения данных от второго ESP
String receivedData2 = "Нет данных";  // Переменная для хранения данных от второго ESP
String receivedData3 = "Нет данных";  // Переменная для хранения данных от второго ESP
String receivedData4 = "Нет данных";  // Переменная для хранения данных от второго ESP


#define I2C_HUB_ADDR 0x70 // конфигурация портов платы MGB-I2C63
#define EN_MASK 0x08
#define DEF_CHANNEL 0x00
#define MAX_CHANNEL 0x08

PCA9634 testModule(0x1C);
BH1750 lightMeter;

PCA9634 testModule2(0x1C);
BH1750 lightMeter2;

PCA9634 testModule3(0x1C);
BH1750 lightMeter3;

PCA9634 testModule4(0x1C);
BH1750 lightMeter4;

/* Адрес зависит от перемычек на модуле(также попробуйте просканировать адрес: https://github.com/MAKblC/Codes/tree/master/I2C%20scanner)
  Данный пример позволяет запустить MGL-RGB3 с тремя надетыми джамперами
*/

void setup() {
  Serial.begin(115200);
Wire.begin();



  setBusChannel(0x06);
  lightMeter.begin();
  testModule.begin();
for (int channel = 0; channel < testModule.channelCount(); channel++)
  {
    testModule.setLedDriverMode(channel, PCA9634_LEDPWM); // установка режима ШИМ (0-255)
  }


  setBusChannel(0x03);
  lightMeter3.begin();
  testModule3.begin();
for (int channel = 0; channel < testModule.channelCount(); channel++)
  {
    testModule3.setLedDriverMode(channel, PCA9634_LEDPWM); // установка режима ШИМ (0-255)
  }


  setBusChannel(0x05);
  lightMeter4.begin();
  testModule4.begin();
for (int channel = 0; channel < testModule4.channelCount(); channel++)
  {
    testModule4.setLedDriverMode(channel, PCA9634_LEDPWM); // установка режима ШИМ (0-255)
  }




  setBusChannel(0x07);
  lightMeter2.begin();
  testModule2.begin();
  for (int channel = 0; channel < testModule2.channelCount(); channel++)
  {
    testModule2.setLedDriverMode(channel, PCA9634_LEDPWM); // установка режима ШИМ (0-255)
  }





    WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  WiFi.softAP("esp", "ew");
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP()); // Обычно 192.168.4.1
  Serial.println("");
  Serial.println("WiFi подключен");
  Serial.println("IP адрес: ");
  Serial.println(WiFi.localIP());

  // Настраиваем обработчики веб-сервера
  server.on("/", handleRoot);      // Главная страница
  server.on("/data", handleData);  // API для приема данных
  
  server.begin();
  Serial.println("HTTP сервер запущен");
}

void loop() {

  server.handleClient();  // Обслуживаем клиентов
  setBusChannel(0x06);
  float lux = lightMeter.readLightLevel(); // считывание освещенности
  Serial.println(lux);
    Serial.println("first");

  // с помощью функции
  
  if (lux < 1000){

  receivedData = "First sensor: Night, lights on";
  testModule.write1(3, 0x90);
  testModule.write1(2, 0x90);
  testModule.write1(5, 0x90);
  }
  else{
    receivedData = "First sensor: Day, lights off";

  testModule.write1(3, 0x00);
  testModule.write1(2, 0x00);
  testModule.write1(5, 0x00);

  }
setBusChannel(0x07);
  float lux2 = lightMeter2.readLightLevel(); // считывание освещенности
  Serial.println(lux2);
    Serial.println("second");


  // с помощью функции
  
  if (lux2 < 100){
    
  receivedData2 = "Second sensor: Night, lights on";
  testModule2.write1(3, 0x90);
  testModule2.write1(2, 0x90);
  testModule2.write1(5, 0x90);
  }
  else{
    receivedData2 = "Second sensor: Day, lights off";
 setBusChannel(0x07);
  testModule2.write1(3, 0x00);
  testModule2.write1(2, 0x00);
  testModule2.write1(5, 0x00);}



setBusChannel(0x05);
  float lux4 = lightMeter4.readLightLevel(); // считывание освещенности
  Serial.println(lux4);
    Serial.println("third");


  // с помощью функции
  
  if (lux4 < 100){
    
  receivedData4 = "Fourth sensor: Night, lights on";
  testModule4.write1(3, 0x90);
  testModule4.write1(2, 0x90);
  testModule4.write1(5, 0x90);
  }
  else{
    receivedData4 = "Fourth sensor: Day, lights off";
 setBusChannel(0x05);
  testModule4.write1(3, 0x00);
  testModule4.write1(2, 0x00);
  testModule4.write1(5, 0x00);}




setBusChannel(0x03);
  float lux3 = lightMeter3.readLightLevel(); // считывание освещенности
  Serial.println(lux3);
    Serial.println("fourth");


  // с помощью функции
  
  if (lux3 < 100){
    
  receivedData3 = "Third sensor: Night, lights on";
  testModule3.write1(3, 0x90);
  testModule3.write1(2, 0x90);
  testModule3.write1(5, 0x90);
  }
  else{
    receivedData3 = "Third sensor: Day, lights off";
 setBusChannel(0x03);
  testModule3.write1(3, 0x00);
  testModule3.write1(2, 0x00);
  testModule3.write1(5, 0x00);

  }
}

bool setBusChannel(uint8_t i2c_channel)
{
 if (i2c_channel >= MAX_CHANNEL)
 {
 return false;
 }
 else
 {
 Wire.beginTransmission(I2C_HUB_ADDR);
 Wire.write(i2c_channel | EN_MASK);
 Wire.endTransmission();
 return true;
 }
}


void handleRoot() {
  String html = "<html><head><meta http-equiv='refresh' content='2'></head>";
  html += "<body><h1>data from esp:</h1>";
  html += "<h2>" + receivedData + "</h2>";
  html += "<h2>" + receivedData2 + "</h2>";
  html += "<h2>" + receivedData3 + "</h2>";
  html += "<h2>" + receivedData4 + "</h2>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

// API для приема данных
void handleData() {
  if (server.hasArg("value")) {
    receivedData = server.arg("value");
    receivedData2 = server.arg("value");
    receivedData3 = server.arg("value");
    receivedData4 = server.arg("value");

    Serial.println("Получены данные: " + receivedData);
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Bad Request");
  }
}