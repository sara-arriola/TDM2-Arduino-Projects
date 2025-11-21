/**
 * 🧠 BITIA SERIAL CONTROLLER
 * Este módulo maneja toda la comunicación técnica con el Arduino.
 * Separa la lógica de conexión de la interfaz visual.
 */

export class BitiaConnector {
    constructor() {
        this.port = null;
        this.writer = null;
        this.reader = null;
        this.keepReading = false;

        // Eventos vacíos (hooks) para que la UI se "enganche" a ellos
        this.onScroll = (direccion) => {};  // Recibe "CW" o "CCW"
        this.onStatus = (mensaje) => {};    // Para mensajes de debug o conexión
        this.onError = (error) => {};       // Para manejar errores
    }

    /**
     * Inicia la conexión con el Arduino
     */
    async conectar() {
        if (!navigator.serial) {
            this.onError("Tu navegador no soporta Web Serial API.");
            return;
        }

        try {
            // 1. Pedir permiso al usuario
            this.port = await navigator.serial.requestPort();
            
            // 2. Abrir puerto a 9600 baudios (Igual que el .ino)
            await this.port.open({ baudRate: 9600 });

            // 3. Configurar el escritor (PC -> Arduino)
            const textEncoder = new TextEncoderStream();
            textEncoder.readable.pipeTo(this.port.writable);
            this.writer = textEncoder.writable.getWriter();

            // 4. Configurar el lector (Arduino -> PC)
            const textDecoder = new TextDecoderStream();
            this.port.readable.pipeTo(textDecoder.writable);
            this.reader = textDecoder.readable.getReader();

            this.keepReading = true;
            this.onStatus("✅ Conexión establecida con Arduino Bitia");
            
            // 5. Iniciar el bucle de escucha en segundo plano
            this.leerLoop();

        } catch (err) {
            this.onError("Error conectando: " + err);
        }
    }

    /**
     * Envía el comando de cambio de estado formateado
     * @param {number} id - El número del núcleo (0, 1, 2, 3)
     */
    async setNucleo(id) {
        const comando = `ESTADO:${id}`;
        await this.enviarRaw(comando);
        this.onStatus(`📤 Comando enviado: ${comando}`);
    }

    /**
     * Envía texto crudo al Arduino
     */
    async enviarRaw(texto) {
        if (this.writer) {
            await this.writer.write(texto + "\n");
        } else {
            this.onError("⚠️ No estás conectado al Arduino.");
        }
    }

    /**
     * Desconecta el puerto limpiamente
     */
    async desconectar() {
        this.keepReading = false;
        if (this.reader) await this.reader.cancel();
        if (this.writer) await this.writer.close();
        if (this.port) await this.port.close();
        this.onStatus("🔌 Desconectado.");
    }

    /**
     * Bucle infinito que escucha lo que dice el Arduino
     */
    async leerLoop() {
        let buffer = "";

        while (this.port.readable && this.keepReading) {
            try {
                const { value, done } = await this.reader.read();
                if (done) break;

                if (value) {
                    buffer += value;
                    // Separar por saltos de línea para procesar mensajes completos
                    let lines = buffer.split('\n');
                    buffer = lines.pop(); // Guardar el resto incompleto

                    for (let line of lines) {
                        this.procesarMensaje(line.trim());
                    }
                }
            } catch (err) {
                this.onError("Error de lectura: " + err);
                break;
            }
        }
    }

    /**
     * Interpreta qué dijo el Arduino
     */
    procesarMensaje(msg) {
        if (msg === "CW" || msg === "CCW") {
            // Disparar el evento de scroll para que la UI lo use
            this.onScroll(msg);
        } else {
            // Cualquier otro mensaje (como "BTN" o debug)
            console.log("Arduino dice:", msg);
        }
    }
}
