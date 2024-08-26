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


double best_neighbor(int n, int p, double alpha, double beta, int *s, const Cenario cen[], int size, double fo, int *melhor_i, int *melhor_j)
{
  int aux;
  double fo_melhor_viz = fo;

  double fo_viz;
  for(int i=0 ; i < p ; i++){
    for(int j=p ; j < n ; j++) {
      
      movimento_troca_hub_naohub(i,j,s);

      fo_viz = FO_cvar(n, p, alpha, beta, s, cen, size);
      // Armazenar o melhor movimento (melhor troca)
      if(fo_viz < fo_melhor_viz){
        *melhor_i = i;
        *melhor_j = j;
        fo_melhor_viz = fo_viz;
      }

      // Desfaz o movimento
      movimento_troca_hub_naohub(i,j,s);
    }
  }
  return fo_melhor_viz;

}//melhor_vizinho


double best_improvement(int n, int p, double alpha, double beta, int *s, const Cenario cen[], int size)
{
  int aux, melhor_i, melhor_j;
  double fo_viz, fo;
  bool melhorou;

  fo = fo_viz = FO_cvar(n, p, alpha, beta, s, cen, size);

  do{
     melhorou = false;
     
     fo_viz = best_neighbor(n, p, alpha, beta, s, cen, size, fo, &melhor_i, &melhor_j);
     if (fo_viz < fo ){
	 	//Efetiva o movimento para o melhor vizinho      s <- s'
		  movimento_troca_hub_naohub(melhor_j,melhor_i,s);
          
          fo = fo_viz;
          melhorou = true;
     }
  } while (melhorou == true);
  return fo;
}



