#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASSWORD";
const char* apSSID = "ESP_SSID";
const char* apPassword = "ESP_PASSWORD";

// Структуры данных
struct SystemState {
    // Traffic Lights
    String trafficLight1 = "red";
    String trafficLight2 = "red";
    
    // Parking
    bool parkingOrder = false;
    bool parkingState = false;
    bool parkingEsit = false;
    
    // Environment
    float temperature = 0.0;
    float humidity = 0.0;
    float pressure = 0.0;
    
    // Train
    float trainTime = 0.0;
    float trainDelay = 0.0;
} systemState;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Полный HTML-код
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Smart City Control</title>
    <style>
        body {font-family: Arial; margin: 20px; background: #f0f0f0;}
        .card {background: white; padding: 20px; margin: 15px; border-radius: 10px; box-shadow: 0 2px 5px rgba(0,0,0,0.1);}
        .grid {display: grid; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); gap: 20px;}
        .status {font-size: 1.2em; color: #666;}
        .value {font-size: 2em; color: #333; margin: 10px 0;}
        button {background: #4CAF50; color: white; border: none; padding: 10px 20px; border-radius: 5px; cursor: pointer;}
        button:hover {background: #45a049;}
    </style>
</head>
<body>
    <h1>Smart City Monitoring System</h1>
    
    <div class="grid">
        <!-- Traffic Lights -->
        <div class="card">
            <h2>Traffic Lights</h2>
            <div class="status">Intersection 1: <span id="tl1">-</span></div>
            <div class="status">Intersection 2: <span id="tl2">-</span></div>
            <button onclick="sendCommand('traffic', 'toggle')">Toggle Lights</button>
        </div>

        <!-- Parking -->
        <div class="card">
            <h2>Parking System</h2>
            <div class="status">State: <span id="parkState">-</span></div>
            <div class="status">Emergency: <span id="parkEsit">-</span></div>
            <button onclick="sendCommand('parking', 'toggle')">Toggle Gate</button>
        </div>

        <!-- Environment -->
        <div class="card">
            <h2>Environment</h2>
            <div class="status">Temperature: <span id="temp">-</span> °C</div>
            <div class="status">Humidity: <span id="hum">-</span> %</div>
            <div class="status">Pressure: <span id="press">-</span> hPa</div>
        </div>

        <!-- Train -->
        <div class="card">
            <h2>Train Info</h2>
            <div class="status">Next Train: <span id="trainTime">-</span> min</div>
            <div class="status">Delay: <span id="trainDelay">-</span> min</div>
        </div>
    </div>

    <script>
        const ws = new WebSocket(`ws://${window.location.hostname}/ws`);
        
        ws.onmessage = function(event) {
            const data = JSON.parse(event.data);
            updateUI(data);
        };

        function updateUI(data) {
            // Traffic Lights
            if(data.traffic) {
                document.getElementById('tl1').textContent = data.traffic[0];
                document.getElementById('tl2').textContent = data.traffic[1];
            }
            
            // Parking
            if(data.parking) {
                document.getElementById('parkState').textContent = data.parking.state ? "Open" : "Closed";
                document.getElementById('parkEsit').textContent = data.parking.esit ? "Active" : "Inactive";
            }
            
            // Environment
            if(data.environment) {
                document.getElementById('temp').textContent = data.environment.temp;
                document.getElementById('hum').textContent = data.environment.hum;
                document.getElementById('press').textContent = data.environment.press;
            }
            
            // Train
            if(data.train) {
                document.getElementById('trainTime').textContent = data.train.time;
                document.getElementById('trainDelay').textContent = data.train.delay;
            }
        }

        function sendCommand(type, action) {
            ws.send(JSON.stringify({
                type: type,
                action: action
            }));
        }
    </script>
</body>
</html>
)rawliteral";

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if(info->final && info->index == 0 && info->len == len){
        DynamicJsonDocument doc(256);
        DeserializationError error = deserializeJson(doc, data, len);
        
        if(!error) {
            String type = doc["type"];
            String action = doc["action"];
            
            if(type == "traffic" && action == "toggle") {
                systemState.trafficLight1 = (systemState.trafficLight1 == "red") ? "green" : "red";
                systemState.trafficLight2 = (systemState.trafficLight2 == "red") ? "green" : "red";
            }
            else if(type == "parking" && action == "toggle") {
                systemState.parkingState = !systemState.parkingState;
            }
        }
    }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, 
             AwsEventType type, void *arg, uint8_t *data, size_t len) {
    switch(type) {
        case WS_EVT_CONNECT:
            Serial.printf("Client #%u connected\n", client->id());
            break;
        case WS_EVT_DISCONNECT:
            Serial.printf("Client #%u disconnected\n", client->id());
            break;
        case WS_EVT_DATA:
            handleWebSocketMessage(arg, data, len);
            break;
        default:
            break;
    }
}

String getSystemData() {
    DynamicJsonDocument doc(512);
    
    // Traffic Lights
    JsonArray traffic = doc.createNestedArray("traffic");
    traffic.add(systemState.trafficLight1);
    traffic.add(systemState.trafficLight2);
    
    // Parking
    JsonObject parking = doc.createNestedObject("parking");
    parking["state"] = systemState.parkingState;
    parking["esit"] = systemState.parkingEsit;
    
    // Environment
    JsonObject env = doc.createNestedObject("environment");
    env["temp"] = systemState.temperature;
    env["hum"] = systemState.humidity;
    env["press"] = systemState.pressure;
    
    // Train
    JsonObject train = doc.createNestedObject("train");
    train["time"] = systemState.trainTime;
    train["delay"] = systemState.trainDelay;

    String output;
    serializeJson(doc, output);
    return output;
}

void setup() {
    Serial.begin(115200);
    
    // WiFi Setup
    WiFi.softAP(apSSID, apPassword);
    WiFi.begin(ssid, password);
    
    while(WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("\nWiFi Connected!");
    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());
    Serial.print("Local IP: ");
    Serial.println(WiFi.localIP());

    // Web Server Routes
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", index_html);
    });

    // WebSocket
    ws.onEvent(onEvent);
    server.addHandler(&ws);

    // Start Server
    server.begin();
}

void loop() {
    static unsigned long lastUpdate = 0;
    if(millis() - lastUpdate > 1000) {
        // Обновление данных
        systemState.temperature = random(200, 300)/10.0;
        systemState.humidity = random(300, 700)/10.0;
        systemState.pressure = random(900, 1100);
        systemState.trainTime = random(1, 15);
        
        // Рассылка данных
        ws.textAll(getSystemData());
        lastUpdate = millis();
    }
    
    ws.cleanupClients();
    delay(10);
}
