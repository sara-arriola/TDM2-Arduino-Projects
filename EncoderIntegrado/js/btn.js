/* btn.js - VERSIÓN "HIJO" (Recibe órdenes del index.html) */

let enCancionApp = false;
let enDemandasApp = false;

document.addEventListener("DOMContentLoaded", () => {
  let selectedButton = null; 
  let index = 0;

  const buttons = document.querySelectorAll(".btn");
  const categories = document.querySelectorAll(".categoria");

  // --- FUNCIÓN: Actualizar selección visual ---
  function actualizarCategoria() {
    if (selectedButton) {
      selectedButton.classList.remove("selected");
    }
    selectedButton = buttons[index];
    selectedButton.classList.add("selected");

    // console.log("Sección activa:", selectedButton.id); // Debug

    const id = selectedButton.id;
    categories.forEach(cat => {
      cat.style.display = cat.classList.contains(id) ? "block" : "none";
    });
  }

  // --- FUNCIÓN LÓGICA CENTRAL (Mouse y Encoder la usan) ---
  function cambiarIndice(direccion) {
    // Si loopIsVisible es true, bloqueamos el movimiento
    if (typeof loopIsVisible !== 'undefined' && loopIsVisible) return;

    if (direccion > 0) {
       // AVANZAR (Rueda abajo / Encoder CW)
       index = (index + 1) % buttons.length;
    } else if (direccion < 0) {
       // RETROCEDER (Rueda arriba / Encoder CCW)
       index = (index - 1 + buttons.length) % buttons.length;
    }

    // --- Lógica de estados ---
    if (index === 1) {
      enCancionApp = true;
      if (window.resetSongSelection) window.resetSongSelection = true;
    } else {
      enCancionApp = false;
      if(typeof closeActiveSong === 'function') closeActiveSong();
    }

    if (index === 2) {
      enDemandasApp = true;
    } else {   
      enDemandasApp = false;
      if(typeof closeActiveDemanda === 'function') closeActiveDemanda();
    }

    actualizarCategoria();
  }

  // --- CLICK en botones (Mouse) ---
   buttons.forEach((button, i) => {
    button.addEventListener("click", () => {
      index = i;
      actualizarCategoria();
    });
  });

  // Inicializar selección
  actualizarCategoria();

  
  // --- WHEEL GLOBAL (Mouse) ---
  let lastRueda = 0;
  const RUEDA_DELAY = 40; 
  
  window.addEventListener("wheel", (e) => {
    const now = performance.now();
    if (now - lastRueda < RUEDA_DELAY) {
      e.preventDefault();
      return;
    }
    lastRueda = now;
    e.preventDefault(); 

    // Usamos la función compartida: deltaY > 0 es "bajar/avanzar"
    cambiarIndice(e.deltaY > 0 ? 1 : -1);

  }, { passive: false });


  // --- TECLADO (Lógica original) ---
  window.addEventListener("keydown", (event)=>{
    switch(event.code){
      case "Digit1":
      if(typeof loopIsVisible !== 'undefined' && !loopIsVisible){
        const videoCopyright = document.getElementById("copyright-video");
        const videoDemandas = document.getElementById("demandas-video");

        if(selectedButton && selectedButton.id === "copyright"){
            if(videoCopyright.paused) videoCopyright.play();
            else videoCopyright.pause();
        }
      } else if(selectedButton && selectedButton.id === "demandas"){
          enDemandasApp = true;
          const videoDemandas = document.getElementById("demandas-video");
          if (videoDemandas && !videoDemandas.paused) {
            videoDemandas.pause();
          } else if(selectedButton && selectedButton.id == "canciones"){
            enCancionApp = true;
          }
      } else { 
        enCancionApp = false;
        enDemandasApp = false;
      }
      break;
    }
  });


  // ============================================================
  //   COMUNICACIÓN CON EL INDEX (NO TOCAR SERIAL AQUÍ)
  // ============================================================
  
  // Escuchamos el mensaje que envía main.js desde el index.html
  window.addEventListener('message', (evento) => {
    
    // Verificamos que el mensaje sea del tipo correcto
    if (evento.data && evento.data.tipo === 'ENCODER_BITIA') {
        
        const dirString = evento.data.dir; // Recibimos "CW" o "CCW"
        
        // Convertimos el texto a número para nuestra función
        // CW (Clockwise) = 1 (Avanzar)
        // CCW (Counter-Clockwise) = -1 (Retroceder)
        const delta = (dirString === 'CW') ? 1 : -1;
        
        cambiarIndice(delta);
    }
  });

});