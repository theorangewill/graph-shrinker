#include <stdio.h>
#include <stdlib.h>

/*! Registro das arestas. */
typedef struct {
  int id; /**< Identificador da aresta. */
  int saida; /**< Identificador do vertice no qual a aresta sai. */
  int chegada; /**< Identificador do outro vertice no qual a aresta chega. */
  float distancia; /**< Custo da aresta. */
}Aresta;


/*! Registro dos vertices. 
*/
typedef struct {
  int id; /**< Identificador do vertice. */
  float latitude; /**< Latitude. */
  float longitude; /**< Longitude. */
  Aresta **saidas; /**< Arestas que saem do vertice. */
  int grauSaida; /**< Numero de arestas que saem do vertice. */
  //Aresta **chegadas; /**< Arestas que chegam no vertice. */
  int grauChegada; /**< Numero de arestas que chegam no vertice. */
}Vertice;


/*! Registro do grafo. 
  matrizsaida[i][j] = a ----> aresta A sai de I e chega em J
  matrizchegada[i][j] = a ----> aresta A chega em I e sai de J
*/
typedef struct {
  //int **matrizchegada; /**< Matriz que associa os vertices as arestas de chegada. */
  //int **matrizsaida; /**< Matriz que associa os vertices as arestas de saida. */
  Vertice *vertices; /**< Vetor de vertices do grafo. */
  Aresta *arestas; /**< Vetor de arestas do grafo. */
  int numeroVertices; /**< Numero de vertices no grafo. */
  int numeroArestas; /**< Numero de arestas no grafo. */
  int grauMaximo; /**< Grau maximo do grao. */
}Grafo;


/** Libera a memoria alocada para um grafo
 *
 *  g eh o grafo.
 */
void libera(Grafo *g)
{
  int i;
  for(i=0; i<g->numeroVertices; i++){
    //free(g->vertices[i].chegadas);
    if(g->vertices[i].grauSaida != 0)
      free(g->vertices[i].saidas);
  }
  free(g->vertices);
  free(g->arestas);
  for(i=0; i<g->numeroVertices; i++){
    //free(g->matrizsaida[i]);
    //free(g->matrizchegada[i]);
  }
  //free(g->matrizsaida);
  //free(g->matrizchegada);
  free(g);
  g = NULL;
}

/** Libera a memoria alocada para um grafo
 *
 *  g eh o grafo.
 */
void liberaNovo(Grafo *g)
{
  int i;
  for(i=0; i<g->numeroVertices; i++){
    free(g->vertices[i].saidas);
  }
  free(g->vertices);
  free(g->arestas);
  free(g);
  g = NULL;
}

/** Le a instancia
 *
 *  file eh o arquivo de leitura.
 *  g eh o grafo a ser criado.
 *  retorna 0 caso tenha dado erro na leitura, 1 caso contrario.
 */
int inicializaGrafoNovo(Grafo *g, Grafo *novo)
{
  int i;

  novo->numeroArestas = 0;
  novo->numeroVertices = 0;
  novo->grauMaximo = 0;

  novo->vertices= malloc(sizeof(Vertice)*(g->numeroVertices));
  for(i=0; i<g->numeroVertices; i++){
    novo->vertices[i].id = i;
    novo->vertices[i].latitude = 0;
    novo->vertices[i].longitude = 0;
    novo->vertices[i].grauSaida = 0;
    novo->vertices[i].saidas = NULL;
    novo->vertices[i].grauChegada = 0;
  }
  novo->arestas= malloc(sizeof(Aresta)*(g->numeroArestas));

  return 1;
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
  float lat, log, dist;

  g->numeroArestas = 0;
  g->numeroVertices = 0;
  g->grauMaximo = 0;
  fscanf(file, "G %d %d %d", &(g->numeroVertices), &(g->numeroArestas),&(g->grauMaximo));
  g->numeroVertices++;

  g->vertices= malloc(sizeof(Vertice)*(g->numeroVertices));
  for(i=0; i<g->numeroVertices; i++){
    g->vertices[i].id = i;
    g->vertices[i].latitude = 0;
    g->vertices[i].longitude = 0;
    g->vertices[i].grauSaida = 0;
    g->vertices[i].saidas = NULL;
    g->vertices[i].grauChegada = 0;
    //g->vertices[i].chegadas = NULL;
  }

  // g->matrizsaida= malloc(sizeof(int*)*(g->numeroVertices));
  // g->matrizchegada= malloc(sizeof(int*)*(g->numeroVertices));
  // for(i=0; i<g->numeroVertices; i++){
  //   g->matrizsaida[i]= malloc(sizeof(int)*(g->numeroVertices));
  //   g->matrizchegada[i]= malloc(sizeof(int)*(g->numeroVertices));
  //   //g->matriz[i] = calloc(g->numeroVertices,sizeof(int));

  //   for(j=0; j<g->numeroVertices; j++){
  //     g->matrizchegada[i][j] = -1;
  //     g->matrizsaida[i][j] = -1;
  //   }
  // }
  g->arestas= malloc(sizeof(Aresta)*(g->numeroArestas));

  aresta = 0;
  for(i=1;i<g->numeroVertices;i++){
    fscanf(file, "\nN %d %f %f %d", &v1, &lat, &log, &qnt);
    //printf("%d %f %f %d", v1, lat, log, qnt);
    g->vertices[v1].id = v1;
    g->vertices[v1].latitude = lat;
    g->vertices[v1].longitude = log;
    g->vertices[v1].grauSaida = qnt;

    if(g->vertices[v1].grauSaida != 0)
      g->vertices[v1].saidas = malloc(sizeof(Aresta*)*(g->vertices[v1].grauSaida));

    for(j=0; j<qnt; j++){
      fscanf(file, "%d %f", &v2, &dist);
      //printf(" %d %f", v2, dist);
      //if(g->vertices[v2].grauChegada == 0)
      //  g->vertices[v2].chegadas = malloc(sizeof(Aresta*)*(g->grauMaximo));

      g->arestas[aresta].id = aresta;
      g->arestas[aresta].chegada = v2;
      g->arestas[aresta].saida = v1;
      g->arestas[aresta].distancia = dist;

      //g->matrizsaida[v1][v2] = aresta;
      //g->matrizchegada[v2][v1] = aresta;

      //g->vertices[v2].chegadas[g->vertices[v2].grauChegada] = &(g->arestas[aresta]);
      g->vertices[v2].grauChegada++;
      
      g->vertices[v1].saidas[j] = &(g->arestas[aresta]);
      aresta++;
    }
    //printf("\n");
  }

  if(aresta != g->numeroArestas) return 0;

  return 1;
}

/** Imprimir o grafo
 *
 *  g eh o grafo a ser impresso na tela.
 */
void imprimeGrafo(Grafo *g)
{
  printf("NUMERO DE VERTICES:\t%d\n", g->numeroVertices);
  printf("NUMERO DE ARESTAS:\t%d\n", g->numeroArestas);
  printf("GRAU MAXIMO:\t%d\n", g->grauMaximo);
  int i, j;
  for(i=0; i<g->numeroVertices; i++){
    printf("%d(%f,%f)\n",g->vertices[i].id,g->vertices[i].latitude,g->vertices[i].longitude);
    // if(g->vertices[i].grauSaida>0){
    //   printf("\tSAI PARA:\n");
    // }
    // else
    //   printf("\tNAO SAI PARA NINGUEM\n");
    for(j=0; j<g->vertices[i].grauSaida; j++){
      printf("\ta%d(%d->%d)[%f]\n", 
        g->vertices[i].saidas[j]->id,
        g->vertices[i].saidas[j]->saida,
        g->vertices[i].saidas[j]->chegada,
        g->vertices[i].saidas[j]->distancia);
    }
    printf("CHEGAM NELE: %d\n", g->vertices[i].grauChegada);
    // if(g->vertices[i].grauChegada>0){
    //   printf("\tCHEGA DE:\n");
    // }
    // else
    //   printf("\tNAO CHEGA DE NINGUEM\n");
    // for(j=0; j<g->vertices[i].grauChegada; j++){
    //   printf("\t\ta%d(%d<-%d)[%f]\n", 
    //     g->vertices[i].chegadas[j]->id,
    //     g->vertices[i].chegadas[j]->chegada,
    //     g->vertices[i].chegadas[j]->saida,
    //     g->vertices[i].chegadas[j]->distancia);
    // }
    printf("-------------------\n");
  }
}