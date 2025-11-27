import { BitiaConnector } from './bitia_connector.js';

// --- CLASE MOCK PARA SIMULACIÓN ---
// Esta clase simula el comportamiento de BitiaConnector cuando el Arduino no está conectado.
class MockConnector {
    constructor() {
        console.log("⚠️ Ejecutando en Modo Simulación (Sin Arduino)");
        // onScroll debe ser inicializado para que main.js lo defina después.
        this.onScroll = (direccion) => { 
            console.log(`[MOCK] Scroll simulado: ${direccion}`); 
        };
    }
    async conectar() {
        // Simula la conexión instantánea y exitosa
        return true; 
    }
    setNucleo(nucleo) {
        console.log(`[MOCK] setNucleo(${nucleo}) llamado.`);
    }
    // Propiedad para que el main sepa que estamos en modo simulación
    isMock = true;
}


// --- 1. INSTANCIACIÓN ---
// Comprueba si la API Serial está disponible (window.Serial).
// Si no lo está o no es compatible, usa el MockConnector.
const arduino = window.Serial ? new BitiaConnector() : new MockConnector(); 


const statusDisplay = document.getElementById('status');
const contentFrame = document.getElementById('contentFrame'); 
const btnConnect = document.getElementById('btnConnect');

// EXPOSICIÓN GLOBAL (Útil para debug)
window.app = arduino;
window.contentFrame = contentFrame;


// --- 2. CONEXIÓN Y ACTIVACIÓN ---
if (btnConnect) {
    btnConnect.addEventListener('click', async () => {
        await arduino.conectar();
        
        // Feedback visual en el botón
        btnConnect.style.background = "#0f0";
        btnConnect.innerText = "✅ ACTIVO";
        
        if (statusDisplay) {
            statusDisplay.innerText = arduino.isMock ? "Sistema en MODO SIMULACIÓN. Usa 8/0 o Flechas." : "Sistema Bitia Activo";
            statusDisplay.style.color = "#0f0";
        }

        // Configura el Nucleo inicial (simulado o real)
        arduino.setNucleo(0); 
        
        // --- Activa la Simulación por Teclado si estamos en modo Mock ---
        if (arduino.isMock) {
            addKeyboardSimulators();
        }
    });
}


// --- 3. FEEDBACK DE ERRORES (Solo se define si no estamos en Mock) ---
if (!arduino.isMock) {
    arduino.onError = (err) => {
        if (statusDisplay) {
            statusDisplay.innerText = "Error: " + err;
            statusDisplay.style.color = "red";
        }
        console.error("Error de conexión serial:", err);
    };
}


// --- 4. SCROLL / COMANDO AL IFRAME ---
// Esta función es llamada tanto por el BitiaConnector real como por los simuladores de teclado.
arduino.onScroll = (direccion) => {
    
    // Verificamos si el iframe tiene una ventana cargada
    if (contentFrame.contentWindow) {
        // console.log("Enviando comando al iframe:", direccion);
        
        // ENVIAMOS EL MENSAJE POSTAL SEGURO
        contentFrame.contentWindow.postMessage({ 
            tipo: 'ENCODER_BITIA', 
            dir: direccion 
        }, '*');
    }
};


// --- 5. FUNCIÓN DE SIMULACIÓN POR TECLADO ---
function addKeyboardSimulators() {
    console.log("Simulación activa. Usa Flecha Arriba/Abajo o teclas 8/0 para desplazar.");
    
    document.addEventListener('keydown', (event) => {
        let direccion = null;
        
        // 8 y Flecha Arriba simulan 'UP'
        if (event.key === 'ArrowUp' || event.key === '8') {
            direccion = 'UP';
            
        // 0 y Flecha Abajo simulan 'DOWN'
        } else if (event.key === 'ArrowDown' || event.key === '0') {
            direccion = 'DOWN';
            
        // Puedes añadir 'Enter' para simular un CLICK (si lo usas en tu hardware)
        } else if (event.key === 'Enter') {
             direccion = 'CLICK'; 
        }

        if (direccion) {
            // Llama a la función principal onScroll definida arriba
            arduino.onScroll(direccion); 
            event.preventDefault(); // Evita el scroll nativo de la página maestra
        }
    });
}


// --- 6. DETECCIÓN DE CAMBIO DE PÁGINA (OPCIONAL PERO RECOMENDADO) ---
// Note: Esto sólo funcionará si el iframe carga contenido en el mismo dominio.
/*
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
*/

// --- 7. FUNCIÓN AUXILIAR GLOBAL ---
window.changePage = (url) => {
    if (contentFrame.src.endsWith(url)) return;
    contentFrame.src = url;
};