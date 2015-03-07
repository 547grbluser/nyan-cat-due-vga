//Programa usado para tomar todos los headers generados por gimp (cuadros
//individuales) y consolidarlos en un solo header de salida

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char nombreArchivoSal[] = "../Cuadros_San_V.h";
const char plantillaArchivoEnt[] = "../Imagenes_San_Valentin/Cuadro%i.h";
const int numArchivos = 12;

void main() {
  int i;
  FILE *pArchSal;
  FILE *pArchEnt;
  char nombreArchEnt[256];
  int tamArchEnt;
  char *datosEnt;
  char *subCad1;
  char *subCad2;
  char *subCadFin;
  int nBytes;

  //Abre el archivo para escritura
  pArchSal = fopen(nombreArchivoSal, "w");

  //Procesa todos los archivos de entrada
  for (i=1; i<=numArchivos; i++) {
    //Determina el nombre del archivo abrir para esta iteracion
    sprintf(nombreArchEnt, plantillaArchivoEnt, i);

    //Abre el archivo para lectura
    pArchEnt = fopen(nombreArchEnt, "r");

    //Determina su tamano
    fseek(pArchEnt, 0, SEEK_END);
    tamArchEnt = ftell(pArchEnt);
    fseek(pArchEnt, 0, SEEK_SET);

    //Aloja memoria suficiente para cargarlo
    datosEnt = malloc(tamArchEnt+1);

    //Lee el contenido y le anexa un 0 terminador
    fread(datosEnt, 1, tamArchEnt, pArchEnt);
    datosEnt[tamArchEnt] = 0;

    //Imprime la llave de apertura
    fprintf(pArchSal, "{\n");

    //Busca la posicion de inicio de los datos de imagen
    subCad1 = strstr(datosEnt, "static char header_data[] = {");
    //Busca la posicion final de los datos de imagen
    subCadFin = strstr(subCad1, "};");

    //Extrae todas las lineas de texto con los datos de imagen y las traslada al
    //archivo de salida
    while (1) {
      //Busca la posicion despues del siguiente tabulador
      subCad1 = strstr(subCad1, "\t") + 1;
      //Si la posicion alcanza o excede el final, termina de procesar el archivo
      if (subCad1 >= subCadFin) break;

      //Busca la posicion del siguiente fin de linea
      subCad2 = strstr(subCad1, "\n");
      //Determina cuantos bytes (caracteres) tiene la fila
      nBytes = (int) (subCad2 - subCad1);

      //Imprime el contenido de la fila, precedido de 2 espacios y continuado
      //por un fin de linea
      fprintf(pArchSal, "  ");
      fwrite(subCad1, 1, nBytes, pArchSal);
      fprintf(pArchSal, "\n");
    }

    //Imprime la llave de cierre
    fprintf(pArchSal, "},\n");

    //Una vez terminado, desaloja la memoria y cierra el archivo de entrada
    free(datosEnt);
    fclose(pArchEnt);
  }

  //Desaloja todos los recursos
  fclose(pArchSal);
}