import { BitiaConnector } from './bitia_connector.js';

// Inicializa el conector de Arduino
const arduino = new BitiaConnector();

// ----------------------------------------
// Funciones Globales para el HTML
// ----------------------------------------

// Función para conectar el Arduino (disponible globalmente)
function conectarArduino() {
    // Busca dónde mostrar el estado. Si existe 'status' úsalo, si no, usa un console.log.
    const statusDisplay = document.getElementById('status');
    
    // 1. Manejar el estado de la conexión
    arduino.onStatus = (msg) => {
        if (statusDisplay) {
            statusDisplay.innerText = msg;
            statusDisplay.style.color = "#0f0";
        } else {
            console.log("STATUS:", msg);
        }
    };
    
    // 2. Manejar errores
    arduino.onError = (err) => {
        if (statusDisplay) {
            statusDisplay.innerText = "Error";
            statusDisplay.style.color = "red";
            alert(err);
        } else {
            console.error("ERROR:", err);
            alert("Error de conexión serial. Mira la consola.");
        }
    };

    // 3. La MAGIA DEL SCROLL (Se aplica a toda la ventana)
    arduino.onScroll = (direccion) => {
        // Cantidad de píxeles a mover por cada "tic" del encoder
        const velocidad = 150; 

        if (direccion === "CW") {
            // Clockwise -> Bajar (Scroll positivo)
            window.scrollBy({
                top: velocidad,
                behavior: 'smooth' // Movimiento suave
            });
        } else {
            // Counter-Clockwise -> Subir (Scroll negativo)
            window.scrollBy({
                top: -velocidad,
                behavior: 'smooth'
            });
        }
    };

    // 4. Iniciar la conexión
    arduino.conectar();
}


// ----------------------------------------
// Exponer variables/funciones globales
// ----------------------------------------

// Asegúrate de que las funciones y el objeto Arduino sean accesibles globalmente
// para los botones 'onclick' y la conexión.
window.app = arduino;
window.conectarArduino = conectarArduino;


// ----------------------------------------
// Inicialización Automática (si el elemento existe)
// ----------------------------------------
// Busca el botón de conexión en la página y le asigna el evento.
document.addEventListener('DOMContentLoaded', () => {
    const btnConnect = document.getElementById('btnConnect');
    if (btnConnect) {
        btnConnect.addEventListener('click', conectarArduino);
    }
});