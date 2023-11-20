/* 
 ** Universidade Federal de São Carlos
 ** Departamento de Computação
 ** Prof. Hélio Crestana Guardia
 */

/*
 ** Programa : soma dos elementos de um vetor
 ** Comando: 
 ** Objetivo: observar passagem de valores para função da thread.
              Nesse caso, usa-se vetor em que cada thread preenche 
              os resultados da sua operação. 
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h> 
#include <limits.h> 
#include <time.h>
#include <omp.h>

#define NELEM (1<<25)
#define N_THREADS 8


// Variável atômica. Pode ser incrementada sem mutex, usando opearação específica
// _Atomic double total = 0;
// Incremento: __atomic_fetch_add (& total, parcial, __ATOMIC_SEQ_CST);

int
main(int argc, char *argv[])
{
	int i;
	float *vet;
	double sum;
	long int nelem;
	unsigned int seedp;

	// int num_threads = 1;
	// vetor de pthread_t para salvamento dos identificadores das threads
	// pthread_t *threads; 

	if(argc > 1)
		nelem = atoi(argv[1]);
	else
		nelem = NELEM;

	// alocação do vetor
	vet = (float *)malloc(nelem * sizeof(float));

	if (!vet) {
		perror("Erro na alocacao do vetor de elementos.");
		return EXIT_FAILURE;
	}
	
	srand(1);
	
	// atribuição de valores (0<val<=1) aos elementos do vetor 
	// Atenção para a função de geração de valores aleatórios: rand_r é 'thread safe'

	#pragma omp parallel for 
	for(i=0; i < nelem; i++) 
		// vet[i] = (float)((float)rand() / (float)RAND_MAX);
		vet[i] = (float)((float)rand_r(&seedp) / (float)RAND_MAX);

	// Será que vale a pena paralelizar a geração de valores também?
	// Numa aplicação real, provavelmente leria dados de arquivo ou estes seriam gerados no código...

	// soma sequencial dos elementos do vetor
	sum = 0;
	#pragma omp parallel for reduction(+:sum)
	for(i=0; i < nelem; i++)
		sum += vet[i];

	printf("Soma: %f\n",sum);

	// libera o vetor de ponteiros para as threads
	// free(threads);

	// libera o vetor de valores
	free(vet);

	return(0);
}



