# Diseño de infraestructura de red: Sistema distribuido de renderizado de gráficos

## Enunciado
Utilizaremos las primitivas pertinentes MPI2 como acceso paralelo a disco y gestión de procesos dinámico: 

* Inicialmente el usuario lanzará un solo proceso mediante mpirun -np 1 ./pract2. Con ello MPI lanza un primer proceso que será el que tiene acceso a la pantalla de gráficos pero no a disco. Él mismo será el encargado de levantar N procesos (con N definido en tiempo de compilación como una constante) que tendrán acceso a disco pero no a gráficos directamente.

* Los nuevos procesos lanzados se encargarán de leer de forma paralela los datos del archivo foto.dat. Después, se encargarán de ir enviando los pixeles al primer elemento de proceso para que éste se encargue de representarlo en pantalla. Usaremos la plantilla pract2.c para comenzar a desarrollar la práctica. En ella debemos completar el código que ejecuta el proceso con acceso a la ventana de gráficos (rank 0 inicial) y la de los procesos “trabajadores”.

Se proporciona el archivo foto.dat. La estructura interna de este archivo es 400 filas por 400 columnas de puntos. Cada punto está formado por una tripleta de tres “unsigned char” correspondiendo al valor R, G y B de cada uno de los colores primarios. Estos valores se pueden usar para la función dibujaPunto.

## Instalar OpenMPI
Para que le proyecto funcione correctamente, es necesario tener instaladas las librerias de OpenMPI. Las instrucciones podeis encontrarlas en el siguiente enlace: [http://lsi.ugr.es/jmantas/pdp/ayuda/instalacion.php?ins=openmpi](http://lsi.ugr.es/jmantas/pdp/ayuda/instalacion.php?ins=openmpi) 

## Compilación y ejecución del proyecto
Para compilar los programas simplemente debemos de ejecutar las siguientes instrucciones en un terminal en el directorio:

```bash
make compilar
```
Compilar con filtro escala de grises::
```bash
make compilarBYN
```
Compilar con filtro sepia:
```bash
make compilarSepia
```
Si queremos ejecutar el programa simplemente tenemos que ejecutar la siguiente instrucción:
```bash
make ejecutar
```


## Informe
Todos las explicaciones pertinentes del diseño del programa y como ejecutarlo, están reflejadas en el [informe en PDF](https://github.com/critramo/DIR-RenderizadoGraficoDistribuido/blob/master/Informe.pdf).
