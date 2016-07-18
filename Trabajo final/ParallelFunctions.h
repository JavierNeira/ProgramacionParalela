#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <omp.h>

using namespace std;
using namespace cv;

void inicilizacionMatrices(int);
void on_serial(int);
void parallel_whole_image(int);
void parallel_block_image(int);
void parallel_group_image(int);
void show_final_images(int);
void destruirMatrices();

const int NUM_THREADS = 16;

string nombreImagen = "Imagenx.jpg";
Mat *imagenes, *detBordes;

void InicializacionMatrices(int cantImagenes){
	imagenes = new Mat[cantImagenes]();
	detBordes = new Mat[cantImagenes]();
}

void on_serial(int cantImagenes){

	InicializacionMatrices(cantImagenes);

	for (int i = 0; i < cantImagenes; i++){
		imagenes[i] = imread(nombreImagen);
		if (!imagenes[i].data) {
			cout << "Error al cargar la imagen: " << nombreImagen << endl;
			exit(-1*i);
		}
	}

	for (int i = 0; i < cantImagenes; i++){
		Canny(imagenes[i], detBordes[i], 100, 150, 3);
	}

}

void parallel_whole_image(int cantImagenes){

	int numThreads;

	InicializacionMatrices(cantImagenes);

	numThreads = (cantImagenes < 64)?cantImagenes:64;

	#pragma omp parallel firstprivate(nombreImagen) num_threads(numThreads)
	{
		int total_threads = omp_get_num_threads();
		int ID_thread = omp_get_thread_num();

		for(int i = ID_thread; i < cantImagenes; i += total_threads){
			imagenes[i] = imread(nombreImagen);
			if (!imagenes[i].data) {
				cout << "Error al cargar la imagen: " << nombreImagen << endl;
				exit(-1*i);
			}
		}

		// Deteccion de bordes en imagenes completas
		for(int i = ID_thread; i < cantImagenes; i += total_threads){
			Canny(imagenes[i], detBordes[i], 100, 150, 3);
		}
	}

}

void parallel_block_image(int cantImagenes){

	InicializacionMatrices(cantImagenes);

	#pragma omp parallel firstprivate(nombreImagen) num_threads(NUM_THREADS)
	{
		int ID_thread = omp_get_thread_num();

		for(int i = ID_thread; i < cantImagenes; i += NUM_THREADS){

			imagenes[i] = imread(nombreImagen);
			if (!imagenes[i].data) {
				cout << "Error al cargar la imagen: " << nombreImagen << endl;
				exit(-1*i);
			}
		}

	}

	for(int i = 0; i < cantImagenes; i++){
		Mat bloques[NUM_THREADS];
		#pragma omp parallel num_threads(NUM_THREADS)
		{
			int ID_thread = omp_get_thread_num();

			int ancho_imagen, alto_imagen, ancho_bloque;
			Rect rectangulo;

			// Deteccion de bordes en imagenes particionadas
			ancho_imagen = imagenes[i].cols;
			alto_imagen = imagenes[i].rows;
			ancho_bloque = ancho_imagen/NUM_THREADS;

			rectangulo.x = ID_thread*ancho_bloque;
			rectangulo.y = 0;
			rectangulo.height = alto_imagen;

			if(ID_thread + 1 < NUM_THREADS){				
				rectangulo.width = ancho_bloque;
			}
			else{
				rectangulo.width = ancho_imagen - ID_thread*ancho_bloque;
			}

			Mat bloque(imagenes[i],rectangulo);

			Canny(bloque, bloque, 100, 150, 3);

			bloques[ID_thread] = bloque.clone();
			
		}

		hconcat(bloques[0], bloques[1], detBordes[i]);
		for(int j = 2; j < NUM_THREADS; j++){
			hconcat(detBordes[i], bloques[j], detBordes[i]);
		}
	}
}

void parallel_group_image(int cantImagenes){

	int numThreads;

	InicializacionMatrices(cantImagenes);

	numThreads = cantImagenes;

	#pragma omp parallel firstprivate(nombreImagen) num_threads(numThreads)
	{
		int ID_thread = omp_get_thread_num();

		for(int i = ID_thread; i < cantImagenes; i += NUM_THREADS){

			imagenes[i] = imread(nombreImagen);
			if (!imagenes[i].data) {
				cout << "Error al cargar la imagen: " << nombreImagen << endl;
				exit(-1*i);
			}
		}

	}

	numThreads = cantImagenes*NUM_THREADS;
	if (numThreads >= 64) numThreads = 64;

	Mat (*bloques)[NUM_THREADS] = new Mat[cantImagenes][NUM_THREADS];

	#pragma omp parallel num_threads(numThreads)
	{
		int ID_thread = omp_get_thread_num();

		int ancho_imagen, alto_imagen, ancho_bloque;
		Rect rectangulo;

		for(int i = floor((float)(ID_thread/NUM_THREADS)); i < cantImagenes; i += 4){
		
			// Deteccion de bordes en imagenes particionadas
			ancho_imagen = imagenes[i].cols;
			alto_imagen = imagenes[i].rows;
			ancho_bloque = ancho_imagen/NUM_THREADS;

			rectangulo.x = (ID_thread%NUM_THREADS)*ancho_bloque;
			rectangulo.y = 0;
			rectangulo.height = alto_imagen;

			if((ID_thread%NUM_THREADS) + 1 < NUM_THREADS){				
				rectangulo.width = ancho_bloque;
			}
			else{
				rectangulo.width = ancho_imagen - (ID_thread%NUM_THREADS)*ancho_bloque;
			}

			Mat bloque(imagenes[i],rectangulo);

			Canny(bloque, bloque, 100, 150, 3);

			bloques[i][ID_thread%NUM_THREADS] = bloque.clone();
		}
			
	}

	for(int i = 0; i < cantImagenes; i++){
		hconcat(bloques[i][0], bloques[i][1], detBordes[i]);
		for(int j = 2; j < NUM_THREADS; j++){
			hconcat(detBordes[i], bloques[i][j], detBordes[i]);
		}
	}

}

void show_final_images(int cantImagenes){

	for(int i = 0; i < cantImagenes; i++){
		nombreImagen[6] = char('1' + i);
		namedWindow(nombreImagen, CV_WINDOW_AUTOSIZE);
		imshow(nombreImagen, detBordes[i]);
	}

}

void destruirMatrices(){
	delete[] imagenes;
	delete[] detBordes;
}
