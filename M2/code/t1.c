
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define NUM 10000000
#define MIN_BLK 1024

int _vet[NUM];


int
calc(int start, int finish)
{
   int i, dif;
   int sum = 0, sum1, sum2;

   if (finish-start <= MIN_BLK) {    // calcula
      // printf("Calculando %d -> %d\n", start, finish);

      for (i=start; i < finish; i++)
        sum+=_vet[i];

   } else {                           // divide, criando novas tasks

      // printf("Dividindo %d -> %d\n", start, finish);

      dif = finish - start;

      // variável de retorno deve ser compartilhada para que o valor retornado
      // não seja salvo em uma nova variável da nova task

      #pragma omp task shared(sum1)
      sum1 = calc(start, start + dif / 2);

      #pragma omp task shared(sum2)
      sum2 = calc(finish - dif / 2, finish);

      // Taskwait funciona como uma barreira para tasks, fazendo com que o fluxo de execução
      // seja pausado até que as tasks tenham sido completadas. Taskwait faz com
      // que threads suspendam o que estavam fazendo e passam a atuar nas tasks da fila.
      // The taskwait construct specifies a wait on the completion of child tasks of the current task.
      #pragma omp taskwait
      // Once all tasks have been processed, threads resume their normal execution flow.

      sum = sum1 + sum2;
  }
  return sum;
}

int main()
{
  int i, sum;

  // inicia vetor com valores aleatorios 0..9
  srand(time(NULL));
  for (i=0; i< NUM; i++)
    _vet[i]=rand()%10;

  #pragma omp parallel     // cria um time de threads
  {
    // Questão: queremos ter um time de threads, que irá executar as tarefas
    // que criarmos nas chamadas recursivas. Contudo, é preciso fazer apenas 1
    // chamada à função de cálculo. Isso é feito com a diretiva single()

    #pragma omp single
      sum = calc (0, NUM);
  }

  printf("Soma: %d\n", sum);

  return 0;
}