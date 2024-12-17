#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESP32Servo.h>
#include "Page.h"  // HTML file included

// Define servo pin and initialize servo object
const int servoPin = 23;
const int buttonPin = 34;
Servo myservo;

// Variables to control the servo
int servoPos = 0;  // Initial position of the servo (0 degrees)

// Wi-Fi Access Point Credentials
const char* ssid = "ESP32_WS";
const char* password = "helloesp32WS";

// IP Configuration for AP mode
IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

// Web Server and WebSocket Setup
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");  // WebSocket for real-time communication

// Function to send servo position updates to all connected WebSocket clients
void notifyClients(const String &message) {
  ws.textAll(message);  // Send message to all connected WebSocket clients
}

// WebSocket event handler
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.println("Client connected.");
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.println("Client disconnected.");
  } else if (type == WS_EVT_DATA) {
    // Handle incoming WebSocket messages
    String message = "";
    for (size_t i = 0; i < len; i++) {
      message += (char) data[i];
    }

    // Check if the message starts with "SET"
    if (message.startsWith("SET")) {
      // Extract the new position (remove "SET" and any extra spaces)
      int newPos = message.substring(4).toInt();  // "SET <position>"
      if (newPos >= 0 && newPos <= 180) {
        servoPos = newPos;  // Update servo position variable
        myservo.write(servoPos);  // Move the servo to the new position

        // Notify all clients about the new servo position
        notifyClients("Servo:" + String(servoPos));
      }
    }
  }
}

void setup() {
  // Setup servo
  myservo.attach(servoPin);  // Attach the servo to the specified pin
  myservo.write(servoPos); 

  pinMode(buttonPin, INPUT_PULLUP);

  // Setup Wi-Fi in Access Point mode
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  Serial.begin(115200);
  Serial.println("Access point started.");
  Serial.println("IP: " + WiFi.softAPIP().toString());

  // Serve the main page (HTML)
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);  // Send HTML page
  });

  // Handle the servo position via WebSocket
  ws.onEvent(onEvent);
  server.addHandler(&ws);

  // Iniciar el servidor web
  server.begin();


}

void loop() {
  // Manejamos WebSocket clients
  ws.cleanupClients();
  
  // Variable para detectar el estado del botón
  static bool lastButtonState = HIGH;
  
  // Leemos el estado actual del botón
  bool currentButtonState = digitalRead(buttonPin);
  
  // Detectamos el flanco de bajada (cuando se presiona)
  if (currentButtonState == LOW && lastButtonState == HIGH) {
    // Pequeño anti-rebote
    delay(50);
    
    // Incrementamos 5 grados
    if (servoPos <= 175) { // Nos aseguramos de no pasarnos de 180
      servoPos += 5;
      myservo.write(servoPos);
      
      // Notificamos a los clientes WebSocket
      notifyClients("Servo:" + String(servoPos));
    }
    
    // Esperamos a que se suelte el botón para evitar múltiples incrementos
    while(digitalRead(buttonPin) == LOW);
  }
  
  // Guardamos el estado actual para la próxima comparación
  lastButtonState = currentButtonState;
}
