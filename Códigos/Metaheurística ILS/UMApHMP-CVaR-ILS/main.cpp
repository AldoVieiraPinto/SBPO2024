#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fstream>
#include <iostream>
#include <conio.h>
#include <limits.h>
#include <vector>
#include <time.h>
#include "util.h"
#include "Cenario.h"
#include "CalculofO.h"
#include "Movimentos.h"
#include "BuscaLocal.h"
#include "ILS.h"
#include "SolucaoInicial.h"

using namespace std;

#define inf 10e10 // Parametro usado para representar um numero suficientemente grande


int main(int argc, char* argv[]) {
	try {
		/**============================
		 *  Leitura dos dados
		 *=============================== */
		// Entrada via linha de comando: UMApHMP-CVaR-ILS.exe instancia.txt n numcenarios p alpha beta
		ifstream arq(argv[1]);
		if (!arq.is_open()) {
			cout << "Error openning file: " << argv[1] << endl;
			arq.close();
			exit(EXIT_FAILURE);
		}
		int n; // Quantidade de nós
		int p; // Quantidade de hubs a serem ativados
		int numcen; // Número de Cenários
		double alpha; // Fator de desconto entre hubs
		double beta; // Fator de risco do CVaR
		if (argc >= 2) //Coletar dados via linha de comando
			n = atoi(argv[2]);
		else n = 25;
		numcen = atoi(argv[3]);
		p = atoi(argv[4]);
		alpha = atof(argv[5]);
		beta = atof(argv[6]);
		double m = numcen;
		int ILSmax = 9;	// número máximo de iterações sem melhora
		Cenario cen[numcen];  // struct Cenario armazena a probabilidade, a matriz de custos e a matriz de fluxos associados ao cenário
		clock_t inicio_CPU, fim_CPU; 
		for (int w = 0; w < numcen; w++){
			cen[w].prob=1/m;
			cen[w].fluxo = cria_matriz_double(n, n);
			cen[w].custo = cria_matriz_double(n, n);
		}
		// leitura da matriz de custos e da matriz de fluxos de cada cenário
		for (int w = 0; w < numcen; w++){
			for (int i = 0; i < n; i++){
				for (int j = 0; j < n; j++){
					arq >> cen[w].custo[i][j];
				}
			}
			for (int i = 0; i < n; i++){
				for (int j = 0; j < n; j++){
					arq >> cen[w].fluxo[i][j];
				}
			}
		}
		double fotimo;
		double tempo;
	        inicio_CPU = clock();
		int *s;
		s = cria_vetor(n);
		fotimo = ILS(n, p, alpha, beta, s, cen, numcen, ILSmax);
		fim_CPU = clock();
		tempo=double(fim_CPU - inicio_CPU)/(CLOCKS_PER_SEC);
		printf("\nHubs Ativados:");
		for (int i = 0; i < p; i++){
			printf("\t %d", s[i]+1);
		}
		printf("\n Valor de Funcao ILS = ... \n %10.4f", fotimo);
		printf("\n Tempo de Execucao = ...\n %10.4f", tempo);
		
	}
	catch(int erro) {
		cerr << "Error: " << endl;
	}
	return 0;
}
