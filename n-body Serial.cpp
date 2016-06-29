#include <iostream>
#include <stdio.h>
#include <malloc.h>
#include <cmath>
#include <cstring>
#include <omp.h>

using namespace std;

#define X 0
#define Y 1
#define DIM 2

const double G = 6.673e-11;

struct Particula{
    float masa;
    float posicion[DIM];
    float velocidad[DIM];
};

int main(){

    int cantidadParticulas;
    int T;
    float delta_t;
    Particula *particulas = NULL;
    double **fuerzas;

    // Variables para medir el tiempo de ejecucion
    double t_init, t_fin, t_proc;

    printf("Cantidad de Particulas a analizar: ");
    scanf("%d",&cantidadParticulas);
    fflush(stdin);

    particulas = (Particula *)malloc(sizeof(Particula)*cantidadParticulas);
    fuerzas = (double **)malloc(sizeof(double)*cantidadParticulas);
    for(int i = 0; i < DIM; i++){
        fuerzas[i] = (double *)malloc(sizeof(double)*cantidadParticulas);
    }

    for (int i = 0; i < cantidadParticulas; i++){
        printf("Datos particula %d\n", i);
        printf("\tMasa de la particula  : ");
        scanf("%f", &particulas[i].masa);
        printf("\tPosicion Inicial en X : ");
        scanf("%f", &particulas[i].posicion[X]);
        printf("\tPosicion Inicial en Y : ");
        scanf("%f", &particulas[i].posicion[Y]);
        printf("\tVelocidad Inicial en X: ");
        scanf("%f", &particulas[i].velocidad[X]);
        printf("\tVelocidad Inicial en Y: ");
        scanf("%f", &particulas[i].velocidad[Y]);
    }

    printf("Ingrese el paso de tiempo en segundos: ");
    scanf("%f", &delta_t);
    printf("Ingrese la cantidad de paso: ");
    scanf("%d", &T);

    t_init = omp_get_wtime();

    for(int t = 0; t < T; t++){

        printf("\nEn t = %f\n", t*delta_t);
        for(int q = 0; q < cantidadParticulas; q++){
            printf("\tParticula %d\n\n", q);
            printf("\t\tPosicion, componente en X: %e\n", particulas[q].posicion[X]);
            printf("\t\tPosicion, componente en Y: %e\n\n", particulas[q].posicion[Y]);
            printf("\t\tVelocidad, componente en X: %e\n", particulas[q].velocidad[X]);
            printf("\t\tVelocidad, componente en Y: %e\n\n", particulas[q].velocidad[Y]);
        }

        // Calculo de las fuerzas

        //memset(fuerzas,0,cantidadParticulas*sizeof(double [DIM]));

        for(int i = 0; i < cantidadParticulas; i++){
            for(int j = 0; j < DIM; j++){
                fuerzas[i][j] = 0;
            }
        }

        float x_dif, y_dif;
        float distancia, dist_cubo;
        double fuerza_qk[DIM];

        for(int q = 0; q < cantidadParticulas; q++){
            for(int k = q + 1; (k > q) && (k < cantidadParticulas); k++){
                x_dif = particulas[q].posicion[X] - particulas[k].posicion[X];
                y_dif = particulas[q].posicion[Y] - particulas[k].posicion[Y];

                distancia = sqrt(x_dif*x_dif + y_dif*y_dif);
                dist_cubo = distancia*distancia*distancia;

                fuerza_qk[X] = -(G*particulas[q].masa*particulas[k].masa/dist_cubo)*x_dif;
                fuerza_qk[Y] = -(G*particulas[q].masa*particulas[k].masa/dist_cubo)*y_dif;

                fuerzas[q][X] += fuerza_qk[X];
                fuerzas[q][Y] += fuerza_qk[Y];
                fuerzas[k][X] -= fuerza_qk[X];
                fuerzas[k][Y] -= fuerza_qk[Y];
            }
        }

        // Calculo de las posiciones y velocidades
        for(int q = 0; q < cantidadParticulas; q++){
            particulas[q].posicion[X] += delta_t*particulas[q].velocidad[X];
            particulas[q].posicion[Y] += delta_t*particulas[q].velocidad[Y];
            particulas[q].velocidad[X] += (delta_t/particulas[q].masa)*fuerzas[q][X];
            particulas[q].velocidad[Y] += (delta_t/particulas[q].masa)*fuerzas[q][Y];
        }

        for(int q = 0; q < cantidadParticulas; q++){
            printf("\t\tFuerza, componente en X: %e\n", fuerzas[q][X]);
            printf("\t\tFuerza, componente en Y: %e\n\n", fuerzas[q][Y]);
        }
    }

    t_fin = omp_get_wtime();
    t_proc = t_fin - t_init;

    printf("\nEn t = %f\n", T*delta_t);
    for(int q = 0; q < cantidadParticulas; q++){
        printf("\tParticula %d\n\n", q);
        printf("\t\tPosicion, componente en X: %e\n", particulas[q].posicion[X]);
        printf("\t\tPosicion, componente en Y: %e\n\n", particulas[q].posicion[Y]);
        printf("\t\tVelocidad, componente en X: %e\n", particulas[q].velocidad[X]);
        printf("\t\tVelocidad, componente en Y: %e\n\n", particulas[q].velocidad[Y]);
        printf("\t\tFuerza, componente en X: %e\n", fuerzas[q][X]);
        printf("\t\tFuerza, componente en Y: %e\n\n", fuerzas[q][Y]);
    }

    printf("\nEl el proceso tomo : %f segundos\n", t_proc);

    return 0;
}
