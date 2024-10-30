#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include<iostream>
#include<conio.h>
#include <limits.h>
#include <time.h>
#include "util.h"
#include "Movimentos.h"
#include "Cenario.h"
#include "SolucaoInicial.h"


void construcao_gulosa (int n, int *s, const Cenario cen[], int size){
  	double v[n];
	for (int k = 0; k < n; k++){
		v[k]=0;
		s[k]=k;
	}
  	for (int m = 0; m < size; m++){
		for (int i = 0; i<n; i++){
	    	for (int j = 0; j<n; j++){
	    		v[i]=v[i]+(cen[m].custo[i][j])*(cen[m].fluxo[i][j]);
	    	}
	  	}
	}
	insertionSort(v, s, n);	
}
