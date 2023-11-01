/**
 * @author Rafael Emery Cade Poloni
 * Matrícula: 2017203662
 * Disciplina: Teoria dos Grafos - 2023/1
 * Data de entrega: 02/06/2023
 * 
 * Exercício "Ciclos".
 * 
 * gcc cycles.c -o cycles
 * ./cycles entrada.txt
*/
/**
 * Ciclos
 * 
 * Análise do problema:
 * 
 * O problema consiste em "n" planetas e "m" linhas.
 * Cada linhas possui um custo "c" que pode ser positivo ou negativo.
 * Quanto maior o valor de uma linha, mais indesejável é para ser utilizada.
 * Os valores das arestas devem ser atribuṕidos de modo que não hajam ciclos negativos.
 * O objetivo é ler dos dados e retornar se há ou não um ciclo negativo.
 * Existem condições para os valores de entrada.
 * Caso o diegrama tenha um ciclo negativo, retorne "SIM", caso não, "NÃO".
 * 
 * Solução do problema:
 * 
 * Funções básicas de grafos utilizando matriz de adjacências (considerando valor de arestas).
 * Leitura de arquivo "entrada.txt".
 * Validações de valores de entrada.
 * Função que verifica se há um ciclo negativo com o algoritmo de Bellman-Ford.
 * Recebe o valor booleano resultante de Bellman-Ford. Será utilizado este algoritmo
 * e não Dijkstra pois existem arestas com valor negativo.
 * Imprime a saída.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 
#include <time.h>

#define INFINITO 99999999

// Estrutura que define um grafo com matriz de adjacências.
typedef struct grafo {
    int V;
    int E;
    int **adj;
} Grafo;

// Funções presentes na aula 2 para representação de Grafos em C.
int **alocaMatrizAdj(int r, int c, int val);
Grafo *criaGrafo(int V);
// Função de inserção de aresta modificada para receber o peso da aresta.
void insereArestaComPeso(Grafo *G, int v, int w, int peso);
void removeAresta(Grafo *G, int v, int w);
void imprimeGrafo(Grafo *G);

// Funções definidas pelo autor
bool verificaCaminhosNegativosBellmanFord(Grafo *G, int inicio);

int main() {
    int qtdPlanetas = 0, qtdVias = 0;
    int i;
    FILE *arquivoEntrada = fopen("entrada.txt", "r");

    double time_spent = 0.0;

    clock_t begin = clock();
 
    // Criando o grafo a partir do arquivo entrada.txt
    if (arquivoEntrada == NULL) {
        printf("Erro ao abrir o arquivo de entrada e/ou arquivo inválido.\n");
        return 0;
    }

    fscanf(arquivoEntrada, "%d %d", &qtdPlanetas, &qtdVias);
    printf("Existem %d planetas e %d vias.\n\n", qtdPlanetas, qtdVias);

    // Criando grafo a partir dos valores presentes na primeira linha do arquivo
    // Planetas -> Vértices. Linhas -> Arestas.
    Grafo *grafo = criaGrafo(qtdPlanetas);

    int planetaEntrada = 0, planetaSaida = 0, valorVia = 0;

    for (i = 0; i < qtdVias; i++) {
        fscanf(arquivoEntrada, "%d %d %d", &planetaEntrada, &planetaSaida, &valorVia);
        printf("Definindo a via entre os planetas %d e %d.\n", planetaEntrada, planetaSaida);

        insereArestaComPeso(grafo, planetaEntrada - 1, planetaSaida - 1, valorVia);
    }

    imprimeGrafo(grafo);

    bool possuiCaminhosNegativos = verificaCaminhosNegativosBellmanFord(grafo, grafo->adj[0][0]);

    printf("\n\n");
    if (possuiCaminhosNegativos == true) {
        printf("SIM\n");
    } else {
        printf("NÃO\n");
    }

    clock_t end = clock();

    time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
    printf("(Limite de tempo: 1s) Tempo: %f\n", time_spent);

    return 0;
}

int **alocaMatrizAdj(int r, int c, int val) {
    int i, j;
    int **m = malloc(r* sizeof(int *));

    for (i = 0; i < r; i++) {
        m[i] = malloc(c* sizeof(int *));
    }
    for (i = 0; i < r; i++) {
        for (j = 0; j < c; j++) {
            m[i][j] = val;
        }
    }

    return m;
}

Grafo *criaGrafo(int V) {
    Grafo *G = malloc(sizeof(Grafo));

    G->V = V;
    G->E = 0;
    G->adj = alocaMatrizAdj(V, V, 0);

    return G;
}

// Função para inserção de aresta considerando o peso, sendo simplesmente 
// armazenado como aresta na linha v e coluna w (e vice-versa) da matriz.
void insereArestaComPeso(Grafo *G, int v, int w, int peso) {
    if (v != w && G->adj[v][w] == 0) {
        G->adj[v][w] = peso;
        G->adj[w][v] = peso;
        
        G->E++;
    }
}

void removeAresta(Grafo *G, int v, int w) {
    if (G->adj[v][w] == 1) {
        G->adj[v][w] = 0;
        G->adj[w][v] = 0;
        
        G->E--;
    }
}

void imprimeGrafo(Grafo *G) {
    int v, w;

    printf("\n\nImprimindo grafo:\n");

    for (v = 0; v < G->V; v++) {
        printf("%2d: ", v + 1);

        for (w = 0; w < G->V; w++) {
            if (G->adj[v][w] == 1) {
                printf(" %2d", w + 1);
            }
        }
        printf("\n");
    }
}

// Função que implementa o algoritmo de Bellman-Ford para demonstrar se
// existe ou não ciclo negativo.
bool verificaCaminhosNegativosBellmanFord(Grafo *G, int inicio) {
    bool possuiCicloNegativo = false;
    int i, u, v;
    int dist[G->V]; // Todas as distâncias.

    // Inicializa todas as distâncias como infinito.
    for (int i = 0; i < G->V; i++) {
        dist[i] = INFINITO;
    }

    // Define a distância do vértice de origem como 0.
    dist[inicio] = 0;

    // Relaxa as arestas repetidamente.
    for (i = 0; i < G->V - 1; i++) {
        for (u = 0; u < G->V; u++) {
            for (v = 0; v < G->V; v++) {
                if (G->adj[u][v] != 0 && dist[u] != INFINITO && dist[u] + G->adj[u][v] < dist[v]) {
                    dist[v] = dist[u] + G->adj[u][v];
                }
            }
        }
    }

    // Verifica se há ciclos negativos.
    for (int u = 0; u < G->V; u++) {
        for (int v = 0; v < G->V; v++) {
            if (G->adj[u][v] != 0 && dist[u] != INFINITO && dist[u] + G->adj[u][v] < dist[v]) {
                possuiCicloNegativo = true;
            }
        }
    }

    return possuiCicloNegativo;
}
