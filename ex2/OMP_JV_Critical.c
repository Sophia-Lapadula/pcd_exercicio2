#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int iterations, N, n_cels_totais;
struct timeval start_Critical, end_Critical;

int** constroiMatriz(){
    int i, j;
    int** matriz = (int**) malloc(N * sizeof(int*));
    
    for(i=0;i<N;i++){
        matriz[i] = (int*) malloc(N * sizeof(int));
        for(j=0;j<N;j++){
            matriz[i][j] = 0;
        }
    }
    return matriz;
}


void tabuleiroInicial(int** grid){
    int lin = 1, col = 1;
    //inicializa o Glider
    grid[lin  ][col+1] = 1;
    grid[lin+1][col+2] = 1;
    grid[lin+2][col  ] = 1;
    grid[lin+2][col+1] = 1;
    grid[lin+2][col+2] = 1;
    //-------------------
    lin = 10;
    col = 30;
    //inicializa o R-pentonimo
    grid[lin  ][col+1] = 1;
    grid[lin  ][col+2] = 1;
    grid[lin+1][col  ] = 1;
    grid[lin+1][col+1] = 1;
    grid[lin+2][col+1] = 1;
    //-------------------
}


int getLivingCells(int** grid){
    gettimeofday(&start_Critical, NULL);
    int i, j, sum = 0;

    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            sum += grid[i][j]; 
        }
    }

    return sum;
}


void printTabuleiro(int** grid, int n){
    
    int i, j;

    printf("Tabuleiro %d\n", n);

    for(i=0;i<50;i++){
        for(j=0;j<50;j++){
            if(grid[i][j] == 1){
                printf("#");    
            }else{
                printf(".");
            }
            
        }
        printf("\n");
    }
}


int getLivingNeighbors(int** grid, int i, int j){
    int nVizinhos = 0;
    
    // confere vizinho de cima e esquerda
    if(i == 0){
        if (j == 0){
            nVizinhos += grid[N-1][N-1];
        } else {
            nVizinhos += grid[N-1][j-1];
        }
    } else {
        if (j == 0){
            nVizinhos += grid[i-1][N-1];
        } else {
            nVizinhos += grid[i-1][j-1];
        }
    }


    // confere vizinho de cima
    if(i == 0){
        nVizinhos += grid[N-1][j];
    } else {
        nVizinhos += grid[i-1][j];
    }


    // confere vizinho de cima e direita
    if(i == 0){
        if (j == N-1){
            nVizinhos += grid[N-1][0];
        } else {
            nVizinhos += grid[N-1][j+1];
        }
    } else {
        if (j == N-1){
            nVizinhos += grid[i-1][0];
        } else {
            nVizinhos += grid[i-1][j+1];
        }
    }

    // confere vizinho da direita
    if(j == N-1){
        nVizinhos += grid[i][0];
    } else {
        nVizinhos += grid[i][j+1];
    }


    // confere vizinho de baixo e direita
    if(i == N-1){
        if (j == N-1){
            nVizinhos += grid[0][0];
        } else {
            nVizinhos += grid[0][j+1];
        }
    } else {
        if (j == N-1){
            nVizinhos += grid[i+1][0];
        } else {
            nVizinhos += grid[i+1][j+1];
        }
    }


    // confere vizinho de baixo
    if(i == N-1){
        nVizinhos += grid[0][j];
    } else {
        nVizinhos += grid[i+1][j];
    }


    // confere vizinho de baixo e esquerda
    if(i == N-1){
        if (j == 0){
            nVizinhos += grid[0][N-1];
        } else {
            nVizinhos += grid[0][j-1];
        }
    } else {
        if (j == 0){
            nVizinhos += grid[i+1][N-1];
        } else {
            nVizinhos += grid[i+1][j-1];
        }
    }


    // confere vizinho da esquerda
    if(j == 0){
        nVizinhos += grid[i][N-1];
    } else {
        nVizinhos += grid[i][j-1];
    }


    return nVizinhos;
}


 //Regras do jogo
int defineStatus(int neighbors, int currentStatus){

    if(currentStatus == 1){
        if(neighbors == 2 || neighbors == 3){
            return 1;
        }
    } else {
        if(neighbors == 3){
            return 1;
        }
    }

    return 0;
}


void updateBoard(int** grid, int** newgrid){
    int i, j;

    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            grid[i][j] = newgrid[i][j]; 
        }
    }
}


void getGeneration(int** grid, int** newgrid, int numTh, int k){
    
    int i, j, neighbors;
    //criacao da regiao paralela para o for
    #pragma omp parallel for num_threads(numTh) \
        private (i, j, neighbors) \
        shared (grid, newgrid)
    
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            neighbors = getLivingNeighbors(grid, i, j);
            newgrid[i][j] = defineStatus(neighbors, grid[i][j]);
        }
    }

    updateBoard(grid, newgrid);
    if(k==iterations){
        #pragma omp critical
        n_cels_totais = getLivingCells(grid);
        gettimeofday(&end_Critical, NULL);

        float tempo_Critical = (float) ((end_Critical.tv_sec-start_Critical.tv_sec)+(end_Critical.tv_usec-start_Critical.tv_usec));

        printf("Tempo de execucao parte pragma critical: %f milissegundos.\n", tempo_Critical);
    }
}


int main() {
    struct timeval start_Paralelo, end_Paralelo, start_Code, end_Code;
    printf("N: ");
    scanf("%d", &N);

    int i, numTh;
    printf("Generations: ");
    scanf("%d", &iterations);

    printf("Numero de threads: ");
    scanf("%d", &numTh);

    gettimeofday(&start_Code, NULL);

    int** grid = constroiMatriz();
    int** newgrid = constroiMatriz();
    tabuleiroInicial(grid);
    printTabuleiro(grid, 0);
    
    gettimeofday(&start_Paralelo, NULL);

    for(i = 1; i<=iterations; i++){
        getGeneration(grid, newgrid, numTh, i);
        if(i<=5){
            printTabuleiro(grid, i);
        }
    }
   
    gettimeofday(&end_Paralelo, NULL);

    float tempo_Paralelo = (float) (1000 * (end_Paralelo.tv_sec-start_Paralelo.tv_sec)+(end_Paralelo.tv_usec-start_Paralelo.tv_usec)/1000);

    printTabuleiro(grid, i);
    printf("Quantidade de celulas vivas: %d\n", n_cels_totais);
    printf("\n\n");
    gettimeofday(&end_Code, NULL);

    float tempo_Total = (float) (1000 * (end_Code.tv_sec-start_Code.tv_sec)+(end_Code.tv_usec-start_Code.tv_usec)/1000);

    printf("Tempo de execucao parte paralela: %f milissegundos.\n", tempo_Paralelo);
    printf("Tempo de execucao total: %f milissegundos.", tempo_Total);
    
    return 0;
}

