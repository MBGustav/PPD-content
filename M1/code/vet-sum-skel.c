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


// Variável atômica. Pode ser incrementada sem mutex, usando opearação específica
// _Atomic double total = 0;
// Incremento: __atomic_fetch_add (& total, parcial, __ATOMIC_SEQ_CST);

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h> 
#include <limits.h> 
#include <time.h>

//macros declaration
#define min(a,b) (((a) < (b)) ? (a) : (b))

//threads parameter
#define N_THREADS 8
#define CACHE_LINE_SIZE (128)

//vector parameter
#define data_t double
#define NELEM (1<<20)

//passagem de parametros por meio de struct
typedef struct arg_soma{
	int stride;
	int first_element;
	int num_thread;
	data_t *vector;
}arg_soma;

//resolvido: false sharing -> criamos um padding entre os dados.
typedef struct __attribute__((aligned(CACHE_LINE_SIZE))){
    data_t val;
}return_soma;


//devo criar essa estrutura em tempo de execucao ?
return_soma ReturnSoma[N_THREADS];



// Funcao da thread: o que passar como parâmetro?
void *
soma(void *arg)
{	
	arg_soma args  = *(arg_soma*) arg;
	// ReturnSoma[args.num_thread].val =0.0f;
	// int size = args.stride;
	for(int i = args.first_element; i <args.stride; i++)
		ReturnSoma[args.num_thread].val += args.vector[i];
	// printf("thread[%i], %i, %i, %i\n", args.num_thread, args.stride, args.first_element, -args.first_element + args.stride);
	// printf("result:%f\n\n", ReturnSoma[args.num_thread].val);
	pthread_exit(NULL);
}



int
main(int argc, char *argv[])
{
	int i;
	data_t *vet;
	data_t sum = 0.0f;
	long int nelem;
	unsigned int seedp;
	int status;
	const int num_threads = N_THREADS;
	// vetor de pthread_t para salvamento dos identificadores das threads
	pthread_t threads[num_threads]; 

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
	vet = (data_t *)malloc(nelem * sizeof(data_t));

	if (!vet) {
		perror("Erro na alocacao do vetor de elementos.");
		return EXIT_FAILURE;
	}

	// gerar sempre a mesma sequência de valores? 
	// srand(time(NULL));
	srand(777);
	data_t checker = 0.0f;
	// atribuição de valores (0<val<=1) aos elementos do vetor 
	// Atenção para a função de geração de valores aleatórios: rand_r é 'thread safe'
	for(i=0; i < nelem; i++) {
		// vet[i] = (data_t)((data_t)rand() / (data_t)RAND_MAX);
		vet[i] = (data_t)((data_t)rand_r(&seedp) / (data_t)RAND_MAX);
		ReturnSoma[num_threads].val = 0.0f;
		checker += vet[i];
	}

	// Será que vale a pena paralelizar a geração de valores também?
	// Numa aplicação real, provavelmente leria dados de arquivo ou estes seriam gerados no código...


	// soma sequencial dos elementos do vetor
	// 1o: Comentar código acima :-)
	// Criar as threads, atribuindo uma parte do vetor para soma por cada uma delas
	// O que passar como parâmetro? Intervalo do vetor cujos valores cada uma vai calcular...

	
	int elem_per_threads = (int) nelem / num_threads;
	
	arg_soma args[num_threads];

	// Loop de criacao das threads
	for (int i=0; i < num_threads; i++) {
		
		
		args[i].stride = (int) min((1+i)*elem_per_threads, nelem);
		args[i].first_element = (int) i*elem_per_threads;
		args[i].num_thread = i;
		args[i].vector = vet;

		status = pthread_create(&threads[i], NULL, soma, (void *)&args[i]);

		if (status) {
			printf("Falha da criacao da thread %d (%d)\n", i, status);
			return (EXIT_FAILURE);
		}
	}


	// Juntar as somas parciais? Em qual tarefa?
	// loop de pthread_join...
	// for(i=0; i < num_threads; i++) {
	//		...
	// }

	// Como coletar as somas parciais calculadas pelas threads?
	// Thread retorna valor?  Usar vetor global?
	// Usar variável global incrementada com exclusão mútua?
	// Usar variável global incrementada com instrução atômica?
	// loop de espera pelo término da execução das threads
	for (int i=0; i < num_threads; i++) {
		// join recebendo a soma parcial de cada thread
		status = pthread_join(threads[i], NULL);

		if (status) {
			printf("Erro em pthread_join (%d)\n",status);
			break;
		}
		sum += ReturnSoma[i].val;
	}

	printf("Soma:     %3.10f\n",sum);
	printf("Expected: %3.10f\n",checker);

	// libera o vetor de ponteiros para as threads
	// free(threads);

	// libera o vetor de valores
	free(vet);

	return(0);
}



