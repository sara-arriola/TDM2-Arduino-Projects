document.addEventListener("DOMContentLoaded", () => {

    // --- DATOS DE LAS DEMANDAS ---
    const demandaInfo = [
        { titulo: "El caso Gilbert O’Sullivan vs. Biz Markie", video: "video/video1.mp4" },
        { titulo: "RIAA vs. Suno y Udio", video: "video/video2.mp4" }
    ];

    const demanda = document.getElementById("cuerpo-demanda");
    const demandaBtn = document.querySelectorAll(".btn-demanda");

    // === NUEVO: referencia al <video> cuando se crea ===
    let demandaVideo = null;

    // --- FUNCIÓN: renderizar demanda seleccionada ---
    function renderDemanda(index) {
        const data = demandaInfo[index];

        demanda.innerHTML = `
            <h1>${data.titulo}</h1>
            <div class="video">
                <video src="${data.video}" id="demandas-video" autoplay></video>
            </div>
        `;

        // guardar referencia al video recién creado
        demandaVideo = document.getElementById("demandas-video");

        // si el navegador bloquea autoplay no se rompe
        demandaVideo?.play().catch(() => {});
    }

    // --- FUNCIÓN: resetear demanda ---
    function resetDemanda() {
        if (demandaVideo) {
            demandaVideo.pause();
            demandaVideo.currentTime = 0;
        }
        demanda.innerHTML = "";
        demandaVideo = null;
        isVisible = false;
        activeIndex = null;
    }

    // --- VARIABLES DE CONTROL (IGUAL QUE EN CANCIONES) ---
    let selectedIndex = 0; // índice que mueve A y S
    let activeIndex = null; // índice actualmente abierto
    let isVisible = false;  // hay demanda visible?

    // --- Actualizar la selección visualmente (solo imagen) ---
    function updateSelection() {
        demandaBtn.forEach((btn, i) => {
            btn.src = (i === selectedIndex)
                ? `img/demanda${i + 1}-selected.png`
                : `img/demanda${i + 1}.png`;
        });
    }

    // --- Confirmar selección (abrir demanda o pausar/play) ---
    function confirmSelection() {
        if (loopIsVisible) return;

        // si vuelve a seleccionar la misma → alterna play/pause
        if (activeIndex === selectedIndex && isVisible && enDemandasApp) {
            if (demandaVideo.paused) {
                demandaVideo.play();
            } else {
                demandaVideo.pause();
            }
            return;
        }

        // cerrar demanda anterior si existe
        closeActiveDemanda();

        // abrir la nueva
        renderDemanda(selectedIndex);
        activeIndex = selectedIndex;
        isVisible = true;
    }

    // --- cerrar demanda actualmente abierta ---
    function closeActiveDemanda() {
        if (activeIndex !== null) {
            demandaBtn[activeIndex].src = `img/demanda${activeIndex + 1}.png`;
            resetDemanda();
        }
    }

    // === EVENTOS CLICK ===
    demandaBtn.forEach((btn, i) => {
        btn.addEventListener("click", () => {
            selectedIndex = i;
            updateSelection();
            confirmSelection();
        });
    });

    // === CERRAR DEMANDA AL CAMBIAR DE CATEGORÍA ===
    const copyright = document.getElementById("copyright");
    const canciones = document.getElementById("canciones");

    if (copyright)
        copyright.addEventListener("click", closeActiveDemanda);
    if (canciones)
        canciones.addEventListener("click", closeActiveDemanda);

    // === CONTROL CON TECLADO (A, S, 1) ===
    window.addEventListener("keydown", (event) => {
        if (event.defaultPrevented) return;

        switch (event.code) {
            case "Digit1":
                if (enDemandasApp) {
                    updateSelection();
                    confirmSelection();
                }
                break;

            case "KeyA":
                if (!loopIsVisible && enDemandasApp) {
                    selectedIndex = (selectedIndex - 1 + demandaBtn.length) % demandaBtn.length;
                    updateSelection();
                    closeActiveDemanda();
                }
                break;

            case "KeyS":
                if (!loopIsVisible && enDemandasApp) {
                    selectedIndex = (selectedIndex + 1) % demandaBtn.length;
                    updateSelection();
                    closeActiveDemanda();
                }
                break;
        }
    });

    window.closeActiveDemanda = closeActiveDemanda; // acceso externo

});
