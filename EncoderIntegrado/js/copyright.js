document.addEventListener("DOMContentLoaded", () => {

/*e = the "event" that just happened.
It lets you know what key, what mouse button, or what element was involved in the event. */

video = document.getElementById("copyright-video");

    function restartVideo(){
        video.pause();
        video.currentTime = 0;
    }

    // --- parar video al cambiar de categoría ---
    const demandas = document.getElementById("demandas");
    const canciones = document.getElementById("canciones");
    

    if (demandas){
        demandas.addEventListener("click", restartVideo);
    };
    if (canciones){
        canciones.addEventListener("click", restartVideo);
    };


});
