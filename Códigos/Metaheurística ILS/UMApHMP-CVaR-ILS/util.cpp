#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <time.h>
#include "util.h"


void insertionSort( double* v, int *w, int n ) //n é tamanho do vetor
{
	int i= 0;
	int j= 1;
	double aux = 0;
	int aux2 = 0;
	while (j < n)
	{
		aux = v[j];
		aux2 = w[j];
		i = j - 1;
		while ((i >= 0) && (v[i] > aux))
			{
			v[i + 1] = v[i];
			w[i + 1] = w[i];
			i = i - 1;
			}
		v[i + 1] = aux;
		w[i + 1] = aux2;
	j = j + 1;
	}
}

void insertionSort_doub( double* v, double *w, int n ) 
{
	int i= 0;
	int j= 1;
	double aux = 0;
	double aux2 = 0;
	while (j < n)
	{
		aux = v[j];
		aux2 = w[j];
		i = j - 1;
		while ((i >= 0) && (v[i] > aux))
			{
			v[i + 1] = v[i];
			w[i + 1] = w[i];
			i = i - 1;
			}
		v[i + 1] = aux;
		w[i + 1] = aux2;
	j = j + 1;
	}
}


double cvar(double* c, double* pp, double beta, int size ){
		insertionSort_doub(c, pp, size);
		double soma = 0.;
		int i = 0;
		int kb;
		while((soma < beta-0.000000000000001)&&(i<size)){
			kb=i;
			soma = soma + pp[i];
			i = i+1;
		};

		double var = c[kb];
		//printf("\n VaR igual a  %10.4f", var);
		double cvar = (soma-beta)*var; 
		for (int j = kb+1; j < size; j++){
			cvar = cvar + pp[j]*c[j];
		}
		cvar = cvar/(1-beta);
	return cvar;
}

void atualiza_vetor(int *s_star, int *s, int n)
{
   for (int j=0; j < n; j++) s_star[j] = s[j];
}


void embaralha_vetor(int *vetor, int n)
{
  int aux, j1, j2;
  srand((unsigned) time(NULL));
  for (int i=0; i < n; i++){
    j1 = rand() % (n);
    j2 = rand() % (n);
    while (j1 == j2) j2 = rand() % (n);
    aux = vetor[j1];
    vetor[j1] = vetor[j2];
    vetor[j2] = aux;
  }
}

double **cria_matriz_double(int nlinhas, int ncolunas)
{
  register int i;
  double **matriz;

  matriz = (double **) malloc(nlinhas*sizeof(double *));
  if (!matriz) {
        printf("Falta memoria para alocar a matriz de ponteiros\n");
        exit(1);
  }
  for (i=0; i < nlinhas; i++) {
    matriz[i] = (double *) malloc(ncolunas*sizeof(double));
    if (!matriz[i]){
      printf("Falta memoria para alocar a matriz de ponteiros.\n");
      exit(1);
    }
  }
  return matriz;
}

/* cria memoria para um vetor de tam posicoes */
int *cria_vetor(int tam)
{
  int *vetor;

  vetor = (int *) malloc(tam*sizeof(int));
  if (!vetor){
  	printf("Falta memoria para alocar o vetor de ponteiros");
    exit(1);
  }
  return vetor;
}



void imprime_vetor(int *s, int n)
{
    for (int j=0; j < n; j++) 
	   printf("s[%2d]=%d \n",j,s[j]);
}


void inicializa_vetor(int *vetor, int tam)
{
    for (int j=0; j<tam; j++) vetor[j] = 0;
}


void inicializa_vetor_int(int *vetor, int tam)
{
    for (int j=0; j<tam; j++) vetor[j] = j;
}

void libera_vetor(int *vetor)
{
  free(vetor);
}

void libera_matriz_double(double **matriz, int nlinhas)
{
  register int i;

  for (i=nlinhas-1; i >= 0; i--)
    free((double *) matriz[i]);
  free((double *) matriz);
}

void embaralha_hub_e_naohub(int *vetor, int n, int p)
{
  int *haux = cria_vetor(p);
  int *nhaux = cria_vetor(n-p);
  for (int i = 0; i < p; i++){
		haux[i]=int(vetor[i]);
  }
  embaralha_vetor(haux, p);
  for (int i = 0; i < n-p; i++){
		nhaux[i]=int(vetor[p+i]);
  }
  for (int i = 0; i < p; i++){
		vetor[i]=haux[i];
  }
  embaralha_vetor(nhaux, n-p);
  for (int i = p; i < n; i++){
		vetor[i]=nhaux[i-p];
  }
  libera_vetor(haux);
  libera_vetor(nhaux);
  }

