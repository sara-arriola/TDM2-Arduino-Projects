document.addEventListener('DOMContentLoaded', () => {

  const contenedor = document.querySelector('.contenedor-3d');
  const ventanas = Array.from(document.querySelectorAll('.calendario-ventana'));
  let indice = 0;

  // Estado del modo (Controla si movemos el calendario o no)
  // Lo hacemos global (window.modoCalendario) para que el HTML sepa en qué modo estamos
  window.modoCalendario = 'normal'; // 'normal' = calendario, 'right-bar' = barra lateral

  // Separaciones y cosas visuales entre las ventanas
  const ESPACIADO_X = 140;
  const ESPACIADO_Z = 180;
  const ROT_Y = 0;
  const FRONT_Z = 160;
  const FRONT_ESCALA = 1.06;

  function actualizarVista() {
    ventanas.forEach((v, i) => {
      const offset = i - indice;
      const absOff = Math.abs(offset);

      if (offset === 0) {
        v.classList.add('front');
        v.style.transform = `translateX(0px) translateZ(${FRONT_Z}px) rotateY(0deg) scale(${FRONT_ESCALA})`;
        v.style.opacity = '1';
        v.style.zIndex = String(1000);
      } else {
        v.classList.remove('front');
        const dir = offset < 0 ? 1 : -1;
        const x = offset * ESPACIADO_X;
        const z = -absOff * ESPACIADO_Z;
        const rotY = dir * ROT_Y;
        v.style.transform = `translateX(${x}px) translateZ(${z}px) rotateY(${rotY}deg) scale(1)`;
        v.style.opacity = String(Math.max(0.20, 1 - absOff * 0.25));
        v.style.zIndex = String(900 - absOff);
      }
    });
  }

  actualizarVista();

  let lastRueda = 0;
  const RUEDA_DELAY = 110;

  // -------- SCROLL MOUSE --------
  window.addEventListener('wheel', (e) => {
    if (window.modoCalendario !== 'normal') return; 

    const now = performance.now();
    if (now - lastRueda < RUEDA_DELAY) {
      e.preventDefault();
      return;
    }
    lastRueda = now;
    e.preventDefault();

    if (e.deltaY > 0) indice++;
    else if (e.deltaY < 0) indice--;

    // Límites
    if (indice < 0) indice = 0;
    if (indice > ventanas.length - 1) indice = ventanas.length - 1;

    actualizarVista();
  }, { passive: false });


  // -------- 🔥 NUEVO: INTEGRACIÓN CON ENCODER ARDUINO --------
  window.addEventListener('message', (evento) => {
    if (evento.data && evento.data.tipo === 'ENCODER_BITIA') {
      
      // Solo nos movemos si estamos en modo calendario
      if (window.modoCalendario === 'normal') {
        const direccion = evento.data.dir; // "CW" o "CCW"
        
        if (direccion === "CW") {
          indice++; // Avanzar
        } else {
          indice--; // Retroceder
        }

        // Límites
        if (indice < 0) indice = 0;
        if (indice > ventanas.length - 1) indice = ventanas.length - 1;

        actualizarVista();
      }
    }
  });


  // -------- TECLADO CONTROL (y/k) --------
  window.addEventListener('keydown', (e) => {
    if (window.modoCalendario !== 'normal') return;

    if (e.key === 'y' || e.key === 'Y') {
      indice = Math.min(ventanas.length - 1, indice + 1);
      actualizarVista();
    } else if (e.key === 'k' || e.key === 'K') {
      indice = Math.max(0, indice - 1);
      actualizarVista();
    }
  });

  // -------- DETECTAR MANTENER Z (CAMBIO DE MODO) --------
  let zPresionada = false;
  let zTimer = null;

  window.addEventListener('keydown', (e) => {
    if (e.key === 'z' || e.key === 'Z') {
      if (!zPresionada) {
        zPresionada = true;
        zTimer = setTimeout(() => {
          // Toggle de modo global
          window.modoCalendario = (window.modoCalendario === 'normal') ? 'right-bar' : 'normal';
          console.log(`Modo cambiado a: ${window.modoCalendario}`);
          
          // Disparamos un evento para que el HTML se entere y actualice la UI de la barra
          window.dispatchEvent(new CustomEvent('cambioModo', { detail: window.modoCalendario }));
          
        }, 2000); 
      }
    }
  });

  window.addEventListener('keyup', (e) => {
    if (e.key === 'z' || e.key === 'Z') {
      zPresionada = false;
      clearTimeout(zTimer); 
    }
  });

});