#Makefile para la compilacion de la practica 2
 
default: compilar

compilar: 
	@read -p "Introduce el numero de trabajadores: " hijos; \
	mpicc pract2.c -o pract2 -lX11 -DNUMHIJOS=$$hijos -DFILTRO=0

compilarBYN: 
	@read -p "Introduce el numero de trabajadores: " hijos; \
	mpicc pract2.c -o pract2 -lX11 -DNUMHIJOS=$$hijos -DFILTRO=1

compilarSepia: 
	@read -p "Introduce el numero de trabajadores: " hijos; \
	mpicc pract2.c -o pract2 -lX11 -DNUMHIJOS=$$hijos -DFILTRO=2
	
ejecutar: 
	mpirun -np 1 ./pract2

clean:
	-rm -f pract2