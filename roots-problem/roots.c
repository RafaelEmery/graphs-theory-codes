/**
 * @author Rafael Emery Cade Poloni
 * Matrícula: 2017203662
 * Disciplina: Teoria dos Grafos - 2023/1
 * Data de entrega: 02/06/2023
 * 
 * Exercício "Raízes"
 * 
 * gcc roots.c -o roots
 * ./roots entrada.txt
*/

/**
 * Análise do problema:
 * 
 * "n" e "m" representam o número de vértices e arestas
 * "a" e "b" representam a aresta entre os vétices "a" e "b"
 * Existem restrições de pelo menos 2 vértices e pelo menos uma aresta, sendo
 * os valores de "a" e "b" valores reais
 * 
 * Solução do problema:
 * 
 * A solução consiste em imprimir a rota dos vértices (chamados de cruzamentos) visitados, e
 * deve sempre retornar ao vértice inicial. A solução consiste em um percurso em um grafo Euleriano.
 * Caso não seja Euleriano, a solução é considerada impossível. Caso seja, deve retornar o percurso
 * corretamente.
 * 
 * Para verificar a propriedade que queremos, iremos utilizar o algoritmo de Fleury.
 * 
 * Inserir as entradas por arquivo
 * Criar o grafo com matriz de adjacências
 * Verificar se o grafo inserido é Euleriano
 * Aplicar o algoritmo de Fleury
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 
#include <time.h>

// Estrutura que define um grafo com matriz de adjacências.
typedef struct grafo {
    int V;
    int E;
    int **adj;
} Grafo;

int **alocaMatrizAdj(int r, int c, int val);
Grafo *criaGrafo(int V);
void insereAresta(Grafo *G, int v, int w);
void removeAresta(Grafo *G, int v, int w);
void imprimeGrafo(Grafo *G);
bool verificaGrafoConexo(Grafo *G);
void buscaProfundidade(Grafo *G, int v, bool visitado[]);
bool verticesPossuemGrauPar(Grafo *G);
bool verificaArestaPonte(Grafo* grafo, int v);
void verticesGrafoEulerianoFleury(Grafo *grafo, int start, Grafo *tempGrafo);

int main() {
    int qtdCruzamentos = 0, qtdRuas = 0;
    int i;
    FILE *arquivoEntrada = fopen("entrada.txt", "r");
    bool grafoConexo = true;
    bool grafoComGrausPares = true;

    double time_spent = 0.0;

    clock_t begin = clock();

    // Criando o grafo a partir do arquivo entrada.txt
    if (arquivoEntrada == NULL) {
        printf("Erro ao abrir o arquivo de entrada e/ou arquivo inválido.\n");
        return 0;
    }

    fscanf(arquivoEntrada, "%d %d", &qtdCruzamentos, &qtdRuas);
    printf("Existem %d cruzamentos e %d ruas.\n\n", qtdCruzamentos, qtdRuas);

    if (qtdCruzamentos < 2 || qtdCruzamentos >= 1000000 || qtdRuas < 0 || qtdRuas >= 2000000) {
        printf("Entrada inválida!");
        return 0;
    }

    // Criando grafo a partir dos valores presentes na primeira linha do arquivo
    // Cruzamentos -> Vértices. Ruas -> Arestas.
    Grafo *grafo = criaGrafo(qtdCruzamentos);
    Grafo *grafoAuxiliar = criaGrafo(qtdCruzamentos);

    int cruzamentoEntrada = 0, cruzamentoSaida = 0;

    for (i = 0; i < qtdRuas; i++) {
        fscanf(arquivoEntrada, "%d %d", &cruzamentoEntrada, &cruzamentoSaida);
        printf("Definindo a rua entre o cruzamento de %d e %d.\n", cruzamentoEntrada, cruzamentoSaida);

        if (cruzamentoEntrada < 0 || cruzamentoSaida > qtdCruzamentos) {
            printf("Entrada inválida! %d", cruzamentoSaida);
            return 0;
        }

        insereAresta(grafo, cruzamentoEntrada - 1, cruzamentoSaida - 1);

        // O grafo auxiliar será útil na aplicação do algoritmo de Fleury.
        insereAresta(grafoAuxiliar, cruzamentoEntrada - 1, cruzamentoSaida - 1);
    }

    imprimeGrafo(grafo);

    // Realiza uma verificação se o grafo é conexo ou não.
    grafoConexo = verificaGrafoConexo(grafo);
    // Realiza uma verificação se o grafo possui todos os vértices com graus pares.
    grafoComGrausPares = verticesPossuemGrauPar(grafo);

    // Realiza as duas verificações: Se o grafo for conexo e se todos os graus forem par.
    // Um grafo de Euler deve satisfazer ambas condições. Caso não satisfaça, irá retornar
    // "Impossível" pois não é possível obter o caminho que o problema quer.
    if (grafoConexo != true && grafoComGrausPares != true) {
        printf("Impossível.\n");
        return 0;
    }
    printf("\nO grafo é conexo e possui todos os graus de seus vértices par.\n");

    int inicioCaminho = 0;
    printf("\nCaminho:\n %d", inicioCaminho + 1);

    // Retorna o caminho do grafo Euleriano utilizando a implementação do Algoritmo de Fleury.
    verticesGrafoEulerianoFleury(grafo, inicioCaminho, grafoAuxiliar);
    printf("\n");

    clock_t end = clock();

    time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
    printf("(Limite de tempo: 1s) Tempo: %f\n", time_spent);

    return 1;
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

void insereAresta(Grafo *G, int v, int w) {
    if (v != w && G->adj[v][w] == 0) {
        G->adj[v][w] = 1;
        G->adj[w][v] = 1;
        
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

bool verificaGrafoConexo(Grafo *G) {
    bool verticesVisitados[G->V]; 
    int i;

    // Inicializa todos os vértices como não visitados.
    for (i = 0; i < G->V; i++) {
        verticesVisitados[i] = false;
    }

    // Encontra o primeiro vértice não nulo (que possui aresta).
    for (i = 0; i < G->V; i++) {
        if (G->adj[i][0] == 1) {
            break;
        }
    }

    // Realiza uma busca em profundidade (recursivamente) que guarda todos os vértices visitados
    // Caso não tenham sido todos os vértices visitados, significa que o grafo é desconexo
    // pois não conseguiu acesso visto que não tem aresta ligando.
    buscaProfundidade(G, i, verticesVisitados);

    // Verifica se todos os vértices foram visitados
    for (i = 0; i < G->V; i++) {
        if (verticesVisitados[i] == false) {
            return false;
        }
    }

    return true;
}

void buscaProfundidade(Grafo *G, int v, bool visitado[]) {
    visitado[v] = true;

    for (int i = 0; i < G->V; i++) {
        if (G->adj[v][i] && visitado[i] == false) {
            buscaProfundidade(G, i, visitado);
        }
    }
}

// Verificação se todos os graus de vértices do Grafo são pares (propriedade de grafos Eulerianos)
bool verticesPossuemGrauPar(Grafo *G) { 
    int grau[G->V];
    int i = 0;
    int j = 0;
    
    // Inicializa o grau de todos os vértices como zero.
    for (i = 0; i < G->V; i++) {
        grau[i] = 0;
    }

    // Irá calcular o grau de todos os vértices do Grafo utilizando a matriz de adjacência.
    for (i = 0; i < G->V; i++) {
        for (j = 0; j < G->E; j++) {
            if (G->adj[i][j]) {
                grau[i]++;
            }
        }
    }

    // Verificação de graus de todos os vértices do grafo
    for (i = 0; i < G->V; i++) {
        // Caso exista um vértice com grau não par (ímpar), retorna false.
        if (grau[i] % 2 != 0) {
            return false;
        }
    }

    // Todos os vértices do grafo possuem grau par, retorna true.
    return true;
}

bool verificaArestaPonte(Grafo* G, int v) {
    int i;
    int grau = 0;

    for(i = 0; i < G->V; i++) {
        // Caso tenha adjacência com o vértice passado, incrementa o grau.
        if (G->adj[v][i]) {
            grau++;
        }
        if (grau > 1) {
            return false;
        }
    }

    return true;
}

// Imprime o caminho resultante do algoritmo de Fleury aplicando-o recursivamente com um grafo
// auxliar de referência. É necessário neste caso pois subtrai as arestas totais do mesmo e 
// podemos manter a estrutura do grafo inicial.
void verticesGrafoEulerianoFleury(Grafo *G, int verticeInicial, Grafo* grafoAuxiliar) {
    int qtdArestas = grafoAuxiliar->E;
    int arestaPonte = 0;
    int i;
    
    for(i = 0; i < G->V; i++) {        
        if(grafoAuxiliar->adj[verticeInicial][i]) {
            // Realiza a verificação se a aresta com vértice "i" é uma ponte.
            arestaPonte = verificaArestaPonte(grafoAuxiliar, i);
            
            // Caso não seja ponte, irá imprimir, pois se for uma ponte não é possível passar e o
            // grafo também não será Euleriano.
            if(qtdArestas <= 1 || arestaPonte != true) {
                printf(" - %d", i + 1);
                
                // Subtrai as arestas e representa a aresta entre i e verticeInicial passado como 
                // início/parâmetro da função como "nula" quando atribui 0 na matriz de adjacência.
                grafoAuxiliar->E--;
                grafoAuxiliar->adj[verticeInicial][i] = 0;
                grafoAuxiliar->adj[i][verticeInicial] = 0;
                
                verticesGrafoEulerianoFleury(G, i, grafoAuxiliar);
            } 
        }
    }
}
