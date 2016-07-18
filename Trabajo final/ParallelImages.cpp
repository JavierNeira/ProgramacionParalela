#include "ParallelFunctions.h"

int main(){
	double t_init, t_proc;
	int cantImagenes;

	cout << "Ingrese a cantidad de imagenes que seran procesadas: ";
	cin >> cantImagenes;

	// Version serial, las imagenes se procesan una por una
	t_init = omp_get_wtime();			// Inicio de medicion de tiempo de ejecucion
	on_serial(cantImagenes);			// Realizacion del procesamiento serial
	t_proc = omp_get_wtime() - t_init;	// Finalizacion de medicion del tiempo de ejecucion
	printf("\nProcesamiento Serial:\n\ttiempo de ejecucion = %f\n\n", t_proc);
	destruirMatrices();


	// Version en la que se tiene tantos threads como imagenes,
	// cada thread procesa una imagen completa.
	t_init = omp_get_wtime();			// Inicio de medicion de tiempo de ejecucion
	parallel_whole_image(cantImagenes);	// Realizacion del procesamiento paralelo
	t_proc = omp_get_wtime() - t_init;	// Finalizacion de medicion del tiempo de ejecucion
	printf("\nProcesamiento Paralelo en imagenes completas una por una:\n\ttiempo de ejecucion = %f\n\n", t_proc);
	destruirMatrices();


	// Version en la que se tiene una cantidad fija de threads,
	// la cantidad de treads sera la misma en la que cada imagen
	// subdivida para ser procesada por bloques
	t_init = omp_get_wtime();			// Inicio de medicion de tiempo de ejecucion
	parallel_block_image(cantImagenes);	// Realizacion del procesamiento paralelo
	t_proc = omp_get_wtime() - t_init;	// Finalizacion de medicion del tiempo de ejecucion
	printf("\nProcesamiento Paralelo en imagenes divididas en bloques una por una:\n\ttiempo de ejecucion = %f\n\n", t_proc);
	destruirMatrices();


	// Version en la que se tienen una cantidad fija de threads
	// por cada una de las imagenes, de tal manera que cada imagen
	// es dividida en esa cantidad fija, danto en total tantos threads
	// como imagenes por esa cantidad fija
	t_init = omp_get_wtime();			// Inicio de medicion de tiempo de ejecucion
	parallel_group_image(cantImagenes);
	t_proc = omp_get_wtime() - t_init;	// Finalizacion de medicion del tiempo de ejecucion
	printf("\nProcesamiento Paralelo en imagenes divididas en bloques engrupos de 4:\n\ttiempo de ejecucion = %f\n\n", t_proc);
	destruirMatrices();


	// Salida de datos
	//show_final_images(cantImagenes);

	system("pause");

	//Esperar a pulsar una tecla
	//cvWaitKey(0);
	return 0;

}
