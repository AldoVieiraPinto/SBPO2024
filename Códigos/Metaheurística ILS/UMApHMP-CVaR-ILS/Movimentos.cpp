#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "Movimentos.h"
#include "util.h"


void movimento_troca_hub_naohub(int i, int j, int *s){
	int aux;
	aux = s[i];
	s[i] = s[j];
	s[j] = aux;
}



void movimento_ktrocas_hub_naohub(int p, int n, int *s, int k) {
	int aux;
	embaralha_hub_e_naohub(s, n, p);
  	for (int i = 0; i<k; i++){
		aux = s[i];
		s[i] = s[i+p];
		s[i+p] = aux;
    }    
}

