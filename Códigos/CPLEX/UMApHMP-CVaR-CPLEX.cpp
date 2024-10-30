/*********************************************
 * Concert Model
 * Autor: Aldo
 * Data de criação: 10-04-2023
 * Problema de Localização de hubs p-mediana - CVaR (UMApHMP-CVaR)
 *********************************************/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <ilcplex/ilocplex.h>
#include  <chrono>

using namespace std;
ILOSTLBEGIN



int main(int argc, char* argv[]) {
	try {
		/**============================
		 *  Leitura dos dados
		 *=============================== */
		ifstream arq(argv[1]);
		if (!arq.is_open()) {
			cout << "Error openning file: " << argv[1] << endl;
			arq.close();
			exit(EXIT_FAILURE);
		}
		// Entrada via linha de comando: UMApHMP-CVaR-CPLEX.exe instancia.txt n numcenarios p alpha beta
		int n; // Quantidade de nós
		int p; // Quantidade de hubs a serem ativados
		int numcen;// Número de Cenários
		double alpha; // Fator de desconto no custo de transporte em um link entre hubs
		double beta; // nível de risco CVaR
                //Coletar dados via linha de comando
		if (argc >= 2) 
			n = atoi(argv[2]);
		else n = 25;
		numcen = atoi(argv[3]);
		p = atoi(argv[4]);
		alpha = atof(argv[5]);
		beta = atof(argv[6]);
		double m = numcen;
		double prob = 1 / m; // probabilidade de cada cenário (uniforme)
		double beta2 = 1 / (1 - beta);
		std::vector<std::vector<std::vector<double>>> custo(numcen, std::vector<std::vector<double>>(n, std::vector<double>(n, 0)));
		std::vector<std::vector<std::vector<double>>> fluxo(numcen, std::vector<std::vector<double>>(n, std::vector<double>(n, 0)));
		vector<vector<double > > o(numcen, vector<double>(n));
		for (int m = 0; m < numcen; m++) {
			for (int i = 0; i < n; i++) {
				for (int j = 0; j < n; j++) {
					arq >> custo[m][i][j];
				}
			}
			for (int i = 0; i < n; i++) {
				for (int j = 0; j < n; j++) {
					arq >> fluxo[m][i][j];
				}
			}
		}
		for (int m = 0; m < numcen; m++) {
			for (int i = 0; i < n; i++) { //Inicializa vetor o de demandas
				o[m][i] = 0.;
				//Cálculo do vetor o: Matriz com a soma de demanadas a partir de cada nó
				for (int j = 0; j < n; j++) {
					o[m][i] += fluxo[m][i][j];
				}
			}
		}
		/**============================
		*  Declaração do modelo
		*=============================== */

		IloEnv env;
		IloModel mod(env);
		IloCplex cplex(mod);


		IloNumVarArray h(env, n, 0, 1, ILOBOOL); // h[k] variável binária que vale 1 se o nó k é um hub e 0 se não é ativado

		IloArray<IloArray<IloNumVarArray>> z(env, numcen); // z[m][i][k] representa Fluxo enviado do nó i para o hub k no cenário m
		for (int m = 0; m < numcen; m++) {
			z[m] = IloArray<IloNumVarArray>(env, n);
			for (int i = 0; i < n; i++) {
				z[m][i] = IloNumVarArray(env, n, 0, IloInfinity, ILOFLOAT);
			}
		}
		IloArray<IloArray<IloArray<IloNumVarArray>>> y(env, numcen); // y[m][i][k][l] representa o fluxo originado do nó i e roteado por meio dos hubs k e l no cenário m
		for (int m = 0; m < numcen; m++) {
			y[m] = IloArray<IloArray<IloNumVarArray>>(env, n);
			for (int i = 0; i < n; i++) {
				y[m][i] = IloArray<IloNumVarArray>(env, n);
				for (int k = 0; k < n; k++) {
					y[m][i][k] = IloNumVarArray(env, n, 0, IloInfinity, ILOFLOAT);
				}
			}
		}
		IloArray<IloArray<IloArray<IloNumVarArray>>> x(env, numcen); // x[m][i][l][j] representa o fluxo originado do nó i e enviado do hub l para o nó j, no cenário m
		for (int m = 0; m < numcen; m++) {
			x[m] = IloArray<IloArray<IloNumVarArray>>(env, n);
			for (int i = 0; i < n; i++) {
				x[m][i] = IloArray<IloNumVarArray>(env, n);
				for (int l = 0; l < n; l++) {
					x[m][i][l] = IloNumVarArray(env, n, 0, IloInfinity, ILOFLOAT);
				}
			}
		}
		IloNumVarArray custocenario(env, numcen, 0, IloInfinity, ILOFLOAT); // variável custo[m] é o custo total no cenário m

		IloNumVar eta(env, -IloInfinity, IloInfinity, ILOFLOAT);

		IloNumVarArray v(env, numcen, 0, IloInfinity, ILOFLOAT); // variável v[m] usada para linearizar custo[m] no CVaR


		// ====================================Formulação=============================================================== 
		// minimize ( (sum (m in 1..n) beta2*prob*v[m] ) + eta )
		// ===============================================================================================================

		IloExpr expfo(env);
		for (int m = 0; m < numcen; m++) {
			expfo += beta2 * prob * v[m];
		}
		expfo += eta;
		IloAdd(mod, IloMinimize(env, expfo));
		expfo.end();

		// Restrições

		//===========================================================================
		// forall (m in 1..numcen) 
		// custocenario[m] = sum(i in 1 .. n, k in 1..n) cen[m].custo[i][k] * z[m][i][k]+
		//                 +  sum(i in 1 .. n, k in 1..n, l in 1..n) alpha * cen[m].custo[k][l] * y[m][i][k][l]
		//                 +  sum(i in 1 .. n, l in 1..n, j in 1..n) cen[m].custo[l][j] * x[m][i][l][j]
		//===========================================================================
		for (int m = 0; m < numcen; m++) {
			IloExpr r1(env);
			for (int i = 0; i < n; i++) {
				for (int k = 0; k < n; k++) {
					r1 += custo[m][i][k] * z[m][i][k];
				}
			}
			for (int i = 0; i < n; i++) {
				for (int k = 0; k < n; k++) {
					for (int l = 0; l < n; l++) {
						r1 += alpha * custo[m][k][l] * y[m][i][k][l];
					}
				}
			}
			for (int i = 0; i < n; i++) {
				for (int j = 0; j < n; j++) {
					for (int l = 0; l < n; l++) {
						r1 += custo[m][l][j] * x[m][i][l][j];
					}
				}
			}
			mod.add(r1 == custocenario[m]);
			r1.end();
		}

		//===========================================================================
		// sum(k in 1..n) h[i] == p 
		//===========================================================================
		IloExpr r2(env);
		for (int i = 0; i < n; i++) {
			r2 += h[i];
		}
		mod.add(r2 == p);
		r2.end();

		//===========================================================================
		// forall(m in 1..numcen, i in 1..n)  sum(k in 1..n) z[m][i][k] = o[m][i]   
		//===========================================================================
		for (int m = 0; m < numcen; m++) {
			for (int i = 0; i < n; i++) {
				IloExpr r3(env);
				for (int k = 0; k < n; k++) {
					r3 += z[m][i][k];
				}
				mod.add(r3 == o[m][i]);
				r3.end();
			}
		}

		//===========================================================================
		// forall (m in 1..numcen, i in 1..n, j in 1..n)  sum(l in 1..n) x[m][i][j][l] == cen[m].fluxo[i][j]  
		//===========================================================================
		for (int m = 0; m < numcen; m++) {
			for (int i = 0; i < n; i++) {
				for (int j = 0; j < n; j++) {
					IloExpr r4(env);
					for (int l = 0; l < n; l++) {
						r4 += x[m][i][l][j];
					}
					mod.add(r4 == fluxo[m][i][j]);
					r4.end();
				}
			}
		}
		//======================================================================================================================================================================================
		// forall(m in 1.numcen, i in 1..n, k in 1..n) 
		// sum(l in 1..n:l!=k) y[m][i][k][l]+sum(j in 1..n)x[m][i][k][j] == sum(l in 1..n:l!=k) y[m][i][l][k]+z[m][i][k]
		// aqui codificado como
		// forall(m in 1..numcen, i in 1..n, k in 1..n) 
		// (sum(j in 1..n) x[m][i][k][j]) + (sum(l in 1..n:l!=k) (y[m][i][k][l]-y[m][i][l][k]))-z[m][i][k] == 0
		//======================================================================================================================================================================================
		for (int m = 0; m < numcen; m++) {
			for (int i = 0; i < n; i++) {
				for (int k = 0; k < n; k++) {
					IloExpr r5(env);
					for (int j = 0; j < n; j++) {
						r5 += x[m][i][k][j];
					}
					for (int l = 0; l < n; l++) {
						if (l != k) {
							r5 += y[m][i][k][l] - y[m][i][l][k];
						}
					}
					r5 -= z[m][i][k];
					mod.add(r5 == 0);
					r5.end();
				}
			}
		}
		//===========================================================================
		// forall (m in 1..numcen, i in 1..n, k in 1..n) z[m][i][k] <= o[m][i]*h[k]
		//===========================================================================
		for (int m = 0; m < numcen; m++) {
			for (int i = 0; i < n; i++) {
				for (int k = 0; k < n; k++) {
					mod.add(z[m][i][k] <= o[m][i] * h[k]);
				}
			}
		}
		//===========================================================================
		// forall (m in 1..numcen, i in 1..n, j in 1..n, l in 1..n) x[m][i][l][j] <= cen[m].fluxo[i][j]*h[l]
		//===========================================================================
		for (int m = 0; m < numcen; m++) {
			for (int i = 0; i < n; i++) {
				for (int j = 0; j < n; j++) {
					for (int l = 0; l < n; l++) {
						mod.add(x[m][i][l][j] <= fluxo[m][i][j] * h[l]);
					}
				}
			}
		}

		for (int m = 0; m < numcen; m++) {
			for (int i = 0; i < n; i++) {
				for (int k = 0; k < n; k++) {
					IloExpr r6(env);
					for (int l = 0; l < n; l++) {
						//if (l != k) {
						r6 += y[m][i][k][l];
						//}
					}
					mod.add(r6 <= z[m][i][k]);
					//mod.add(r6 <= o[m][i]*h[k]);
					r6.end();
				}
			}
		}

		
		
		//===========================================================================
		// forall (m in 1..numcen) v[m] >= custocenario[m]-eta
		//===========================================================================
		for (int m = 0; m < numcen; m++) {
			mod.add(v[m] >= custocenario[m] - eta);
		}

		/// ==========================
		/// configurações do Cplex
		/// ==========================
		cplex.setParam(IloCplex::EpGap, 0.0000001); // Definindo uma tolerancia
		cplex.setParam(IloCplex::TiLim, 129600); // Tempo limite de resolução
		//cplex.setWarning(env.getNullStream()); // Eliminar warnings
		//cplex.setOut(mono->env.getNullStream()); // Eliminar os logs do solver
		cplex.setParam(IloCplex::Threads, 1); // Definir a quantidade de núleos
		//cplex.setParam(IloCplex::Param::Benders::Strategy, 3); // Ativar Benders do solver


		 ///==============================
		 /// Resolvendo o problema
		 ///==============================

		IloTimer crono(env);// Variável para coletar o tempo
		double lb = 0;
		double ub = 10e-10;
		double gap;


		crono.start();
		cplex.solve();
		crono.stop();


		if (cplex.getStatus() == IloAlgorithm::Optimal) {
			lb = cplex.getObjValue();
			ub = cplex.getObjValue();
			gap = 0.0;
		}

		///=====================================
		/// Salvando os resultados
		///=====================================
		/// 
		/// 
		cout <<"Instancia:"<< argv[1] << endl;
		cout <<"O valor de n:"<< n << endl;
		cout << "O valor de p:" << p << endl;
		cout << "O valor de alpha:" << alpha << endl;
		cout << "O valor de beta:" << beta << endl;


		printf("\n Valor de Funcao Objetivo %10.4f", cplex.getObjValue());

		cout << endl;

		printf("\n Tempo de CPU: " "%f\t \n", (double)crono.getTime());
		cout << endl;

		printf("Hubs Instalados:");
		for (int j = 0; j < n; j++) {
			if (cplex.getValue(h[j]) >= 0.1) {
				printf(" %d\t ", j + 1);
			}
		}
		cout << endl;

		/**=====================================
		*  Apresenta a configuração final // Ativar se desejar salvar em um arquivo
		* ====================================*/
		//printf("\n Valor funcao objetivo: " "%f\t \n", (double)cplex.getObjValue());

		//FILE *re;
		//re = fopen("Resultados.txt","w+");
		//fprintf(re, "\n Informacoes Gerais: " "%s\t%d\t%d\t%10.2f\n",  argv[1], n, p, alpha);
		//fprintf(re, "\n Valor funcao objetivo: " "%f\t \n", (double) cplex.getObjValue ());
		//fprintf(re, "\n Tempo de CPU: " "%f\t \n", (double) crono.getTime());
		//fprintf(re, "======================================================================");
	}
	catch (IloException& ex) {
		cerr << "Error: " << ex << endl;
	}
	return 0;
}
