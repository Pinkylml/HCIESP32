const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">  <!-- Declarar UTF-8 para manejo correcto de caracteres -->
  <meta name="viewport" content="width=device-width, initial-scale=1.0"> <!-- Ajuste para dispositivos móviles -->
  <title>Selector de Opciones ESP32</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      text-align: center;
      margin: 20px;
      background-color: #f4f4f9;
    }
    h1 {
      color: #333;
    }
    .menu {
      font-size: 20px;
      padding: 10px;
      cursor: pointer;
      background-color: #e0e0e0;
      margin: 5px 0;
      border-radius: 5px;
      transition: background-color 0.3s;
    }
    .menu:hover {
      background-color: #ccc;
    }
    .selected {
      color: white;
      background-color: #007BFF;
    }
    #selectedOption {
      font-weight: bold;
      color: #007BFF;
    }
  </style>
</head>
<body>
  <h1>Interacción con el Pulsador</h1>

  <!-- Lista de opciones -->
  <div id="menu">
    <div id="option0" class="menu">Opción 1</div>
    <div id="option1" class="menu">Opción 2</div>
    <div id="option2" class="menu">Opción 3</div>
    <div id="option3" class="menu">Opción 4</div>
  </div>

  <p>Selección actual: <span id="selectedOption">Opción 1</span></p>

  <script>
    // WebSocket connection
    const ws = new WebSocket('ws://' + window.location.hostname + '/ws');

    // When receiving a message from WebSocket
    ws.onmessage = (event) => {
      const message = event.data;
      if (message.startsWith("Selection:")) {
        const selectedOption = message.replace("Selection:", "");
        document.getElementById('selectedOption').innerText = selectedOption;
        
        // Update visual indication of selected option
        for (let i = 0; i < 4; i++) {
          const optionElement = document.getElementById("option" + i);
          if (optionElement) {
            if (optionElement.innerText === selectedOption) {
              optionElement.classList.add("selected");
            } else {
              optionElement.classList.remove("selected");
            }
          }
        }
      }
    };

    // Function to update the selected option in the ESP32
    function updateSelection(option) {
      ws.send("SET " + option);
    }

    // This will listen for key presses to simulate button actions
    document.addEventListener('keydown', function(event) {
      if (event.key === "ArrowUp") {
        // Move up
        updateSelection("Opción 1");
      } else if (event.key === "ArrowDown") {
        // Move down
        updateSelection("Opción 2");
      }
    });
  </script>
</body>
</html>
)rawliteral";
