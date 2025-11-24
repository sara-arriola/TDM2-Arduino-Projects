import { BitiaConnector } from './bitia_connector.js';

// Instancia global del conector serial
const arduino = new BitiaConnector(); 

const statusDisplay = document.getElementById('status');
const contentFrame = document.getElementById('contentFrame'); 
const btnConnect = document.getElementById('btnConnect');

// EXPOSICIÓN GLOBAL (Útil para debug)
window.app = arduino;

// --- 1. CONEXIÓN ---
if (btnConnect) {
    btnConnect.addEventListener('click', async () => {
        await arduino.conectar();
        
        // Feedback visual en el botón
        btnConnect.style.background = "#0f0";
        btnConnect.innerText = "✅ CONECTADO";
        
        if (statusDisplay) {
            statusDisplay.innerText = "Sistema Bitia Activo";
            statusDisplay.style.color = "#0f0";
        }

        // Configura el Nucleo inicial
        arduino.setNucleo(0); 
    });
}

// --- 2. FEEDBACK DE ERRORES ---
arduino.onError = (err) => {
    if (statusDisplay) {
        statusDisplay.innerText = "Error: " + err;
        statusDisplay.style.color = "red";
    }
    console.error("Error de conexión serial:", err);
};

// --- 3. SCROLL APLICADO AL IFRAME (CORREGIDO) ---
// Este es el "Paso 1" integrado: Envía el mensaje exacto que espera el hijo.
arduino.onScroll = (direccion) => {
    
    // Verificamos si el iframe tiene una ventana cargada
    if (contentFrame.contentWindow) {
        // console.log("Enviando comando al iframe:", direccion); // Descomentar para debug
        
        // ENVIAMOS EL MENSAJE POSTAL SEGURO
        // La clave aquí es que 'tipo' coincida con lo que pusimos en nucleo11.html
        contentFrame.contentWindow.postMessage({ 
            tipo: 'ENCODER_BITIA', 
            dir: direccion 
        }, '*');
    }
};

// --- 4. DETECCIÓN DE CAMBIO DE PÁGINA (OPCIONAL PERO RECOMENDADO) ---
// Esto sirve para cambiar las luces (SetNucleo) automáticamente si el iframe cambia de URL
contentFrame.addEventListener('load', () => {
    try {
        const url = contentFrame.contentWindow.location.href;
        if (url.includes("Nucleo1")) arduino.setNucleo(1);
        else if (url.includes("Nucleo2")) arduino.setNucleo(2);
        else if (url.includes("Nucleo3")) arduino.setNucleo(3);
        else arduino.setNucleo(0);
    } catch (e) {
        // Ignorar errores de cross-origin si ocurren
    }
});

// Función global auxiliar (La mantengo porque venía en tu archivo original)
window.changePage = (url) => {
    if (contentFrame.src.endsWith(url)) return;
    contentFrame.src = url;
};