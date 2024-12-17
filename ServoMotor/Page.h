const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Control de Servo Motor ESP32</title>
  <style>
    body { font-family: Arial; text-align: center; margin: 20px; }
    .slider { width: 300px; }
    .button { padding: 15px 30px; font-size: 20px; margin: 10px; cursor: pointer; }
  </style>
</head>
<body>
  <h1>Control de Servo Motor con ESP32</h1>

  <!-- Servo control using slider -->
  <h2>Control del Servo</h2>
  <p>Posicion del Servo: <span id="servoPos">0</span> grados</p>
  <input type="range" min="0" max="180" class="slider" id="servoSlider" value="0" onchange="updateServoPosition(this.value)">
  
  <script>
    // WebSocket connection
    const ws = new WebSocket('ws://' + window.location.hostname + '/ws');

    // When receiving a message from WebSocket
    ws.onmessage = (event) => {
      const message = event.data;
      if (message.startsWith("Servo:")) {
        const newPosition = message.replace("Servo:", "");
        document.getElementById('servoPos').innerText = newPosition;
        document.getElementById('servoSlider').value = newPosition;
      }
    };

    // Send the updated servo position via WebSocket
    function updateServoPosition(pos) {
      ws.send("SET " + pos);  // Send the new position to the server
    }
  </script>
</body>
</html>
)rawliteral";
