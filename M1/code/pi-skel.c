/* 
 ** Universidade Federal de São Carlos
 ** Departamento de Computação
 ** Prof. Hélio Crestana Guardia
 */

/*
 ** Programa : 
 ** Comando: 
 ** Objetivo: observar passagem de valores para função da thread.
              Nesse caso, usa-se vetor em que cada thread preenche 
              os resultados da sua operação. 
*/

/* 
 Algoritmo de Monte Carlo para cálculo do valor de pi 
 Dados um círculo de raio 1 inserido em um quadrado de lado 2.
 Relação entre pontos aleatórios que estão dentro do círculo 
 e pontos no quadrado pode ser usada para cálculo de pi.

 Relação entre as áreas: (p*r^2)/(4*r^2)
 Considerando r=1: (p/4)=pontos_no_círculo/total_pontos
 p=4*pontos_no_círculo/total_pontos

 Considerando a geração de valores aleatórios entre 0 e 1 (raio)
 para as coordenadas x e y de um ponto, usa-se a equação do teorema 
 de Pitágoras para determinar se ponto está dentro do círculo ou não:
 z^2=x^2+y^2
*/

#include <unistd.h>
#include <stdio.h> 
#include <pthread.h> 
#include <stdlib.h> 
#include <limits.h> 

#define N_THREADS 8
#define SAMPLES 2<<25 

unsigned short seed[3];		// semente usada com valores aleatórios na função erand48()

// Variável atômica. Pode ser incrementada sem mutex, usando opearação específica
// _Atomic total = 0;
// Incremento: __atomic_fetch_add (& total, parcial, __ATOMIC_SEQ_CST);


// funcao da thread: o que passar como parâmetro?
void *
calcula(void *arg)
{
	double x, y;  // coordenadas do ponto
	long int hits = 0;

	//collect data from argument 
	int amostras = *(int*) arg;

	for (long i=0; i < amostras; i++) {
		// Geração de números aleatórios entre 0 e 1, usando a função eradnd48()
		x=erand48(seed);
		y=erand48(seed);

		// verifica se coordenadas do ponto estão dentro do círculo
		if(x*x+y*y <= 1.0)
			hits++;
	}
	printf("val from thread: %d\n", hits);
	pthread_exit((long int) hits );
}


int
main(int argc, char **argv) 
{ 
	int  status;
	long amostras, fracao, parcial;
	int hits =0;	
	pthread_t threads[N_THREADS];

	double pi;

	seed[0] = 0.5; seed[1] = 0.5; seed[2] = (unsigned short)time(NULL);

	if(argc > 1)
		amostras = atoi(argv[1]);
	else
		amostras = SAMPLES;
/*
	// passando o número de threads como argumanto para a função main...
	if(argc > 2) {
		num_threads = atoi(argv[2]);
		if(num_threads <= 0) {
			printf("Númreo de threads inválido...\n");
			return(EXIT_FAILURE);
		}
	} 
	// aloca memória para o vetor de thread IDs
	threads = (pthread_t *)malloc(num_threads * sizeof(pthread_t));	
*/

	// Loop de criacao das threads
	long int total_hits=0;
	long int samples_per_thread = (int) amostras/N_THREADS;
	for (int i=0; i < N_THREADS; i++) {
		status = pthread_create(&threads[i], NULL, calcula, (void*) &samples_per_thread);
		if (status) {
			printf("Falha da criacao da thread %d (%d)\n", i, status);
			return (EXIT_FAILURE);
		}
	}

	// loop de espera pelo término da execução das threads
	for (int i=0; i < N_THREADS; i++) 
	{
		// join recebendo a soma parcial de cada thread
		status = pthread_join(threads[i], (void*) &hits);
		// printf("val from thread[%i] : %d\n",i, hits);
		total_hits += (long int)hits;
		if (status)
		{
			printf("Erro em pthread_join (%d)\n",status);
			break;
		}
	}


	// printf("hits: %ld\n",hits);
 	pi=(double) (4 * (double)((double)total_hits / (double)amostras));

	printf("Pi: %.16f\n",pi);

	return(0);
} 

