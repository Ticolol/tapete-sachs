#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>


typedef struct{
	int rotacao;
	int invertida;	
	int espelhada;
}Peca;

typedef struct{
	Peca **pecas;
	int **tapete;
}Solucao;




float randFloat(){
	return (float)(rand() % 10000)/10000;
}

// ===== OPERACOES BASICAS COM FEIXES ============================================
//Inverte pecas
void InvertBits(int *feixe){
	int i;
	for(i=0; i<32;i++){
		if(feixe[i]==1){
			feixe[i]=0;
		}else{
			feixe[i]=1;
		}
	}
}
//Espelha pecas
void MirrorBits(int *feixe){
	int *oldFeixe;
	int i;
	//Copy feixe into oldFeixe
	oldFeixe = (int *)malloc(32 * sizeof(int));	
	for(i=0; i<32; i++)
		oldFeixe[i] = feixe[i];
	//printf("Hey\n");
	//Change values
	for(i=0; i<16; i++){
		//printf("%d\n", oldFeixe[16-i]);
		feixe[i] = oldFeixe[16-i];
	}
	for(i=17; i<32; i++){
		//printf("%d\n", oldFeixe[48-i]);
		feixe[i] = oldFeixe[48-i];
	}
	free(oldFeixe);
}
//Rotaciona pecas
void RotateBits(int *feixe, int rot){
	int *oldFeixe;
	int i;
	//Copy feixe into oldFeixe
	oldFeixe = (int *)malloc(32 * sizeof(int));	
	for(i=0; i<32; i++)
		oldFeixe[i] = feixe[i];
	//Rotate Bits
	for(i=0; i<32;i++){
		feixe[i] = oldFeixe[(i+rot)%32];
	}
	free(oldFeixe);
}

// ===== METODOS PARA PECAS =============================================
//Inicializa peca
Peca* CreatePeca(){//int feixe[32]){
	Peca *peca = (Peca *)malloc(sizeof(Peca));
	//peca->feixe = (int *)malloc(sizeof(int));
	//for(i=0; i<32; i++)
	//	peca->feixe[i] = feixe[i];
	//peca->feixe = ref;
	peca->rotacao = 0;//{0,31}
	peca->invertida = 0;//false
	peca->espelhada = 0;//false
	return peca;
}
//Printa peca
void PrintPeca(Peca *p, int *feixe){
	int i;
	printf("==> Peca:\nFeixe: ");
	for(i=0; i<32; i++)
		printf("%d", feixe[i]);
	printf("\nRotacao: %d\nInvertida: %d\nEspelhada: %d\n\n",p->rotacao,p->invertida,p->espelhada);
}
//Retorna valor real do feixe de bits
int* GetTrueFeixe(Peca *p, int *feixe){
	int *newFeixe;
	int i;
	//Valores de p.feixe para newFeixe
	newFeixe = (int *)malloc(32 * sizeof(int));
	for(i=0; i<32; i++){
		newFeixe[i] = feixe[i];
	}
	//Aplicar modificadores de feixe
	if(p->espelhada)
		MirrorBits(newFeixe);
	if(p->invertida)
		InvertBits(newFeixe);
	RotateBits(newFeixe, p->rotacao);
	//Retornar
	return newFeixe;
}
//Retorna o lado de contato side da Peca p
int** GetSide(Peca *p, int *feixe){ 
	int i;
	//Alocar vetor de lados
	int **sides = (int **)malloc(6 * sizeof(int *))	;
	for(i=0; i<6; i++){
		if(i==0 || i==3)
			sides[i]=(int *)malloc(7 * sizeof(int));	
		else
			sides[i]=(int *)malloc(6 * sizeof(int));
	}

	//Pegar feixe verdadeiro
	int* trueFeixe = GetTrueFeixe(p, feixe);

	//Pegar cada lado
	//L1
	for(i=0; i<6; i++)
		sides[1][i] = trueFeixe[5-i];
	//L0
	for(i=0; i<7; i++)
		sides[0][i] = trueFeixe[11-i];
	//L5
	for(i=0; i<6; i++)
		sides[5][i] = trueFeixe[16-i];
	//L4
	for(i=0; i<6; i++)
		sides[4][i] = trueFeixe[16+i];
	//L3
	for(i=0; i<7; i++)
		sides[3][i] = trueFeixe[21+i];
	//L2
	for(i=0; i<5; i++)
		sides[2][i] = trueFeixe[27+i];
	sides[2][5] = trueFeixe[0];
	
	free(trueFeixe);

	return sides;
}
//Desaloca lados gerados em GetSides
void FreeSides(int **v){
	int i;
	for(i=0; i<6; i++)
		free(v[i]);
	free(v);
}



//int r = rand() % 20; //Random number between 0 and 19
//float entre 0 e 1: (float)rand() % 100)/100
//===== OPERACOES COM SOLUCOES =====================================
//Gerar solucao padrao
Solucao* GenerateBlankSolucao(int **feixes, int inputSize, int matrixHeight, int matrixWidth){
	int i;
	Solucao *s;
	s = (Solucao *)malloc(sizeof(Solucao));
	//Alocar pecas
	s->pecas = (Peca **)malloc(inputSize * sizeof(Peca *));
	for(i=0; i<inputSize; i++)
		s->pecas[i] = CreatePeca();
	//Alocar tapete
	s->tapete = (int **)malloc(matrixHeight * sizeof(int *));
	for(i=0; i<matrixHeight; i++)
		s->tapete[i] = (int *)malloc(matrixWidth * sizeof(int));
	return s;
}
//Gerar nova solucao
Solucao* GenerateNewSolucao(int **feixes, int inputSize, int matrixHeight, int matrixWidth){
	int i, j, k;
	Solucao *s;
	s = (Solucao *)malloc(sizeof(Solucao));
	//Alocar pecas
	s->pecas = (Peca **)malloc(inputSize * sizeof(Peca *));
	for(i=0; i<inputSize; i++)
		s->pecas[i] = CreatePeca();
	//Alocar tapete
	s->tapete = (int **)malloc(matrixHeight * sizeof(int *));
	for(i=0; i<matrixHeight; i++)
		s->tapete[i] = (int *)malloc(matrixWidth * sizeof(int));
	
	//Randomize pecas
	for(i=0; i<inputSize; i++){
		s->pecas[i]->rotacao = rand() % 32;
		s->pecas[i]->invertida = rand() % 2;
		s->pecas[i]->espelhada = rand() % 2;
	}
	//Randomize tapete
	int usedValues[matrixHeight*matrixWidth];
	int randomNum, usedValCount, flag;
	usedValCount  = 0;
	for(i=0; i<matrixHeight; i++){
		for(j=0; j<matrixWidth; j++){
			flag=1;
			while(flag==1){
				flag=0;
				randomNum = rand() % inputSize;
				for(k=0; k<usedValCount; k++)
					if(randomNum == usedValues[k])
						flag = 1;
			}//Se sair, temos um valor randomico em [0, InputSize) nao repetido
			usedValues[usedValCount]=randomNum;
			usedValCount++;
			s->tapete[i][j] = randomNum;
		}
	}	
	return s;	
}

//Libera memória de solução
void FreeSolucao(Solucao *s, int inputSize, int matrixHeight, int matrixWidth){
	int i;
	for(i=0; i<inputSize; i++)
		free(s->pecas[i]);
	free(s->pecas);

	for(i=0; i<matrixHeight; i++)
		free(s->tapete[i]);
	free(s->tapete);

	free(s);
}

//Realiza operações de crossover
void CrossoverSolution(int **p1, int **p2, int **f1, int **f2, int w, int h){
	int i, j, k, l;

	//Obter região de crossover
	int x1=(rand() % w);
	int x2=(rand() % (w-x1))+(x1+1);
	int y1=(rand() % h);
	int y2=(rand() % (h-y1))+(y1+1);
	
	//Passar região do crossover p filhos
	for(i=x1; i<x2; i++){
		for(j=y1; j<y2; j++){
			f1[i][j] = p2[i][j];
			f2[i][j] = p1[i][j];
		}
	}

	//Ver se valores da região fora do corte precisam ser trocados
	int flag;
	for(i=0; i<w; i++)
		for(j=0; j<h; j++){			
			//Continuar apenas se não estivermos dentro da região de corte
			if((i<x1||i>=x2) || (j<y1||j>=y2)){

				//===F1
				flag=1;
				f1[i][j]=p1[i][j];
				while(flag){
					flag=0;
					//Checar se valor atual está no corte			
					for(k=x1; k<x2 && !flag; k++)
						for(l=y1; l<y2 && !flag; l++)
							if(f1[i][j]==f1[k][l]){
								//Trocar
								f1[i][j]=p1[k][l];
								flag = 1;
							}
				}

				//===F2
				flag=1;
				f2[i][j]=p2[i][j];
				while(flag){
					flag=0;
					//Checar se valor atual está no corte			
					for(k=x1; k<x2 && !flag; k++)
						for(l=y1; l<y2 && !flag; l++)
							if(f2[i][j]==f2[k][l]){
								//Trocar
								f2[i][j]=p2[k][l];
								flag = 1;
							}
				}
			}
		}
}
//Muta a solucao s
void MutateSolution(Solucao *s, int inputSize, int matrixHeight, int matrixWidth, 
	float pecaParamMutRate, float mutatePecasMutRate, float mutateTapeteMutRate){
	int i, i1, i2, j1, j2, v;
	int qtdPecas = (rand() % (inputSize+1));
	int qtdMatriz = (rand() % (matrixHeight * matrixWidth));

	float k = randFloat();
	if(k <= mutatePecasMutRate){
		//Mutar pecas
		for(i=0; i<qtdPecas; i++){
			//Escolhe peça
			v = rand() % inputSize;
			//Aplica probabilidade na randomização dos parâmetros
			if(randFloat()<=pecaParamMutRate)
				s->pecas[v]->rotacao = rand() % 32;
			if(randFloat()<=pecaParamMutRate)
				s->pecas[v]->invertida = rand() % 2;
			if(randFloat()<=pecaParamMutRate)
				s->pecas[v]->espelhada = rand() % 2;
		}
	}
	if(k >= mutateTapeteMutRate){
		//Mutar matriz
		for(i=0; i<qtdMatriz; i++){
			i1 = rand() % matrixHeight;
			i2 = rand() % matrixHeight;
			j1 = rand() % matrixWidth;
			j2 = rand() % matrixWidth;
			v = s->tapete[i1][j1];
			s->tapete[i1][j1] = s->tapete[i2][j2];
			s->tapete[i2][j2] = v;
		}
	}
}
//Retorna index do tapete, levando em conta a topologia toroidal do tapete
int GetPecaIndex(int ip, int jp, Solucao *s, int matrixHeight, int matrixWidth){
	return s->tapete[(ip+matrixHeight) % matrixHeight][(jp+matrixWidth) % matrixWidth];
}
//Obter fitness entre a peca fornecida e a posica
int GetSideFitness(int ip, int jp, int adjPos, Solucao *s, int **feixes, int matrixHeight, int matrixWidth){
	//int **GetSide(Peca *p, int *feixe)
	int fitness = 0, arraySize=0, pecaIndex, i, adjS;	
	int **atuSides, **adjSides;
	
	//Obter side de minha peca
	//Minha peca(index): s->tapete[ip][jp]
	pecaIndex = GetPecaIndex(ip, jp, s, matrixHeight, matrixWidth);
	atuSides = GetSide(s->pecas[pecaIndex], feixes[pecaIndex]);
	
	if(adjPos == 1){
		if(jp % 2 == 1)//Se for impar, i-=1
			ip-=1;
		//Peca adj(index): s->tapete[ip+1][jp+1]	
		pecaIndex = GetPecaIndex(ip, jp+1, s, matrixHeight, matrixWidth);	
		adjSides = GetSide(s->pecas[pecaIndex], feixes[pecaIndex]);
		arraySize = 6;
		adjS = adjPos+3;
	}else if(adjPos == 2){
		if(jp % 2 == 1)//Se for impar, i-=1
			ip-=1;	
		//Peca adj(index): s->tapete[ip+1][jp+1]		
		pecaIndex = GetPecaIndex(ip+1, jp+1, s, matrixHeight, matrixWidth);	
		adjSides = GetSide(s->pecas[pecaIndex], feixes[pecaIndex]);
		arraySize = 6;
		adjS = adjPos+3;
	}else if(adjPos == 3){
		pecaIndex = GetPecaIndex(ip+1, jp, s, matrixHeight, matrixWidth);	
		adjSides = GetSide(s->pecas[pecaIndex], feixes[pecaIndex]);
		arraySize = 7;
		adjS = 0;
	}
	//Se for uma adjPos valida, comparar side do atu e adj e somar iguais
	if(arraySize > 0){
		for(i=0; i<arraySize; i++){
			if(atuSides[adjPos][i]==adjSides[adjS][i])
				fitness+=1;
		}
		FreeSides(atuSides);
		FreeSides(adjSides);
		return fitness;
	}
	return 0;
}

//Obter valor de fitness de uma funcao. Este valor eh obtido para cada bit unido adequadamente entre duas pecas adjacentes. Cada peca eh checada em 3 de seus lados, num total de 19 bits. Logo, a fitness maxima equivale a 19 * matrixHeight * matrixWidth. Na solucao final, este valor eh 9728
int GetFitness(Solucao *s, int **feixes, int matrixHeight, int matrixWidth){
	int i, j, fitness = 0, fitn;
	for(i=0; i<matrixHeight; i++){
		for(j=0; j<matrixWidth; j++){
			//Obter fitness da peca [i][j] da matriz
			fitn = 0;
			fitn += GetSideFitness(i,j, 1, s, feixes, matrixHeight, matrixWidth);//Checar com peca adjacente 1 (ver descricao)
			fitn += GetSideFitness(i,j, 2, s, feixes, matrixHeight, matrixWidth);//Checar com peca adjacente 2 (ver descricao)
			fitn +=	GetSideFitness(i,j, 3, s, feixes, matrixHeight, matrixWidth);//Checar com peca adjacente 3 (ver descricao)
			fitness += fitn;
		}
	}
	//printf("Essa eh a fitness: %d\n", fitness);
	return fitness;
}
//
int partitionFitness(int *v, int *ind, int n){
	int i, j, aux, auxSol;
	i=0;
	for(j=1; j<n; j++){
		if(v[j] > v[0]){
			i++;
			aux = v[i];	v[i] = v[j]; v[j] = aux;
			auxSol = ind[i]; ind[i] = ind[j]; ind[j] = auxSol;
		}	
	}
	aux = v[0];	v[0] = v[i]; v[i] = aux;
	auxSol = ind[0]; ind[0] = ind[i]; ind[i] = auxSol;
	return i;
}
void OrderByFitness(int *fitness, int *ind, int n){
	int pivot;
	if(n>1){
		pivot = partitionFitness(fitness, ind, n);
		OrderByFitness(fitness, ind, pivot);
		OrderByFitness(fitness+pivot+1, ind+pivot+1, n-pivot-1);
	}	
}




int main (){

	//inputSize >= matrixHeight * matrixWidth
	
	//### Parâmetros de execução
	int civMax = 20;
	int genMax = 100;
	int maxPop = 20;//par
	int newPopPerGen = 6;//par

	//### Hiperparâmetros
	float pecaMutationRate = 0.2;
	float pecaParamMutationRate = 0.1;
	float mutatePecasMutRate = 0.7;//
	float mutateTapeteMutRate = 0.5;//


	int inputSize;
	int matrixHeight;
	int matrixWidth;	

	int i, j, k, gen, nSol;
	char word[32];
	int **feixes;
	Solucao **solucoes;	
	int bestFitnessEver = 0;
	
	time_t t;

	srand((unsigned) time(&t));

	scanf(" %d %d %d", &inputSize, &matrixWidth, &matrixHeight);
	if(inputSize < matrixWidth*matrixHeight)
		printf("ERRO: Número de peças não pode ser menor que o tamanho do tapete\n");

	printf("\n - TAPETE MUSICAL DE SACHS - \n\n");
	printf("### Parâmetros de execução\n");
	printf("	Máximo de civilizações = %d\n", civMax);
	printf("	Máximo de gerações = %d\n", genMax);
	printf("	População máxima = %d\n", maxPop);
	printf("	População nova por geração = %d\n", newPopPerGen);
	printf("### Hiperparâmetros\n");
	printf("	Taxa de mutação de novas soluções = %f\n", pecaMutationRate);
	printf("	Taxa de mutação da peça = %f\n", mutatePecasMutRate);
	printf("	Taxa de mutação de parâmetros de peça = %f\n", pecaParamMutationRate);
	printf("	Taxa de mutação do tapete = %f\n", mutateTapeteMutRate);
	printf("\n");
	printf("$$$ Dados de entrada\n");
	printf("	Quantidade de peças fornecidas = %d\n", inputSize);
	printf("	Tapete com proporção de %d X %d peças\n", matrixWidth, matrixHeight);
	printf("	Teto para pontuação máxima do tapete = %d\n", matrixWidth * matrixHeight * 19);//por peca
	printf("\n\n");
	

	//Inicializar ponteiros
	feixes = (int **)malloc(inputSize * sizeof(int *));
	solucoes = (Solucao **)malloc(maxPop * sizeof(Solucao *));

	//Captar pecas de entrada e alocar no vetor de feixes
	for(i=0; i<inputSize; i++){
		//vec[i] = (int *)malloc(32*sizeof(int));
		feixes[i] = (int *)malloc(32 * sizeof(int));
		scanf(" %s\n", word);
		//printf("%s\n", word);
		//Converter para 0s e 1s	
		for(j=0; j<32; j++){
			feixes[i][j] = word[j]-48;
		}
	}

	//Initiate vectors
	int *solFitness, *indexOrder;
	Solucao **sons;
	sons = (Solucao **)malloc(newPopPerGen * sizeof(Solucao *));
	solFitness = (int *)malloc(maxPop * sizeof(int));	
	indexOrder = (int *)malloc(maxPop * sizeof(int));


	for(int civ=0; civ < civMax; civ++){
		printf(">>>>>>> Começando civilização %d\n", civ+1);

		//Gerar maxPop solucoes iniciais
		for(i=0; i<maxPop; i++){
			//printf("\n\n=================\nNEW SOLUTION(%d)\n", i);
			solucoes[i] = GenerateNewSolucao(feixes, inputSize, matrixHeight, matrixWidth);
		}
				
		//Iniciar geracoes
		for(gen=0; gen<genMax; gen++){
			//if(gen % 50 == 0 && gen!=0)
			//	printf("== Geração %d\n", gen);
			
			//Obter valores de fitness para geracao atual	
			for(i=0; i<maxPop; i++)
				solFitness[i]=GetFitness(solucoes[i], feixes, matrixHeight, matrixWidth);
			
			//Ordenar por fitness em novo vetor
			for(k=0; k<maxPop; k++)
				indexOrder[k] = k;
			//printf("-> Ordenando fitness..");
			OrderByFitness(solFitness, indexOrder, maxPop);
			//if(gen % 50 == 0 && gen!=0)
			//	printf("Fitness variando de %d a %d\n", solFitness[0], solFitness[maxPop-1]);
			if(bestFitnessEver < solFitness[0])
				bestFitnessEver = solFitness[0];

			//Criar filho
			for(i=0; i<newPopPerGen; i++){
				sons[i] = GenerateBlankSolucao(feixes, inputSize, matrixHeight, matrixWidth);
			}
			
			//Escolher pares e alterar filho
			//Crossover (PMX)
			for(i=0; i<newPopPerGen; i++)
				for(j=0; j<inputSize; j++){
					sons[i]->pecas[j]->rotacao = solucoes[i]->pecas[j]->rotacao;
					sons[i]->pecas[j]->invertida = solucoes[i]->pecas[j]->invertida;
					sons[i]->pecas[j]->espelhada = solucoes[i]->pecas[j]->espelhada;								
				}		
			for(i=0; i<newPopPerGen/2; i++){
				int **f1, **f2, **p1, **p2;
				f1 = sons[(2*i)]->tapete;
				f2 = sons[(2*i)+1]->tapete;
				//Escolha dos pais
				p1 = solucoes[indexOrder[(rand() % (maxPop/2))]]->tapete;//random da melhor metade
				p2 = solucoes[indexOrder[(rand() % (maxPop/2)) + maxPop/2]]->tapete;//random da pior metade
				CrossoverSolution(p1, p2, f1, f2, matrixHeight, matrixWidth);
			}

			
			//Mutar filhos
			for(i=0; i<newPopPerGen; i++){
				if(randFloat()<=pecaMutationRate){
					//Mutarfloat pecaParamMutationRate = 0.1;
					MutateSolution(sons[i], inputSize, matrixHeight, matrixWidth,
						pecaParamMutationRate, mutatePecasMutRate, mutateTapeteMutRate);
				}
			}
			
			//Arrumar vetor de solucoes para proxima geracao
			for(i=0; i<newPopPerGen; i++){
				//Apagar as newPopPerGen piores solucoes de solucoes e dar lugar aos novos
				FreeSolucao(solucoes[indexOrder[maxPop-i-1]], inputSize, matrixHeight, matrixWidth);									
				solucoes[indexOrder[maxPop-i-1]] = sons[i];
			}				
			
		}
				
		//printf("==== Geração %d\n", gen);
		printf("- Fitness variando de %d a %d\n", solFitness[0], solFitness[maxPop-1]);

		for(i=0; i<maxPop; i++){			
			FreeSolucao(solucoes[i], inputSize, matrixHeight, matrixWidth);					
		}
		printf("\n");
	}

	//====Liberar memória
	free(solucoes);
	free(solFitness);
	free(indexOrder);
	free(sons);

	for(i=0; i<inputSize; i++)
		free(feixes[i]);
	free(feixes);

	printf("\n\n##### Melhor fitness encontrada = %d\n\n", bestFitnessEver);
	
	return 0;
}


//TestMethods
		/*int **sides;
		int *v;
		printf("\n\n\n\n\n");
		for(i=0; i<inputSize; i++){
			
			//Print Inicial
			v = feixes[i];
			printf("Valor Inicial: ");
			for(j=0; j<32; j++){
				printf("%d", v[j]);
			}
			printf("\n");
			
			//Teste InvertBits
			//solucoes[0]->pecas[i]->invertida = 1;

			//Teste Espelhada
			//solucoes[0]->pecas[i]->espelhada = 1;

			//Teste Rotacao
			//solucoes[0]->pecas[i]->rotacao = 3;
			
			//Print Teste
			v = GetTrueFeixe(solucoes[0]->pecas[i], feixes[i]);
			printf("Valor Real   : ");
			for(j=0; j<32; j++){
				printf("%d", v[j]);
			}
			printf("\n");
			free(v);
			
			
			//Print sides
			sides = GetSide(solucoes[0]->pecas[i], feixes[i]);
			for(j=0; j<6; j++){
				printf("Side[%d]: ", j);
				for(k=0; k<6; k++)
					printf("%d", sides[j][k]);
				if(j==0 || j==3)
					printf("%d", sides[j][6]);
				printf("\n");
			}
			FreeSides(sides);
			
			//Print full Peca
			PrintPeca(solucoes[0]->pecas[i], feixes[i]);
			
		}

		//printf("Fitness da solucao 1: %d\n", GetFitness(solucoes[0], feixes, matrixHeight, matrixWidth));

	
	//Print Pecas
	for(i=0; i<inputSize; i++){
		PrintPeca(s->pecas[i], feixes[i]);
	}
	//Print Tapete
	printf("#### TAPETE\n");
	for(i=0; i<matrixHeight; i++){
		printf("[");
		for(j=0; j<matrixWidth; j++){
			printf("%d, ", s->tapete[i][j]);
		}
		printf("] \a\n");
	}
	
		*/
		



