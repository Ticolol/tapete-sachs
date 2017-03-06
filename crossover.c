#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>

void printM(int **m, int si, int sj){
	int i, j;
	for (i=0; i<si; i++){
		for (j=0; j<sj; j++){
			printf(" %d,", m[i][j]);
		}		
		printf("\n");
	}
}



int main(){
	time_t t;

	srand((unsigned) time(&t));

	int **p1, **p2, **f1, **f2;
	int w = 3, h = 3;
	int i, j, k, l;

	p1 = (int **)malloc(w * sizeof(int *));
	p2 = (int **)malloc(w * sizeof(int *));
	f1 = (int **)malloc(w * sizeof(int *));
	f2 = (int **)malloc(w * sizeof(int *));
	for(i=0; i<w; i++){
		p1[i] = (int *)malloc(h * sizeof(int));
		p2[i] = (int *)malloc(h * sizeof(int));
		f1[i] = (int *)malloc(h * sizeof(int));
		f2[i] = (int *)malloc(h * sizeof(int));		
	}

	for(i=0; i<w; i++)
		for(j=0; j<h; j++)
			scanf(" %d", &p1[i][j]);
	for(i=0; i<w; i++)
		for(j=0; j<h; j++)
			scanf(" %d", &p2[i][j]);

	for(i=0; i<w; i++)
		for(j=0; j<h; j++){
			f1[i][j]=0;
			f2[i][j]=0;
		}
	
	printM(p1, w, h);
	printf("----------------\n");
	printM(p2, w, h);
	printf("================\n");


	//Obter região de crossover
	//int x1=1, x2=3;
	//int y1=1, y2=3;
	int x1=(rand() % w);
	int x2=(rand() % (w-x1))+(x1+1);
	int y1=(rand() % h);
	int y2=(rand() % (h-y1))+(y1+1);

	printf("x1=%d, x2=%d\n", x1, x2);
	printf("y1=%d, y2=%d\n", y1, y2);
	
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

				flag=1;
				f1[i][j]=p1[i][j];
				while(flag){
					flag=0;
					//Checar se valor atual está no corte			
					for(k=x1; k<x2 && !flag; k++)
						for(l=y1; l<y2 && !flag; l++){
							if(f1[i][j]==f1[k][l]){
								//Trocar
								f1[i][j]=p1[k][l];
								flag = 1;
							}
						}
				}//LOOP



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
				}//LOOP

					
			}

		}



	printM(f1, w, h);
	printf("----------------\n");
	printM(f2, w, h);

	free(p1);
	free(p2);
	free(f1);
	free(f2);
	return 0;
}

