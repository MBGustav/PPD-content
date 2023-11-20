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
#define WIDTH  512
#define HEIGHT 512

#define smooth_filter(A,i, j) (( A[i-1][j-1] + A[i-1][j] + A[i-1][j+1] +      \
	      		                A[i]  [j-1] + A[i]  [j] + A[i]  [j+1] +       \
      			                A[i+1][j-1] + A[i+1][j] + A[i+1][j+1] ) >> 4);

int mr [WIDTH][HEIGHT];
int mg [WIDTH][HEIGHT];
int mb [WIDTH][HEIGHT];
int ma [WIDTH][HEIGHT];
int mr2 [WIDTH][HEIGHT];
int mg2 [WIDTH][HEIGHT];
int mb2 [WIDTH][HEIGHT];
int ma2 [WIDTH][HEIGHT];

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
			read(fdi,&mr[i][j],1);
			read(fdi,&mg[i][j],1);
			read(fdi,&mb[i][j],1);
			read(fdi,&ma[i][j],1);
		}
	}
	close(fdi);

	// Aplicar filtro (estêncil), separadamente para componentes R, G, B e A

	// Tratar: linhas 0 e n-1; colunas 0 e n-1
	#pragma omp parallel 
	{
		int i,j,x,y;
		#pragma omp for private(i,j,x,y)
		for(i=1; i < nlin -1; i+=TILE_SIZE) {
		for(j=1; j < ncol -1; j+=TILE_SIZE) {
			for(x = i ; x < i + TILE_SIZE; x++)
			for(y = j ; y < j + TILE_SIZE; y++)
			{
				mr2[x][y] = smooth_filter(mr, x, y);
				mg2[x][y] = smooth_filter(mg, x, y);
				mb2[x][y] = smooth_filter(mb, x, y);
				ma2[x][y] = smooth_filter(ma, x, y);
			}
		}
		}
	}
	// gravar imagem resultante
	fdo=open("out.rgba",O_WRONLY|O_CREAT,0644);

	for(i=0; i < nlin; i++) {
		for(j=0; j < ncol; j++) {
			write(fdo,&mr2[i][j], 1);
			write(fdo,&mg2[i][j], 1);
			write(fdo,&mb2[i][j], 1);
			write(fdo,&ma2[i][j], 1);
		}
	}
	close(fdo);
	
	return 0;
}


