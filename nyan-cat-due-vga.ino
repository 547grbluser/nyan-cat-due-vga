#include <SD.h>
#include <SPI.h>
#include <VGA.h>

//Pin usado para el Chip Select de la memoria SD
const int chipSelect = 10;

//Arreglo con las imagenes de todos los cuadros de animacion
//(12 cuadros de animacion en total). Notese que al ser tipo
//const, el arduino Due automaticamente lo alberga en flash
const byte datosVideo[12][80*60] = {
  #include "Cuadros.h"
  //#include "Cuadros_San_V.h"
};
//NOTA: La resolucion de la pantalla a usar es de 320x240 con la
//libreria VGA, sin embargo la animacion de Nyan Cat es de una
//resolucion mucho inferior a eso: 80x60, por lo que en la rutina de
//dibujado de pantalla se cuadruplicaran los pixeles (zoom de 4:1)

void setup() {
  //Inicializa el audio y la libreria VGA
  inicializarAudio();
  VGA.begin(320,240,VGA_COLOUR);
}

void loop() {
  static int nCuadro = 0;
  static bool mostrarVideo = false;
  const byte * const pCuadro = datosVideo[nCuadro];
  int pOrigen, pDestino;
  int x, y;
  byte pixel;

  //Se procura esperar exactamente 4 segundos, para dar espacio a
  //que la cancion comience y la imagen de Nyan Cat ocurra justo
  //cuando termina la parte introductoria de la tonada
  if (!mostrarVideo) {
    //Verifica si ya pasaron 4 segundos. De ser asi marca la bandera
    //como verdadero.
    if (millis() >= 4000) mostrarVideo = true;
    //Mientras se espera, se acualiza el buffer de audio
    actualizarBufferAudio();
    //Retorna mientras no pasen los 4 segundos y repite el proceso
    return;
  }

  //Se inicia el proceso de copia de la imagen con la animacion al
  //buffer VGA
  pOrigen=0;     //Inicialliza los punteros
  pDestino = 0;

  //Copia para 60 filas y 80 columnas, con Zoom de 4 a 1
  for(y=0; y<60; y++) {
    for (x=0; x<80; x++) {
      //Lee el pixel del arreglo de datos de video
      pixel = pCuadro[pOrigen++];

      //Escribe el dato al frame buffer del VGA en 16 ocasiones
      //(Un incremento de 4:1 implica que cada 4^2=16 pixeles del
      //monitor conforman un pixel de la imagen)
      //Notese que las posiciones de pixel se suman como offsets
      //constantes, y que el lazo de zooming esta completamente
      //desenrollado, produciendo una transferencia optimizada
      VGA.cb[pDestino + 320*0 + 0] = pixel;
      VGA.cb[pDestino + 320*0 + 1] = pixel;
      VGA.cb[pDestino + 320*0 + 2] = pixel;
      VGA.cb[pDestino + 320*0 + 3] = pixel;
      VGA.cb[pDestino + 320*1 + 0] = pixel;
      VGA.cb[pDestino + 320*1 + 1] = pixel;
      VGA.cb[pDestino + 320*1 + 2] = pixel;
      VGA.cb[pDestino + 320*1 + 3] = pixel;
      VGA.cb[pDestino + 320*2 + 0] = pixel;
      VGA.cb[pDestino + 320*2 + 1] = pixel;
      VGA.cb[pDestino + 320*2 + 2] = pixel;
      VGA.cb[pDestino + 320*2 + 3] = pixel;
      VGA.cb[pDestino + 320*3 + 0] = pixel;
      VGA.cb[pDestino + 320*3 + 1] = pixel;
      VGA.cb[pDestino + 320*3 + 2] = pixel;
      VGA.cb[pDestino + 320*3 + 3] = pixel;

      //Se apunta al siguiente pixel de la imagen, 4 pixeles a la
      //derecha en el monitor
      pDestino += 4;
    }

    //Cuando se termina de copiar la fila, se avanzan 3 filas
    //adicionales (para completar 4 filas), para asi comenzar con la
    //siguiente
    pDestino += VGA.cw*3;
  }

  //Se esperan 3 cuadros adicionales para temporizar la animacion.
  //Antes de cada espera se actualiza el buffer de audio para evitar
  //que se agote (lo cual provocaria cortes)
  actualizarBufferAudio();
  VGA.waitSync();
  actualizarBufferAudio();
  VGA.waitSync();
  actualizarBufferAudio();
  VGA.waitSync();

  //Se avanza al siguiente cuadro (se repite la animacion cada 12)
  nCuadro = (nCuadro + 1) % 12;
}
