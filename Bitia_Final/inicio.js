document.addEventListener("DOMContentLoaded", function() {

const startButton = document.querySelector(".btn-start img");
const startContainer = document.querySelector(".start");
const loaderContainer = document.querySelector(".loader");

    // Cambiamos 'keydown' por 'keypress'
    window.addEventListener("keypress", (event) => {
        
        // Ahora usamos event.key === "+" para detectar el símbolo '+' 
        // (ya sea del teclado principal con Shift o del teclado numérico).
        if (event.key === "+") {
            
            console.log("Start button pressed (+)");
            
            // Opcional: Detener la acción predeterminada, aunque el '+' no tiene una.
            event.preventDefault();  
            
            startButton.src = "btn-pressed.jpg";//cambia la imagen al estado presionado
            
            setTimeout(() => {
                startContainer.style.display = "none";
                loaderContainer.style.display = "block";
                
                setTimeout(() => {
                    //ACÁ TENES QUE CAMBIAR LA RUTA AL NUCLEO1
                    window.location.href = "instrucciones.html";
                }, 1000);//supuestamente es el tiempo que tarda en cargar el nucleo1
                
            }, 1000);//tiempo que el boton se ve presionado
        }
    });
    
    // NOTA: Para prevenir que otras teclas, como la 'z' (si estaba configurada en keydown),
    // puedan interferir, hemos eliminado el listener de keydown.
});