//Posiciones de reinicio y retorno en el archivo de audio (en muestras)
//usadas para hacer el looping de la musica
const int posReinicio = 88203;
const int posRetorno = 684823;

//Objeto de clase del archivo de audio
File archivoAudio;

//Buffer de audio y variables asociadas
byte audioBuffer[1024];
int pArchivoR = 0;          //Posicion de lectura en el archivo
int pBufferW = 0;           //Posicion de escritura en el buffer
volatile int pBufferR = 0;  //Posicion de lectura del buffer

void inicializarAudio() {
  //Inicializacion de la memoria SD
  //-------------------------------
  //Simplemente se inicializa la memoria y se abre el archivo de audio
  SD.begin(chipSelect);
  archivoAudio = SD.open("audioNy.dat");

  //Configuracion del controlador de administracion de energia (PMC) 
  //----------------------------------------------------------------
  //Desactiva la proteccion de escritura y activa el clock del timer
  //(se usan los mismos ID para la administracion de clock que para
  //las interrupciones).
  pmc_set_writeprotect(false);
  pmc_enable_periph_clk(TC3_IRQn);
  //NOTA: El ATSAM3X8E tiene 3 modulos de timer, nombrados TC0, TC1
  //y TC2, y cada uno posee 3 canales (channel 0-2). El mapeo de
  //interrupciones de timer es tal que las primeras 3 interrupciones
  //son para los tres canales del TC0, luego las siguientes 3 para el
  //TC1 y las ultimas 3 para TC2. En total son 9 canales de
  //interrupcion que se numeran desde TC0_IRQn hasta TC8_IRQn.
  //En este caso, al primer canal del TC1, le toca la interrupcion
  //TC3_IRQn.

  //Configuracion del controlador de timer 1 (TC1)
  //----------------------------------------------
  const int canal = 0;
  const int frecuencia = 22050;
  //Se realiza la configuracion del canal 0
  TC_Configure(TC1, canal, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC |
                           TC_CMR_TCCLKS_TIMER_CLOCK1);
  //Se realiza el calculo del contador de periodo del timer, notese que se
  //divide MCK/2 porque arriba se eligio TIMER_CLOCK1
  uint32_t rc = VARIANT_MCK/2/frecuencia;
  //Se establecen los registros de conteo del timer
  TC_SetRA(TC1, canal, rc/2); //50% en alto, 50% en bajo
  TC_SetRC(TC1, canal, rc);
  //Se arranca el timer
  TC_Start(TC1, canal);
  //Habilita la interruppcion de comparacion solamente,
  //deshabilitando el resto
  TC1->TC_CHANNEL[canal].TC_IER=TC_IER_CPCS;
  TC1->TC_CHANNEL[canal].TC_IDR=~TC_IER_CPCS;

  //Configuracion del controlador de interrupciones (NVIC)
  //------------------------------------------------------
  //Habilita de la interrupcion de timer
  NVIC_EnableIRQ(TC3_IRQn);

}

void actualizarBufferAudio() {
  //La funcion solo procede si se pudo abrir un archivo de audio
  if (!archivoAudio) return;

  //Refresca el buffer de audio mientras quede espacio en el mismo
  while (((pBufferW - pBufferR) & 1023) < 1000) {
    //Copia una muestra (byte) del archivo al buffer
    audioBuffer[pBufferW] = archivoAudio.read();

    //Incrementa la posicion de lectura del archivo, en caso de llegar
    //al punto de retorno, entonces salta de regreso al punto de
    //reinicio (looping)
    pArchivoR++;
    if (pArchivoR >= posRetorno) {
      archivoAudio.seek(posReinicio);
      pArchivoR = posReinicio;
    }

    //Incrementa la posicion de escritura del buffer, en caso de llegar
    //al final del mismo, regresa al inicio (buffer circular)
    pBufferW++;
    pBufferW &= 1023;
  }
}

void TC3_Handler() {
  //Lee el registro de estado para limpiar la bandera de interrupcion
  TC_GetStatus(TC1, 0);

  //Envia la siguiente muestra al DAC solo si hay muestras disponibles
  //en el bufer
  if (((pBufferW - pBufferR) & 1023) > 0) {
    //Lee el dato del buffer y lo envia a ambos canales del DAC
    //(Lo siento, el audio es monaural, no stereo)
    analogWrite(DAC0, audioBuffer[pBufferR]);
    analogWrite(DAC1, audioBuffer[pBufferR]);

    //Avanza el puntero de lectura a la siguiente posicion, en caso que
    //se termine el buffer, vuelve al inicio (buffer circular)
    pBufferR++;
    pBufferR &= 1023;
  }
}
