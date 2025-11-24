  function widgetClickeable() {

  let ventanaActiva = null;
  const aplicacion = document.querySelectorAll('.widget');

  aplicacion.forEach(aplicaciones => {
    aplicaciones.addEventListener('click', (e) => {
      e.stopPropagation();
      const target = aplicaciones.dataset.ventana;
      ventanaActiva = document.querySelector(`.ventana[data-ventana="${target}"]`);
      ventanaActiva.style.display = 'flex';
    });
  });

  document.addEventListener('click', (e) => {
    if (ventanaActiva && !ventanaActiva.contains(e.target)) {
      ventanaActiva.style.display = 'none';
      ventanaActiva = null;
    }  
  });
}
