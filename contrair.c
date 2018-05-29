#include "grafo.h"


void imprimeCaminho(int *caminho, int tam)
{
  printf("CAMINHO: ");
  for(int i=0; i<tam; i++) printf("%d ", caminho[i]);
  printf("\n");
}

void imprimeCaminhoNoArquivo(FILE *file_caminhos, int *caminho, int tam, int totalCaminhos, float distancia)
{
  fprintf(file_caminhos,"p%d 1 %f 0 %d", totalCaminhos, distancia, tam);
  for(int i=0; i<tam; i++)
    fprintf(file_caminhos," %d",caminho[i]);
  fprintf(file_caminhos, "\n");
}

void imprimeCaminhoDuploNoArquivo(FILE *file_caminhos, int *caminho, int tam, int totalCaminhos, float distanciaIda, float distanciaVolta)
{
  fprintf(file_caminhos,"p%d 2 %f %f %d", totalCaminhos, distanciaIda, distanciaVolta, tam);
  for(int i=0; i<tam; i++)
    fprintf(file_caminhos," %d",caminho[i]);
  fprintf(file_caminhos, "\n");
}

void imprimeGrafoNoArquivo(FILE *file_contraido, Grafo *g, Grafo *novo)
{
  //TODO tem q arrumar a quantidade de arestas e o de grau maximo
  fprintf(file_contraido,"G %d %d %d\n",g->numeroVertices+novo->numeroVertices-1, g->numeroArestas+novo->numeroArestas, g->grauMaximo);
  for(int i=1; i<g->numeroVertices; i++){
    fprintf(file_contraido,"N %d %f %f %d", g->vertices[i].id, g->vertices[i].latitude, g->vertices[i].longitude, g->vertices[i].grauSaida);
    for(int j=0; j<g->vertices[i].grauSaida; j++)
      fprintf(file_contraido," %d %f", g->vertices[i].saidas[j]->chegada, g->vertices[i].saidas[j]->distancia);
    fprintf(file_contraido, "\n");
  }
  for(int i=0; i<novo->numeroVertices; i++){
    fprintf(file_contraido,"N %d %f %f %d", novo->vertices[i].id, novo->vertices[i].latitude, novo->vertices[i].longitude, novo->vertices[i].grauSaida);
    for(int j=0; j<novo->vertices[i].grauSaida; j++)
      fprintf(file_contraido," %d %f", novo->vertices[i].saidas[j]->chegada, novo->vertices[i].saidas[j]->distancia);
    fprintf(file_contraido, "\n");
  }
}

void retirarVizinho(Grafo *g, int vertice, int vizinho)
{
  int shift = 0;
  for(int i=0; i<g->vertices[vertice].grauSaida-1; i++){
    if(g->vertices[vertice].saidas[i]->chegada == vizinho) shift=1;
    g->vertices[vertice].saidas[i] = g->vertices[vertice].saidas[i+shift];
  }
  g->vertices[vertice].grauSaida--;
}

void isolaVertice(Grafo *g, int vertice)
{
  if(g->vertices[vertice].grauSaida != 0)
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

void DFS(Grafo *g, int origem, int vertice, int *caminho, int *tam, float *distancia)
{
  int vizinho = g->vertices[vertice].saidas[0]->chegada;
  caminho[*tam] = vertice;
  (*tam)++;
  *distancia += g->vertices[vertice].saidas[0]->distancia;
  // printf("%d ", vizinho);
  printf("%d %d %d\n", vertice, vizinho, g->numeroVertices);
  if(vizinho == origem){
    caminho[*tam] = vizinho;
    (*tam)++;
    if(*tam > 3) isolaVertice(g,vertice);
  }
  else if(g->vertices[vizinho].grauSaida == 1 && g->vertices[vizinho].grauChegada == 1){
    isolaVertice(g,vertice);
    DFS(g,origem,vizinho,caminho,tam,distancia);
  }
  else if(g->vertices[vizinho].grauSaida != 1 || g->vertices[vizinho].grauChegada != 1){
    caminho[*tam] = vizinho;
    (*tam)++;
    if(*tam > 3) isolaVertice(g,vertice);
  }
}

void DFS2(Grafo *g, int origem, int vertice, int *caminho, int *tam, float *distanciaIda, float *distanciaVolta, int proximo)
{
  int vizinho = g->vertices[vertice].saidas[proximo]->chegada;
  caminho[*tam] = vertice;
  (*tam)++;
  *distanciaIda += g->vertices[vertice].saidas[proximo]->distancia;
  // printf("%d ", vizinho);

  //tratar qnd é rua sem saida de mao dupla
  //nao pode isolar antes (tamanho 3)
  if(vizinho == origem){
    caminho[*tam] = vizinho;
    (*tam)++;
    if(*tam > 3) isolaVertice(g,vertice);
  }
  else if(g->vertices[vizinho].grauSaida == 2 && g->vertices[vizinho].grauChegada == 2){
    if(g->vertices[vizinho].saidas[0]->chegada == vertice){
      *distanciaVolta += g->vertices[vizinho].saidas[1]->distancia;
      DFS2(g,origem,vizinho,caminho,tam,distanciaIda,distanciaVolta,1);
      if(*tam >=4) isolaVertice(g,vertice);
    }
    else if(g->vertices[vizinho].saidas[1]->chegada == vertice){
      *distanciaVolta += g->vertices[vizinho].saidas[0]->distancia;
      DFS2(g,origem,vizinho,caminho,tam,distanciaIda,distanciaVolta,0);
      if(*tam >=4) isolaVertice(g,vertice);
    }
  }
  else{
    for(int i=0; i<g->vertices[vizinho].grauSaida; i++){
      if(g->vertices[vizinho].saidas[i]->chegada == vertice){
        caminho[*tam] = vizinho;
        (*tam)++;
        *distanciaVolta += g->vertices[vizinho].saidas[i]->distancia;
        if(*tam >=4) isolaVertice(g,vertice);
        break;
      }
    }
  }
  // else if(g->vertices[vizinho].grauSaida != 1 || g->vertices[vizinho].grauChegada != 1){
  //   caminho[*tam] = vizinho;
  //   (*tam)++;
  //   if(*tam > 3) isolaVertice(g,vertice);
  // }
}

void contrair(Grafo *g, FILE *file_contraido, FILE *file_caminhos)
{
  int vertice, vizinho, j;
  int *caminho = malloc(sizeof(int)*g->numeroVertices);
  int tam, totalCaminhos = 1;
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
        DFS(g,vertice,vizinho,caminho,&tam,&distancia);
        // printf(" - tam=%d ", tam);
        if(tam >= 4){
          trocaVizinho(g,vertice,vizinho,g->numeroVertices+novo->numeroVertices,distancia/2);
          criaVertice(novo,g,caminho[tam-1],distancia/2);
          imprimeCaminhoNoArquivo(file_caminhos,caminho,tam,totalCaminhos,distancia/2);
          totalCaminhos++;
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

  //printf("--------------------------------------------------------\n");
  float distanciaIda, distanciaVolta;
  int volta, proximo;
  for(vertice=1; vertice<g->numeroVertices; vertice++){
    caminho[0] = vertice;
    tam = 1;
    //printf("INICIO: %d ", vertice);
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
        DFS2(g,vertice,vizinho,caminho,&tam,&distanciaIda,&distanciaVolta,proximo);
        //printf(" - tam=%d ", tam);
        if(tam >= 4){
          if(vertice != caminho[tam-1]){
            trocaVizinho(g,vertice,vizinho,g->numeroVertices+novo->numeroVertices,distanciaIda/2);
            trocaVizinho(g,caminho[tam-1],caminho[tam-2],g->numeroVertices+novo->numeroVertices,distanciaVolta/2);
            criaVertice2(novo,g,vertice,caminho[tam-1],distanciaVolta/2,distanciaIda/2);
          }
          else{
            retirarVizinho(g,vertice,caminho[tam-2]);
            trocaVizinho(g,vertice,vizinho,g->numeroVertices+novo->numeroVertices,distanciaIda/2+distanciaVolta/2);
            criaVertice(novo,g,caminho[tam-1],distanciaVolta/2+distanciaIda/2);
          }
          imprimeCaminhoDuploNoArquivo(file_caminhos,caminho,tam,totalCaminhos,distanciaIda/2,distanciaVolta/2);
          totalCaminhos++;
          break;
        }
        else tam = 1;
      }
    }
    //imprimeCaminho(caminho,tam);
    /*if(tam >= 4){
      printf("O GRAFO NOVO: \n");
      imprimeGrafo(novo);
      printf("FIM DO GRAFO NOVO\n\n");
    }*/
    
    // if(tam >=4){

    // }
    //if(tam != 0) limpar ;
  }
  //printf("--------------------------------------------------------\n");

  imprimeGrafoNoArquivo(file_contraido,g,novo);
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
  
  //imprimeGrafo(g);
  //printf("\n\n\n");
  file_contraido = fopen(argv[2],"w");
  file_caminhos = fopen(argv[3],"w");

  contrair(g,file_contraido,file_caminhos);

  //imprimeGrafo(g);

  libera(g);

  return 1;
}