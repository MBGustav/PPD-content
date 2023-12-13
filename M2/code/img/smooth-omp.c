/* 
 * Universidade Federal de São Carlos
 * Departamento de Computação
 * Prof. Hélio Crestana Guardia
 */

/*
 * Programa : smooth (stencil de suavização pela média)
 * Objetivo: 
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define TILE_SIZE (16)
#define WIDTH  (512)
#define HEIGHT (512)

#define _idx(x, y) ((WIDTH)*(x) + (y))


#define smooth_filter(A,i, j) (( A[_idx(i-1, j-1)] +   A[_idx(i-1, j)] + A[_idx(i-1, j+1)] +  \
	      		                 A[_idx(i  , j-1)] +   A[_idx(i  , j)] + A[_idx(i  , j+1)] +  \
      			                 A[_idx(i+1, j-1)] +   A[_idx(i+1, j)] + A[_idx(i+1, j+1)] ) >> 4);


int mr [WIDTH * HEIGHT];
int mg [WIDTH * HEIGHT];
int mb [WIDTH * HEIGHT];
int ma [WIDTH * HEIGHT];
int mr2[WIDTH * HEIGHT];
int mg2[WIDTH * HEIGHT];
int mb2[WIDTH * HEIGHT];
int ma2[WIDTH * HEIGHT];

int 
main(int argc, char **argv)
{
	int i,j;
	int fdi, fdo;

	int nlin = HEIGHT;
	int ncol = WIDTH;

	if(argc<2) {
		printf("Uso: %s nome_arquivo\n",argv[0]);
		exit(0);
	}
	if((fdi=open(argv[1],O_RDONLY))==-1) {
		printf("Erro na abertura do arquivo %s\n",argv[1]);
		exit(0);
	}

	// lê arquivo de imagem, supondo tamanho 512x512
	
	// (ao menos) 2 abordagens: 
	// - ler pixels byte a byte, colocando-os em matrizes separadas
	//	- ler pixels (32bits) e depois usar máscaras e rotações de bits para o processamento.

	// Ordem de leitura dos bytes (componentes do pixel): RGBA?
	for(i=0; i < nlin; i++) {
		for(j=0; j < ncol; j++) {
			read(fdi,&mr[_idx(i, j)],1);
			read(fdi,&mg[_idx(i, j)],1);
			read(fdi,&mb[_idx(i, j)],1);
			read(fdi,&ma[_idx(i, j)],1);
		}
	}
	close(fdi);

	// Aplicar filtro (estêncil), separadamente para componentes R, G, B e A

	// Tratar: linhas 0 e n-1; colunas 0 e n-1
	#pragma omp parallel 
	{
		int i,j,x,y;
		#pragma omp for private(i, j, x, y) shared()
		for(i=1; i < nlin -1; i+=1) {
		for(j=1; j < ncol -1; j+=1) {
			mr2[_idx(i,j)] = smooth_filter(mr, i, j);
			mg2[_idx(i,j)] = smooth_filter(mg, i, j);
			mb2[_idx(i,j)] = smooth_filter(mb, i, j);
			ma2[_idx(i,j)] = smooth_filter(ma, i, j);
		}
		}
	}

	// gravar imagem resultante
	fdo=open("out.rgba",O_WRONLY|O_CREAT,0644);

	for(i=0; i < nlin; i++) {
		for(j=0; j < ncol; j++) {
			write(fdo,&mr2[_idx(i, j)], 1);
			write(fdo,&mg2[_idx(i, j)], 1);
			write(fdo,&mb2[_idx(i, j)], 1);
			write(fdo,&ma2[_idx(i, j)], 1);
		}
	}
	close(fdo);
	
	return 0;
}


