#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <ilcplex/ilocplex.h> 
#include <random>
#include <chrono>

using namespace std;
ILOSTLBEGIN

int main(int argc, char* argv[]) {
	try {
		//=======================================================================================
		// COLETAR DADOS (Entradas via linha de comando: nomedoarquivo numerodecenarios)
		// ========================================================================================
		ifstream arq(argv[1]);
		if (!arq.is_open()) {
			cout << "Error openning file: " << argv[1] << endl;
			arq.close();
			exit(EXIT_FAILURE);
		}
		int numcen; //número de cenários
		if (argc >= 2) //Coletar números de hubs a serem ativados
			numcen = atoi(argv[2]);
		int n; // número de nós lido da instância
		arq >> n;
		vector<vector<double > > w(n, vector<double>(n)); // declara um vetor de vetores para representar uma matriz com n linhas e n colunas - Quantidade de demanda a ser enviada entre os nós i e j
		vector<vector<double > > c(n, vector<double>(n)); // Custos por enviar uma unidade de demanda entre os nós i e j
		vector<vector<double > > watual(n, vector<double>(n)); // declara um vetor de vetores para representar uma matriz com n linhas e n colunas - Quantidade de demanda a ser enviada entre os nós i e j no cenário atual
		vector<vector<double > > catual(n, vector<double>(n)); // Custos por enviar uma unidade de demanda entre os nós i e j no cenário atual

		for (int i = 0; i < n; i++) {   //Coletar fluxos 
			for (int j = 0; j < n; j++) {
				arq >> w[i][j];
			}
		}

		for (int i = 0; i < n; i++) {   //Coletar custos
			for (int j = 0; j < n; j++) {
				arq >> c[i][j];
			}
		}

		std::random_device rd;
		std::mt19937 gen(rd());

		// Define o intervalo desejado - High
		double min = 0.5;
		double max = 1.5;
		FILE* re;
		re = fopen("CAB-High100.txt", "w+"); //exemplo de saída para CAB com 100 cenários

		
		std::uniform_real_distribution<double> dis(min, max);


		for (int m = 0; m < numcen; m++) {
			for (int i = 0; i < n; i++) { //Cálculo do vetor o: Matriz com a soma de demanadas a partir de cada nó
				for (int j = 0; j < n; j++) {
					watual[i][j] = dis(gen) * w[i][j];
				}
			}

			for (int i = 0; i < n; i++) { //Cálculo do vetor o: Matriz com a soma de demanadas a partir de cada nó
				for (int j = 0; j < n; j++) {
					catual[i][j] = dis(gen) * c[i][j];
				}
			}


			for (int i = 0; i < 25; i++) { //Cálculo do vetor o: Matriz com a soma de demanadas a partir de cada nó
				for (int j = 0; j < 25; j++) {
					fprintf(re, "%10.5f \t ", catual[i][j]);
				}
				fprintf(re, "\n");
			}
			for (int i = 0; i < 25; i++) { //Cálculo do vetor o: Matriz com a soma de demanadas a partir de cada nó
				for (int j = 0; j < 25; j++) {
					fprintf(re, "%10.5f \t ", watual[i][j]);
				}
				fprintf(re, "\n");
			}
		}
	}
	catch (IloException& ex) {
		cerr << "Error: " << ex << endl;
	}
	return 0;
}