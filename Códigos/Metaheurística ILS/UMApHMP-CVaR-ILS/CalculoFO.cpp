#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <time.h>
#include "util.h"
#include "Cenario.h"
#include "CalculoFO.h"


#define inf 10e10 /// Parametro usado para representar um número suficientemente grande


//floyd

double floyd2hubs(int n, int p, double alpha, int* s, int w, const Cenario cen[], int size){
	double** d = cria_matriz_double(n, n);
	double** c = cria_matriz_double(n, n);
	double** e = cria_matriz_double(n, n);
	for(int i=0 ; i < p ; i++){
		for (int j=0; j<n; j++){
			if(j<p){
				c[s[i]][s[j]]=cen[w].custo[s[i]][s[j]];
			} 
			else{
				c[s[i]][s[j]]=cen[w].custo[s[i]][s[j]];
			}
		}
	}
	for(int i=p ; i < n ; i++){
		for (int j=p; j<n; j++){
			if(s[i]==s[j]){
				c[s[i]][s[j]]=0.;
				}else{
					c[s[i]][s[j]]=inf;
				}
			}
		for (int j=0; j<p; j++){
			c[s[i]][s[j]]=cen[w].custo[s[i]][s[j]];
		}	
	}
 	for(int k=0 ; k < p ; k++){
		for (int l=0; l<n; l++){
			d[s[k]][s[l]]=inf;
			for (int m=0; m<p; m++){
				if(d[s[k]][s[l]] > alpha*c[s[k]][s[m]]+c[s[m]][s[l]]){
					d[s[k]][s[l]] = alpha*c[s[k]][s[m]]+c[s[m]][s[l]];
				}
			}
		}
	}
	for(int i=0 ; i < n ; i++){
		for (int j=0; j<n; j++){
			e[s[i]][s[j]]=inf;
			for (int k=0; k<p; k++){
				if(e[s[i]][s[j]] > c[s[i]][s[k]]+d[s[k]][s[j]]){
					e[s[i]][s[j]] = c[s[i]][s[k]]+d[s[k]][s[j]];
				}
			}
		}
	}
	double soma=0;
	for(int i=0 ; i < n ; i++){
		for (int j=0; j<n; j++){
			soma = soma + e[s[i]][s[j]] * (cen[w].fluxo[s[i]][s[j]]);
		}
	}
	libera_matriz_double(c, n);
	libera_matriz_double(d, n);
	libera_matriz_double(e, n);
	return soma;
}

double FO_cvar(int n, int p, double alpha, double beta, int* s, const Cenario cen[], int size){
	double custo_cen[size];
	double pp[size];
	for (int t = 0; t<size; t++){
		custo_cen[t]=floyd2hubs(n, p, alpha, s, t, cen, size);
		pp[t]=cen[t].prob;
	}
	double result = cvar(custo_cen, pp, beta, size);
	return result;
}


