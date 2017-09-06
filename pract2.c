/* Pract2  RAP 09/10    Javier Ayllon*/
/* Modificacion por: Cristian Trapero Mora */
/* Compilar con: mpicc pract2.c -o pract2 -lX11 -DNUMHIJOS=$$hijos */

#include <openmpi/mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h> 
#include <assert.h>   
#include <unistd.h>   

#define NIL (0)     
//#define NUMHIJOS 8
//#define FILTRO 0
#define IMAGEN "foto.dat"
#define TAMANIOIMAGEN 400


/*Variables Globales */

XColor colorX;
Colormap mapacolor;
char cadenaColor[]="#000000";
Display *dpy;
Window w;
GC gc;

/*Funciones auxiliares */

/* Funcion que inicializa el servidor X11 para trabajar con ventanas */
void initX() {

	dpy = XOpenDisplay(NIL);
	assert(dpy);

	int blackColor = BlackPixel(dpy, DefaultScreen(dpy));
	int whiteColor = WhitePixel(dpy, DefaultScreen(dpy));

	w = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0, 400, 400, 0, whiteColor, whiteColor);
	XSelectInput(dpy, w, StructureNotifyMask);
	XMapWindow(dpy, w);
	gc = XCreateGC(dpy, w, 0, NIL);
	XSetForeground(dpy, gc, blackColor);

	for(;;) {
		XEvent e;
		XNextEvent(dpy, &e);
		if (e.type == MapNotify)
			break;
	}

	mapacolor = DefaultColormap(dpy, 0);

}

/* Funcion que dibuja un punto */
void dibujaPunto(int x, int y, int r, int g, int b) {

	sprintf(cadenaColor,"#%.2X%.2X%.2X", r, g, b);
	XParseColor(dpy, mapacolor, cadenaColor, &colorX);
	XAllocColor(dpy, mapacolor, &colorX);
	XSetForeground(dpy, gc, colorX.pixel);
	XDrawPoint(dpy, w, gc, x, y);
	XFlush(dpy);

}


/* Funcion que permite recibir los puntos desde los trabajadores y dibujarlos */
void recibirPuntos(MPI_Comm commPadre){

	/* Buffer auxiliar con las coordenadas y el color a pintar*/
	int puntoADibujar[5];
	int i;

	/* Obtenemos punto a punto todos los de la matriz */
	for(i=0; i<TAMANIOIMAGEN*TAMANIOIMAGEN; i++){

		/*Recibimos el punto a pintar */
		MPI_Recv(&puntoADibujar, 5, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, commPadre, MPI_STATUS_IGNORE);

		/*Dibujamos el punto*/
		dibujaPunto(puntoADibujar[0], puntoADibujar[1], puntoADibujar[2], puntoADibujar[3], puntoADibujar[4]);
	}
}



/* Programa principal */

int main (int argc, char *argv[]) {

	int rank, size;
	MPI_Status status;

	/*Intercomunicador de los procesos trabajadores */
	MPI_Comm commPadre;

	/* Codigos de error del lanzamiento de los trabajadores */
	int codigosError[NUMHIJOS];

	/* Buffer que almacena el punto a pintar con las coordenadas (x,y) y el color RGB */
	int puntoAPintar[5];

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_get_parent(&commPadre);

	/* Si es el maestro */
	if ((commPadre==MPI_COMM_NULL) && (rank==0)){

		/* Codigo del maestro */
		initX();

		/* Lanzo procesos trabajadores */
		MPI_Comm_spawn("pract2", MPI_ARGV_NULL, NUMHIJOS, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &commPadre, codigosError);

		/* Recibimos los puntos de los trabajadores y los dibujamos */
		recibirPuntos(commPadre);

		printf("Imagen dibujada. Sera mostrada durante 5 segundos.\n");

		/* Esperamos 5 segundos para poder visualizar la imagen */
		sleep(5);

	/* Si es un trabajador */
	}else {

	/* Codigo de todos los trabajadores */
	/* El archivo sobre el que debemos trabajar es foto.dat */

		/* Numero de filas que leera cada trabajador */
		/* 400/10 = 40 lineas por trabajador */
		int filasPorTrabajador=TAMANIOIMAGEN/NUMHIJOS;

		/* Tamanio del bloque que leera cada trabajador */
		/* 40 lineas * 400 columnas * 3 datos RGB * tamaño de un char = 48000 char */
		int bloquePorTrabajador=filasPorTrabajador*TAMANIOIMAGEN*3*sizeof(unsigned char);

		/* Variables que nos permitiran saber a partir que datos de la matriz leer */
		int inicioLectura=rank*filasPorTrabajador;
		int finalLectura=inicioLectura+filasPorTrabajador;

		/* Manejador de ficheros de MPI */
		MPI_File manejadorArchivo;

		/*Abrimos el archivo con permisos de lectura */
		MPI_File_open(MPI_COMM_WORLD, IMAGEN, MPI_MODE_RDONLY, MPI_INFO_NULL, &manejadorArchivo);

		/* Solo accedemos a los datos del fichero correspondientes a mi rank */
		MPI_File_set_view(manejadorArchivo, rank*bloquePorTrabajador, MPI_UNSIGNED_CHAR, MPI_UNSIGNED_CHAR, "native", MPI_INFO_NULL);

		/* Variable auxiliar que almacena el color de un punto de la matriz */
		unsigned char color[3];

		int i;
		int j;

		if(rank==NUMHIJOS-1){
			finalLectura=TAMANIOIMAGEN;
		}
		
		for(i=inicioLectura; i<finalLectura; i++){
			for(j=0; j<TAMANIOIMAGEN; j++){

				MPI_File_read(manejadorArchivo, color, 3, MPI_UNSIGNED_CHAR, &status);
				puntoAPintar[0]=j;
				puntoAPintar[1]=i;

				/* Aplicamos un filtro a la imagen */
				/* R G B */
				switch(FILTRO){

					/* Sin filtro */
					case 0:
						/*Rojo*/ puntoAPintar[2]=(int)color[0];
						/*Verde*/puntoAPintar[3]=(int)color[1];
						/*Azul*/ puntoAPintar[4]=(int)color[2];
						break;

					/* Filtro en blanco y negro */
					case 1:
						/*Rojo*/ puntoAPintar[2]=(((int)color[0])*0.393)+(((int)color[1])*0.769)+(((int)color[2])*0.189);
						/*Verde*/puntoAPintar[3]=(((int)color[0])*0.393)+(((int)color[1])*0.769)+(((int)color[2])*0.189);
						/*Azul*/ puntoAPintar[4]=(((int)color[0])*0.393)+(((int)color[1])*0.769)+(((int)color[2])*0.189);
						break;

					/*Filtro en sepia */
					case 2:
						/*Rojo*/ puntoAPintar[2]=(((int)color[0])*0.393)+(((int)color[1])*0.769)+(((int)color[2])*0.189);
						/*Verde*/puntoAPintar[3]=(((int)color[0])*0.349)+(((int)color[1])*0.686)+(((int)color[2])*0.168);
						/*Azul*/ puntoAPintar[4]=(((int)color[0])*0.272)+(((int)color[1])*0.534)+(((int)color[2])*0.131);
						break;
				}

				/* Controlamos que no sea un color correcto */
				if(puntoAPintar[2]>255){
					puntoAPintar[2]=255;
				}

				if(puntoAPintar[3]>255){
					puntoAPintar[3]=255;
				}

				if(puntoAPintar[4]>255){
					puntoAPintar[4]=255;
				}

				MPI_Send(&puntoAPintar, 5, MPI_INT, 0, 1, commPadre);
			}
		}

		/*Cerramos el archivo */
		MPI_File_close(&manejadorArchivo);
	}

	MPI_Finalize();
	return 0;
}

