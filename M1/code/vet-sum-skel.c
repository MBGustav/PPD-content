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


#define NELEM (1<<30)
#define N_THREADS 1


// Variável atômica. Pode ser incrementada sem mutex, usando opearação específica
// _Atomic double total = 0;
// Incremento: __atomic_fetch_add (& total, parcial, __ATOMIC_SEQ_CST);


// Funcao da thread: o que passar como parâmetro?
void *
soma(void *arg)
{

	// Como tratar a soma local? Retorna como parâmetro?
	// Salva de maneira atômica em variável global?
	// Usa vetor global de somas parciais?

	pthread_exit(NULL);
}



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

	// alocação do vetor
	vet = (float *)malloc(nelem * sizeof(float));

	if (!vet) {
		perror("Erro na alocacao do vetor de elementos.");
		return EXIT_FAILURE;
	}

	// gerar sempre a mesma sequência de valores? 
	// srand(time(NULL));

	// atribuição de valores (0<val<=1) aos elementos do vetor 
	// Atenção para a função de geração de valores aleatórios: rand_r é 'thread safe'
	for(i=0; i < nelem; i++) 
		// vet[i] = (float)((float)rand() / (float)RAND_MAX);
		vet[i] = (float)((float)rand_r(&seedp) / (float)RAND_MAX);

	// Será que vale a pena paralelizar a geração de valores também?
	// Numa aplicação real, provavelmente leria dados de arquivo ou estes seriam gerados no código...


	// soma sequencial dos elementos do vetor
	sum = 0;
	for(i=0; i < nelem; i++)
		sum += vet[i];

	// Como fazer a soma em paralelo?

	// 1o: Comentar código acima :-)
	// Criar as threads, atribuindo uma parte do vetor para soma por cada uma delas
	// O que passar como parâmetro? Intervalo do vetor cujos valores cada uma vai calcular...

/*
	// Loop de criacao das threads
	for (int i=0; i < num_threads; i++) {

		status = pthread_create(&threads[i], NULL, soma, ... );

		if (status) {
			printf("Falha da criacao da thread %d (%d)\n", i, status);
			return (EXIT_FAILURE);
		}
	}
*/

	// Juntar as somas parciais? Em qual tarefa?
	// loop de pthread_join...
	// for(i=0; i < num_threads; i++) {
	//		...
	// }

	// Como coletar as somas parciais calculadas pelas threads?
	// Thread retorna valor?  Usar vetor global?
	// Usar variável global incrementada com exclusão mútua?
	// Usar variável global incrementada com instrução atômica?
/*
	// loop de espera pelo término da execução das threads
	for (int i=0; i < num_threads; i++) {

		// join recebendo a soma parcial de cada thread
		status = pthread_join(threads[i], ... );

		if (status) {
			printf("Erro em pthread_join (%d)\n",status);
			break;
		}
	}
*/

	printf("Soma: %f\n",sum);

	// libera o vetor de ponteiros para as threads
	// free(threads);

	// libera o vetor de valores
	free(vet);

	return(0);
}



