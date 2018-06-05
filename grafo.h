#include <stdio.h>
#include <stdlib.h>
#include<math.h>

/*
  O progama possui duas estruturas de dados: uma representando o grafo original e um com os vertices contraidos.
  O grafo final eh os dois grafos mesclados.
*/

/*! Registro das arestas. */
typedef struct {
  int id; /**< Identificador da aresta. */
  int saida; /**< Identificador do vertice no qual a aresta sai. */
  int chegada; /**< Identificador do outro vertice no qual a aresta chega. */
  double distancia; /**< Custo da aresta. */
}Aresta;

/*! Registro dos vertices. */
typedef struct {
  int id; /**< Identificador do vertice. */
  double latitude; /**< Latitude. */
  double longitude; /**< Longitude. */
  Aresta **saidas; /**< Arestas que saem do vertice. */
  int grauSaida; /**< Numero de arestas que saem do vertice. */
  int grauChegada; /**< Numero de arestas que chegam no vertice. */
}Vertice;


/*! Registro do grafo original.*/
typedef struct {
  Vertice *vertices; /**< Vetor de vertices do grafo. */
  Aresta *arestas; /**< Vetor de arestas do grafo. */
  int numeroVertices; /**< Numero de vertices no grafo. */
  int numeroArestas; /**< Numero de arestas no grafo. */
  int grauMaximo; /**< Grau maximo do grafo. */
  int arestasContraidas;
}Grafo;

/*! Registro dos vertices contraidos.*/
typedef struct {
  int id; /**< Identificador do vertice. */
  double latitude; /**< Latitude. */
  double longitude; /**< Longitude. */
  Aresta *ida; /**< Arestas que saem do vertice. */
  int grauSaida; /**< Numero de arestas que saem do vertice. */
  int grauChegada; /**< Numero de arestas que chegam no vertice. */
  int *caminho; /**< Caminho que o vertice contrai. */
  int tamanho; /**< Tamanho do caminho. */
}VerticeContraido;

/*! Registro dos vertices contraidos de mao dupla*/
typedef struct {
  int id; /**< Identificador do vertice. */
  double latitude; /**< Latitude. */
  double longitude; /**< Longitude. */
  Aresta *ida; /**< Aresta que sai do vertice de ida. */
  Aresta *volta; /**< Aresta que sai do vertice de volta. */
  int *caminho; /**< Caminho que o vertice contrai. */
  int tamanho; /**< Tamanho do caminho. */
}VerticeContraidoDuplo;

/*! Registro do grafo contraido.*/
typedef struct {
  VerticeContraido *vertices; /**< Vetor de vertices do grafo. */
  Aresta *arestas; /**< Vetor de arestas do grafo. */
  VerticeContraidoDuplo *verticesDuplo; /**< Vetor de vertices de mao dupla do grafo. */
  int numeroVertices; /**< Numero de vertices no grafo. */
  int numeroVerticesDuplo; /**< Numero de vertices no grafo. */
  int numeroArestas; /**< Numero de arestas no grafo. */
  int grauMaximo; /**< Grau maximo do grao. */
}Contraido;

/** Libera a memoria alocada para um grafo
 *
 *  g eh o grafo.
 */
void libera(Grafo *g)
{
  int i;
  for(i=0; i<g->numeroVertices; i++){
    if(g->vertices[i].grauSaida != 0)
      free(g->vertices[i].saidas);
  }
  free(g->vertices);
  free(g->arestas);
  for(i=0; i<g->numeroVertices; i++){
  }
  free(g);
  g = NULL;
}

/** Libera a memoria alocada para o grafo contraido
 *
 *  novo eh o grafo.
 */
void liberaNovo(Contraido *novo)
{
  int i;
  for(i=0; i<novo->numeroVertices; i++)
    free(novo->vertices[i].caminho);
  for(i=0; i<novo->numeroVerticesDuplo; i++)
    free(novo->verticesDuplo[i].caminho);
  free(novo->vertices);
  free(novo->verticesDuplo);
  free(novo->arestas);
  free(novo);
  novo = NULL;
}

/** Inicializa o grafo contraido
 *
 *  g eh o grafo a ser contraido.
 *  novo eh o grafo a ser inicializado
 */
void inicializaGrafoNovo(Grafo *g, Contraido *novo)
{
  int i;

  novo->numeroArestas = 0;
  novo->numeroVertices = 0;
  novo->numeroVerticesDuplo = 0;
  novo->grauMaximo = 0;

  novo->vertices= malloc(sizeof(VerticeContraido)*(g->numeroVertices));
  novo->verticesDuplo= malloc(sizeof(VerticeContraidoDuplo)*(g->numeroVertices));
  for(i=0; i<g->numeroVertices; i++){
    novo->vertices[i].id = i;
    novo->vertices[i].latitude = 0;
    novo->vertices[i].longitude = 0;
    novo->vertices[i].grauSaida = 0;
    novo->vertices[i].grauChegada = 0;
  }
  for(i=0; i<g->numeroVertices; i++){
    novo->verticesDuplo[i].id = i;
    novo->verticesDuplo[i].latitude = 0;
    novo->verticesDuplo[i].longitude = 0;
  }
  novo->arestas = malloc(sizeof(Aresta)*(g->numeroArestas));
}


/** Le a instancia
 *
 *  file eh o arquivo de leitura.
 *  g eh o grafo a ser criado.
 *  retorna 0 caso tenha dado erro na leitura, 1 caso contrario.
 */
int leInstancia(FILE *file, Grafo *g)
{
  int i, j, v1, v2, qnt, aresta;
  double lat, log, dist;

  g->numeroArestas = 0;
  g->numeroVertices = 0;
  g->grauMaximo = 0;
  fscanf(file, "G %d %d %d", &(g->numeroVertices), &(g->numeroArestas),&(g->grauMaximo));
  g->numeroVertices++;
  g->arestasContraidas = g->numeroArestas;
  g->vertices= malloc(sizeof(Vertice)*(g->numeroVertices));
  for(i=0; i<g->numeroVertices; i++){
    g->vertices[i].id = i;
    g->vertices[i].latitude = 0;
    g->vertices[i].longitude = 0;
    g->vertices[i].grauSaida = 0;
    g->vertices[i].saidas = NULL;
    g->vertices[i].grauChegada = 0;
  }

  g->arestas= malloc(sizeof(Aresta)*(g->numeroArestas));

  aresta = 0;
  for(i=1;i<g->numeroVertices;i++){
    fscanf(file, "\nN %d %lf %lf %d", &v1, &lat, &log, &qnt);
    g->vertices[v1].id = v1;
    g->vertices[v1].latitude = lat;
    g->vertices[v1].longitude = log;
    g->vertices[v1].grauSaida = qnt;
    if(g->vertices[v1].grauSaida != 0)
      g->vertices[v1].saidas = malloc(sizeof(Aresta*)*(g->vertices[v1].grauSaida));

    for(j=0; j<qnt; j++){
      fscanf(file, "%d %lf", &v2, &dist);
      g->arestas[aresta].id = aresta;
      g->arestas[aresta].chegada = v2;
      g->arestas[aresta].saida = v1;
      g->arestas[aresta].distancia = dist;
      g->vertices[v2].grauChegada++;
      
      g->vertices[v1].saidas[j] = &(g->arestas[aresta]);
      aresta++;
    }
  }

  if(aresta != g->numeroArestas) return 0;

  return 1;
}