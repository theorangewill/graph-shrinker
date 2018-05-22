#include "grafo.h"


void imprimeCaminho(int *caminho, int tam)
{
  printf("CAMINHO: ");
  for(int i=0; i<tam; i++) printf("%d ", caminho[i]);
  printf("\n");
}

void DFS(Grafo *g, int vertice, int *caminho, int *tam)
{
  int vizinho = g->vertices[vertice].saidas[0]->chegada;
  caminho[*tam] = vertice;
  (*tam)++;
  printf("%d ", vizinho);
  if(g->vertices[vizinho].grauSaida == 1 && g->vertices[vizinho].grauChegada == 1)
    DFS(g,vizinho,caminho,tam);
  else if(g->vertices[vizinho].grauSaida != 1){
    caminho[*tam] = vizinho;
    (*tam)++;
  }
}


void contrair(Grafo *g)
{
  int vertice, vizinho, j;
  int *caminho = malloc(sizeof(int)*g->numeroVertices);
  int tam;

  for(vertice=1; vertice<g->numeroVertices; vertice++){
    caminho[0] = vertice;
    tam = 1;
    printf("INICIO: %d ", vertice);
    for(j=1; j<g->vertices[vertice].grauSaida; j++){
      vizinho = g->vertices[vertice].saidas[j]->chegada;
      if(g->vertices[vizinho].grauSaida == 1  && g->vertices[vizinho].grauChegada == 1){
        printf("%d ", vizinho);
        DFS(g,vizinho,caminho,&tam);
        printf(" - tam=%d ", tam);
        if(tam >= 4) break;
        else tam = 1;
      }
    }
    imprimeCaminho(caminho,tam);
    
    //if(tam != 0) limpar ;
  }

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
  
  file_contraido = fopen(argv[2],"w");
  file_caminhos = fopen(argv[3],"w");

  contrair(g);


  
  libera(g);

  return 1;
}