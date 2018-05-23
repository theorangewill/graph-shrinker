#include "grafo.h"


void imprimeCaminho(int *caminho, int tam)
{
  printf("CAMINHO: ");
  for(int i=0; i<tam; i++) printf("%d ", caminho[i]);
  printf("\n");
}

void isolaVertice(Grafo *g, int vertice)
{
  free(g->vertices[vertice].saidas);
  g->vertices[vertice].grauSaida = 0;
  g->vertices[vertice].grauChegada = 0;
}

void trocaVizinho(Grafo *g, int vertice, int vizinho, int novo, float distancia)
{
  for(int v=0; v<g->vertices[vertice].grauSaida; v++){
    if(g->vertices[vertice].saidas[v]->chegada == vizinho){  
      g->vertices[vertice].saidas[v]->chegada = novo;
      g->vertices[vertice].saidas[v]->distancia = distancia;
      break;
    }
  }
}

void criaVertice(Grafo *novo, Grafo *g, int vizinho, float distancia)
{
  Vertice *vertice = malloc(sizeof(Vertice));
  vertice->id = g->numeroVertices + novo->numeroVertices;
  vertice->latitude = 0;
  vertice->longitude = 0;
  vertice->grauSaida = 1;
  vertice->saidas = malloc(sizeof(Aresta));
  vertice->grauChegada = 1;
  Aresta *aresta = malloc(sizeof(Aresta));
  aresta->id = g->numeroArestas + novo->numeroArestas;
  aresta->chegada = vizinho;
  aresta->saida = vertice->id;
  aresta->distancia = distancia;
  vertice->saidas[0] = aresta;

  novo->vertices[novo->numeroVertices] = *vertice;
  novo->arestas[novo->numeroArestas] = *aresta;
  novo->numeroVertices++;
  novo->numeroArestas++;
}

void criaVertice2(Grafo *novo, Grafo *g, int volta, int ida, float distanciaVolta, float distanciaIda)
{
  Vertice *vertice = malloc(sizeof(Vertice));
  vertice->id = g->numeroVertices + novo->numeroVertices;
  vertice->latitude = 0;
  vertice->longitude = 0;
  vertice->grauSaida = 2;
  vertice->saidas = malloc(sizeof(Aresta)*2);
  vertice->grauChegada = 2;
  Aresta *aresta = malloc(sizeof(Aresta));
  aresta->id = g->numeroArestas + novo->numeroArestas;
  aresta->chegada = ida;
  aresta->saida = vertice->id;
  aresta->distancia = distanciaIda;
  vertice->saidas[0] = aresta;

  novo->vertices[novo->numeroVertices] = *vertice;
  novo->arestas[novo->numeroArestas] = *aresta;
  novo->numeroVertices++;
  novo->numeroArestas++;

  aresta = malloc(sizeof(Aresta));
  aresta->id = g->numeroArestas + novo->numeroArestas;
  aresta->chegada = volta;
  aresta->saida = vertice->id;
  aresta->distancia = distanciaVolta;
  vertice->saidas[1] = aresta;
  
  novo->arestas[novo->numeroArestas] = *aresta;
  novo->numeroArestas++;
}

void DFS(Grafo *g, int vertice, int *caminho, int *tam, float *distancia)
{
  int vizinho = g->vertices[vertice].saidas[0]->chegada;
  caminho[*tam] = vertice;
  (*tam)++;
  *distancia += g->vertices[vertice].saidas[0]->distancia;
  // printf("%d ", vizinho);
  if(g->vertices[vizinho].grauSaida == 1 && g->vertices[vizinho].grauChegada == 1){
    isolaVertice(g,vertice);
    DFS(g,vizinho,caminho,tam,distancia);
  }
  else if(g->vertices[vizinho].grauSaida != 1 || g->vertices[vizinho].grauChegada != 1){
    caminho[*tam] = vizinho;
    (*tam)++;
    if(*tam > 3) isolaVertice(g,vertice);
  }
}

void DFS2(Grafo *g, int vertice, int *caminho, int *tam, float *distanciaIda, float *distanciaVolta, int proximo)
{
  int vizinho = g->vertices[vertice].saidas[proximo]->chegada;
  caminho[*tam] = vertice;
  (*tam)++;
  *distanciaIda += g->vertices[vertice].saidas[proximo]->distancia;
  // printf("%d ", vizinho);

  //tratar qnd é rua sem saida de mao dupla
  if(g->vertices[vizinho].grauSaida == 2 && g->vertices[vizinho].grauChegada == 2){
    isolaVertice(g,vertice);
    if(g->vertices[vizinho].saidas[0]->chegada == vertice){
      *distanciaVolta += g->vertices[vizinho].saidas[1]->distancia;
      DFS2(g,vizinho,caminho,tam,distanciaIda,distanciaVolta,1);
    }
    else if(g->vertices[vizinho].saidas[1]->chegada == vertice){
      *distanciaVolta += g->vertices[vizinho].saidas[0]->distancia;
      DFS2(g,vizinho,caminho,tam,distanciaIda,distanciaVolta,0);
    }
  }
  // else if(g->vertices[vizinho].grauSaida != 1 || g->vertices[vizinho].grauChegada != 1){
  //   caminho[*tam] = vizinho;
  //   (*tam)++;
  //   if(*tam > 3) isolaVertice(g,vertice);
  // }
}

void contrair(Grafo *g)
{
  int vertice, vizinho, j;
  int *caminho = malloc(sizeof(int)*g->numeroVertices);
  int tam;
  float distancia;
  Grafo *novo;
  novo = malloc(sizeof(Grafo));
  inicializaGrafoNovo(g,novo);
  // printf("--------------------------------------------------------\n");
  for(vertice=1; vertice<g->numeroVertices; vertice++){
    caminho[0] = vertice;
    tam = 1;
    // printf("INICIO: %d ", vertice);
    for(j=0; j<g->vertices[vertice].grauSaida; j++){
      vizinho = g->vertices[vertice].saidas[j]->chegada;
      if(g->vertices[vizinho].grauSaida == 1  && g->vertices[vizinho].grauChegada == 1){
        distancia = g->vertices[vertice].saidas[j]->distancia;
        // printf("%d ", vizinho);
        DFS(g,vizinho,caminho,&tam,&distancia);
        // printf(" - tam=%d ", tam);
        if(tam >= 4){
          trocaVizinho(g,vertice,vizinho,g->numeroVertices+novo->numeroVertices,distancia/2);
          criaVertice(novo,g,caminho[tam-1],distancia/2);
          break;
        }
        else{
          tam = 1;
          distancia = 0;
        }
      }
    }
    //imprimeCaminho(caminho,tam);
    // if(tam >= 4){
    //   printf("O GRAFO NOVO: \n");
    //   imprimeGrafo(novo);
    //   printf("FIM DO GRAFO NOVO\n\n");
    // }
    
  }

  printf("--------------------------------------------------------\n");
  float distanciaIda, distanciaVolta;
  int volta, proximo;
  for(vertice=1; vertice<g->numeroVertices; vertice++){
    caminho[0] = vertice;
    tam = 1;
    printf("INICIO: %d ", vertice);
    for(j=0; j<g->vertices[vertice].grauSaida; j++){
      vizinho = g->vertices[vertice].saidas[j]->chegada;
      if(g->vertices[vizinho].grauSaida == 2  && g->vertices[vizinho].grauChegada == 2){
        if(g->vertices[vizinho].saidas[0]->chegada == vertice){
          proximo = 1;
          volta = 0;
        }
        else if(g->vertices[vizinho].saidas[1]->chegada == vertice){
          proximo = 0;
          volta = 1;
        }
        else continue;
        distanciaIda = g->vertices[vertice].saidas[j]->distancia;
        distanciaVolta = g->vertices[vizinho].saidas[volta]->distancia;
        // printf("%d ", vizinho);
        DFS2(g,vizinho,caminho,&tam,&distanciaIda,&distanciaVolta,proximo);
        printf(" - tam=%d ", tam);
        if(tam >= 4){
          trocaVizinho(g,vertice,vizinho,g->numeroVertices+novo->numeroVertices,distanciaIda/2);
          trocaVizinho(g,caminho[tam-1],caminho[tam-2],g->numeroVertices+novo->numeroVertices,distanciaVolta/2);
          criaVertice2(novo,g,vertice,caminho[tam-1],distanciaVolta/2,distanciaIda/2);
          break;
        }
        else tam = 1;
      }
    }
    imprimeCaminho(caminho,tam);
    if(tam >= 4){
      printf("O GRAFO NOVO: \n");
      imprimeGrafo(novo);
      printf("FIM DO GRAFO NOVO\n\n");
    }
    
    // if(tam >=4){

    // }
    //if(tam != 0) limpar ;
  }

  printf("--------------------------------------------------------\n");
  free(caminho);
  liberaNovo(novo);
}

int main(int argc, char** argv)
{
  FILE *file_entrada = NULL, *file_contraido = NULL, *file_caminhos = NULL;
  Grafo *g = malloc(sizeof(Grafo));

  /*Verificacao dos parametros*/
  if(argc<4){
    printf("\nSintaxe: ./contrair <entrada> <contraido> <caminhos> \n");
    return 1;
  }

  /*Leitura do arquivo do grafo*/
  file_entrada = fopen(argv[1],"r");
  if(!file_entrada){
    printf("\nErro na abertura do arquivo de instancias %s\n", argv[1]);
    return 1;
  }

  if(!leInstancia(file_entrada, g)){
    printf("\nProblema na carga da instancia\n");
    fclose(file_entrada);
    libera(g);
    return 1;
  }
  fclose(file_entrada);
  
  imprimeGrafo(g);
  printf("\n\n\n");
  file_contraido = fopen(argv[2],"w");
  file_caminhos = fopen(argv[3],"w");

  contrair(g);

  imprimeGrafo(g);

  
  libera(g);

  return 1;
}