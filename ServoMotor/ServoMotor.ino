#include <WiFi.h>
#include <ESPAsyncWebServer.h>
//#include <ESP32Servo.h>
#include "Page.h"  // HTML file included

// Define button pin
const int buttonPin = 23;

// Variables for managing the button state and menu selection
int currentOption = 0;  // Start with the first option
const char* options[] = {"Opción 1", "Opción 2", "Opción 3", "Opción 4"};  // List of options
const int totalOptions = 4;  // Total number of options

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

// Function to send selected option updates to all connected WebSocket clients
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
      // Extract the option (remove "SET" and any extra spaces)
      String selectedOption = message.substring(4);
      notifyClients("Selection:" + selectedOption);  // Notify all clients about the new selected option
    }
  }
}

void setup() {
  // Setup button pin
  pinMode(buttonPin, INPUT_PULLUP);  // Using INPUT_PULLUP for the button

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

  // Handle WebSocket communication
  ws.onEvent(onEvent);
  server.addHandler(&ws);

  // Start the server
  server.begin();
}

void loop() {
  // Handle WebSocket clients
  ws.cleanupClients();

  // Variable for detecting button state
  static bool lastButtonState = HIGH;
  
  // Read the current button state
  bool currentButtonState = digitalRead(buttonPin);
  
  // Detect falling edge (when the button is pressed)
  if (currentButtonState == LOW && lastButtonState == HIGH) {
    // Small debounce delay
    delay(50);
    
    // Move the selector (up or down)
    if (currentOption < totalOptions - 1) {
      currentOption++;  // Move down in the options list
    } else {
      currentOption = 0;  // Loop back to the first option
    }
    
    // Send the updated option to WebSocket clients
    String selectedOption = options[currentOption];
    notifyClients("Selection:" + selectedOption);  // Notify all clients about the new selection

    // Wait until the button is released to avoid multiple increments
    while (digitalRead(buttonPin) == LOW);
  }

  // Save the current button state for next comparison
  lastButtonState = currentButtonState;
}
