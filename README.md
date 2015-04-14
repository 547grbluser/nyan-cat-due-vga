# nyan-cat-due-vga
Una animacion de Nyan Cat implementada en arduino Due con monitor VGA

Demo Basado en la libreria de VGA creada por stimmer:

http://forum.arduino.cc/index.php/topic,150517.0.html

http://stimmer.github.com/DueVGA/

Nosotros desarrollamos un escudo que funciona perfectamente con este demo. Puedes encontrar los detalles aqui:
https://github.com/alsw/tarjetas-de-circuito/tree/master/Due%20VGA%20Shield

Contenidos de los directorios
=============================

- Convertidor_Headers_Imagenes:
Aca encuentras un programa de C++ para linux que permite convertir los headers generados por gimp (archivos .h) en headers compatibles con el sketch de Arduino. Solo ejecuta el makefile con el comando make y corre el programa.

- Imagenes:
Aqui esta el archivo con la animacion completa en formato gimp, asi como los headers generados con gimp (mediante la funcion exportar a .h).

- Imagenes_San_Valentin:
Similar al directorio anterior, pero esta es una version especial elaborada para el dia de San Valentin.

- Memoria_SD:
Contiene el archivo de audio que debe ir en la memoria SD conectada al due. Es la clasica tonada de Nyan Cat convertida a formato pcm puro (basicamente un .wav sin header) en formato de 22050Hz, 8-bits, mono.
