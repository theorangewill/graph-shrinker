#include "grafo.h"


int main(int argc, char** argv)
{
  FILE* file = NULL;
  Grafo *g = malloc(sizeof(Grafo));

  /*Verificacao dos parametros*/
  if(argc<2){
    printf("\nSintaxe: grasp_heur <in> \n");
    return 1;
  }

  /*Leitura do arquivo do grafo*/
  file = fopen(argv[1],"r");
  if(!file){
    printf("\nErro na abertura do arquivo de instancias %s\n", argv[1]);
    return 1;
  }

  if(!leInstancia(file, g)){
    printf("\nProblema na carga da instancia\n");
    fclose(file);
    libera(g);
    return 1;
  }
  fclose(file);


  imprimeGrafo(g);
  libera(g);

  return 1;
}