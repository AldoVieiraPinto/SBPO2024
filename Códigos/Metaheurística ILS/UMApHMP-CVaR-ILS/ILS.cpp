#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <time.h>
#include "Cenario.h"
#include "util.h"
#include "Movimentos.h"
#include "SolucaoInicial.h"
#include "BuscaLocal.h"
#include "CalculoFO.h"
#include "ILS.h"

double ILS(int n, int p, double alpha, double beta, int *s, const Cenario cen[], int size, int ILSmax)
{
	int i, j, nivel, iter, MelhorIter;
	int *s_2l, *s_star;
	double fo, fo_2l, fo_star;
	s_star = cria_vetor(n);
	inicializa_vetor_int(s_star,n);
	construcao_gulosa(n, s_star, cen, size);
	fo_star = FO_cvar(n, p, alpha, beta, s_star, cen, size);
	atualiza_vetor(s,s_star,n);
	s_2l = cria_vetor(n);
	fo =  best_improvement(n, p, alpha, beta, s_star, cen, size);
	atualiza_vetor(s,s_star,n);
	iter = MelhorIter = 0;
	nivel = 1;
	while (iter - MelhorIter < ILSmax && nivel < 0.7*p){
	    iter++;
	    // s_2l <- s
	    atualiza_vetor(s_2l,s,n);
	    int ntrocasmax = nivel + 1;
		fo_2l = fo;
		movimento_ktrocas_hub_naohub(p, n, s_2l,ntrocasmax); //perturbacao
	   	fo_2l = best_improvement(n, p, alpha, beta, s_2l, cen, size);
	    if (fo_2l < fo){
	        fo = fo_2l;
	        atualiza_vetor(s,s_2l,n);
			MelhorIter = iter;
	    }
		nivel++;
	} // fim while vezesnivel
	libera_vetor(s_star);
	libera_vetor(s_2l);
	return fo;
}



