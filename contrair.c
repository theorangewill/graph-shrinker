#include "grafo.h"

int flag;

/** Imprime os caminhos contraidos no arquivo
 *
 *  file eh o arquivo de escrita.
 *  novo eh o grafo com os vertices contraidos.
 */
void imprimeCaminhoNoArquivo(FILE *file_caminhos, Contraido *novo)
{
  //Numero de caminhos
  fprintf(file_caminhos,"%d\n",novo->numeroVertices+novo->numeroVerticesDuplo);
  //Caminhos de mao unica: id 1 distancia 0.000 N v1 v2 v3... vn
  for(int j=0; j<novo->numeroVertices; j++){
    fprintf(file_caminhos,"%d 1 %.3lf %.3lf %d", novo->vertices[j].id, floor(1000*novo->vertices[j].ida->distancia*2)/1000.0, 0.0, novo->vertices[j].tamanho);
    for(int i=0; i<novo->vertices[j].tamanho; i++)
      fprintf(file_caminhos," %d", novo->vertices[j].caminho[i]);
    fprintf(file_caminhos, "\n");
  }

  //Caminhos de mao dupla: id 2 distanciaIda distanciaVolta N v1 v2 v3... vn
  for(int j=0; j<novo->numeroVerticesDuplo; j++){
    fprintf(file_caminhos,"%d 2 %.3lf %.3lf %d", novo->verticesDuplo[j].id, floor(1000*novo->verticesDuplo[j].ida->distancia*2)/1000.0, 
                                                floor(1000*novo->verticesDuplo[j].volta->distancia*2)/1000.0, novo->verticesDuplo[j].tamanho);
    for(int i=0; i<novo->verticesDuplo[j].tamanho; i++)
      fprintf(file_caminhos," %d", novo->verticesDuplo[j].caminho[i]);
    fprintf(file_caminhos, "\n");
  }
}

/** Imprime o grafo contraido no arquivo
 *
 *  file eh o arquivo de escrita.
 *  novo eh o grafo contraido.
 *  novo eh o grafo com os novos vertices.
 */
void imprimeGrafoNoArquivo(FILE *file_contraido, Grafo *g, Contraido *novo)
{ 
  //G numero de vertices,numero de arestas, grau maximo
  fprintf(file_contraido,"G %d %d %d\n",g->numeroVertices+novo->numeroVertices+novo->numeroVerticesDuplo-1, g->arestasContraidas + novo->numeroVertices + novo->numeroVerticesDuplo*2, g->grauMaximo);
  //Vertices do grafo original, para os vertices q foram contraidos, nao possuem vizinhos
  for(int i=1; i<g->numeroVertices; i++){
    fprintf(file_contraido,"N %d %.7lf %.7lf %d", g->vertices[i].id, g->vertices[i].latitude, g->vertices[i].longitude, g->vertices[i].grauSaida);
    for(int j=0; j<g->vertices[i].grauSaida; j++)
      fprintf(file_contraido," %d %.3lf", g->vertices[i].saidas[j]->chegada, floor(g->vertices[i].saidas[j]->distancia*1000.0)/1000.0);
    fprintf(file_contraido, "\n");
  }
  //Vertices novos de mao unica
  for(int i=0; i<novo->numeroVertices; i++){
    fprintf(file_contraido,"N %d %.7lf %.7lf 1", novo->vertices[i].id, novo->vertices[i].latitude, novo->vertices[i].longitude);
    fprintf(file_contraido," %d %.3lf", novo->vertices[i].ida->chegada, floor(1000*novo->vertices[i].ida->distancia)/1000.0);
    fprintf(file_contraido, "\n");
  }
  //Vertices novos de mao dupla
  for(int i=0; i<novo->numeroVerticesDuplo; i++){
    fprintf(file_contraido,"N %d %.7lf %.7lf 2", novo->verticesDuplo[i].id, novo->verticesDuplo[i].latitude, novo->verticesDuplo[i].longitude);
    fprintf(file_contraido," %d %.3lf", novo->verticesDuplo[i].ida->chegada, floor(1000*novo->verticesDuplo[i].ida->distancia)/1000.0);
    fprintf(file_contraido," %d %.3lf", novo->verticesDuplo[i].volta->chegada, floor(1000*novo->verticesDuplo[i].volta->distancia)/1000.0);
    fprintf(file_contraido, "\n");
  }
}

/** Isola um vertice no grafo, deixando ele sem vizinhos
 *
 *  g eh o grafo.
 *  vertice eh o vertice a ser isolado.
 */
void isolaVertice(Grafo *g, int vertice)
{
  //Libera a memoria do vetor de vizinhos de ida e zera o grau de entrada e saida, isolando-o do resto do grafo
  if(g->vertices[vertice].grauSaida != 0)
    free(g->vertices[vertice].saidas);
  g->arestasContraidas -= g->vertices[vertice].grauSaida;
  g->vertices[vertice].grauSaida = 0;
  g->vertices[vertice].grauChegada = 0;
}

/** Troca um vizinho de um vertice por outro
 *
 *  g eh o grafo original.
 *  novo eh o grafo com os novos vertices.
 *  vertice eh o vertice que trocara de vizinho.
 *  vizinho eh o vertice vizinho q sera retirado.
 *  novoVizinho eh o vertice que sera o novo vizinho.
 *  distancia eh a distancia do vertice para o novoVizinho.
 */
void trocaVizinho(Grafo *g, Contraido *novo, int vertice, int vizinho, int novoVizinho, double distancia)
{
  //Se o vertice eh do grafo original
  if(vertice < g->numeroVertices){
    //Procura o vizinho que sera trocado, e altera o vizinho e a distancia
    for(int v=0; v<g->vertices[vertice].grauSaida; v++){
      if(g->vertices[vertice].saidas[v]->chegada == vizinho){  
        g->vertices[vertice].saidas[v]->chegada = novoVizinho;
        g->vertices[vertice].saidas[v]->distancia = distancia;
        break;
      }
    }
  }
  //Se o vertice eh do grafo contraido, altera apenas o vizinho
  else{  
    novo->vertices[vertice-g->numeroVertices].ida->chegada = novoVizinho;
  }
}

/** Cria um novo vertice de mao unica
 *
 *  novo eh o grafo com os novos vertices.
 *  g eh o grafo original.
 *  vizinho eh o vizinho deste vertice novo.
 *  distancia eh a distancia do vertice novo para o vizinho.
 *  caminho eh o caminho que este vertice novo contrai.
 *  tamanho eh o tamanho do caminho.
 */
void criaVertice(Contraido *novo, Grafo *g, int vizinho, double distancia, int *caminho, int tamanho)
{
  int indice = novo->numeroVertices;
  int indice2 = novo->numeroArestas;

  //Inicializa o vertice novo com o id respectivo, latitude e logitude zeradas
  //Como eh de mao unica, ele tem grau de entrada e de saida 1
  novo->vertices[indice].id = g->numeroVertices + novo->numeroVertices;
  novo->vertices[indice].latitude = 0;
  novo->vertices[indice].longitude = 0;
  novo->vertices[indice].grauSaida = 1;
  novo->vertices[indice].grauChegada = 1;
  //Copia o caminho
  novo->vertices[indice].tamanho = tamanho;
  novo->vertices[indice].caminho = malloc(sizeof(int)*g->numeroVertices);
  for(int i=0; i<tamanho ; i++){
    novo->vertices[indice].caminho[i] = caminho[i];
  }
  //Cria a aresta que conecta o novo vertice ao vizinho
  novo->arestas[indice2].id = g->numeroArestas + novo->numeroArestas;
  novo->arestas[indice2].chegada = vizinho;
  novo->arestas[indice2].saida = novo->vertices[indice].id;
  novo->arestas[indice2].distancia = distancia;
  //Associa a aresta de ia à aresta criada
  novo->vertices[indice].ida = &(novo->arestas[indice2]);

  novo->numeroVertices++;
  novo->numeroArestas++;
}

/** Cria um novo vertice de mao dupla
 *
 *  novo eh o grafo com os novos vertices.
 *  g eh o grafo original.
 *  volta eh o vizinho de volta deste vertice novo.
 *  ida eh o vizinho de ida deste vertice novo.
 *  distanciaVolta eh a distancia do vertice novo para o vizinho de volta.
 *  distanciaIda eh a distancia do vertice novo para o vizinho de ida.
 *  caminho eh o caminho que este vertice novo contrai.
 *  tamanho eh o tamanho do caminho.
 */
void criaVertice2(Contraido *novo, Grafo *g, int volta, int ida, double distanciaVolta, double distanciaIda, int *caminho, int tamanho)
{
  //Inicializa o novo vertice com o id respecitov, latitude e longitude zerados
  novo->verticesDuplo[novo->numeroVerticesDuplo].id = g->numeroVertices + novo->numeroVertices + novo->numeroVerticesDuplo;
  novo->verticesDuplo[novo->numeroVerticesDuplo].latitude = 0;
  novo->verticesDuplo[novo->numeroVerticesDuplo].longitude = 0;
  //Copia o caminho
  novo->verticesDuplo[novo->numeroVerticesDuplo].tamanho = tamanho;
  novo->verticesDuplo[novo->numeroVerticesDuplo].caminho = malloc(sizeof(int)*g->numeroVertices);
  for(int i=0; i<tamanho; i++){
    novo->verticesDuplo[novo->numeroVerticesDuplo].caminho[i] = caminho[i];
  }

  //Cria a aresta que conecta a ida
  novo->arestas[novo->numeroArestas].id = g->numeroArestas + novo->numeroArestas;
  novo->arestas[novo->numeroArestas].chegada = ida;
  novo->arestas[novo->numeroArestas].saida = novo->verticesDuplo[novo->numeroVerticesDuplo].id;
  novo->arestas[novo->numeroArestas].distancia = distanciaIda;
  //Associa a aresta de ida
  novo->verticesDuplo[novo->numeroVerticesDuplo].ida = &(novo->arestas[novo->numeroArestas]);
  novo->numeroArestas++;

  //Cria a aresta que conecta a volta
  novo->arestas[novo->numeroArestas].id = g->numeroArestas + novo->numeroArestas;
  novo->arestas[novo->numeroArestas].chegada = volta;
  novo->arestas[novo->numeroArestas].saida = novo->verticesDuplo[novo->numeroVerticesDuplo].id;
  novo->arestas[novo->numeroArestas].distancia = distanciaVolta;
  //Associa a aresta de volta
  novo->verticesDuplo[novo->numeroVerticesDuplo].volta = &(novo->arestas[novo->numeroArestas]);

  novo->numeroVerticesDuplo++;
  novo->numeroArestas++;
}

/** Esta funcao substitui um vertice que ja estava contraido por outro contraido, isso ocorre quando,
  ao contrair um novo caminho e se deparar com um vertice ja contraido, quer dizer que esse vertice antigo
  deve ser deletado, enquanto o novo representa tanto o seu caminho ate o antigo quanto o caminho do proprio antigo
 *
 *  novo eh o grafo com os novos vertices.
 *  g eh o grafo original.
 *  vertice eh o vertice a ser substituido.
 *  ida eh o vizinho de ida deste vertice novo.
 *  caminho eh o caminho que este vertice novo contrai.
 *  tamanho eh o tamanho do caminho.
 *  distancia eh a distancia do vertice novo para o vizinho.
 */
void arrumaContraido(Contraido *novo, Grafo *g, int vertice, int *caminho, int tam, double distancia)
{
  int i;
  //Copia o novo caminho para o vertice antigo e atualiza a distancia
  for(i=novo->vertices[vertice].tamanho-1; i>0; i--){
    novo->vertices[vertice].caminho[tam+i-1] = novo->vertices[vertice].caminho[i];
  }
  for(i=0; i<tam; i++){
    novo->vertices[vertice].caminho[i] = caminho[i];
  }
  novo->vertices[vertice].tamanho = novo->vertices[vertice].tamanho + tam - 1;
  novo->vertices[vertice].ida->distancia += distancia;

  //Cria o novo vertice com as mesmas caracteristicas do antigo (com o novo caminho e nova distancia anteriormente atualizados)
  criaVertice(novo,g,novo->vertices[vertice].ida->chegada,novo->vertices[vertice].ida->distancia,novo->vertices[vertice].caminho,novo->vertices[vertice].tamanho);
  //Libera a memoria do caminho do antigo vertice pois ele sera deletado
  free(novo->vertices[vertice].caminho);

  //Retira o vertice antigo do vetor, deslocando os vertices em um indice, alterando o id do vertice
  int v;
  for(i=vertice; i<novo->numeroVertices-1; i++){
    novo->vertices[i] = novo->vertices[i+1];
    //Atualiza tambem o vertice que referencia o vertice em questao
    v = novo->vertices[i].caminho[0];
    for(int k=0; k<g->vertices[v].grauSaida; k++){
      if(g->vertices[v].saidas[k]->chegada == novo->vertices[i].id){
        g->vertices[v].saidas[k]->chegada = i+g->numeroVertices;
        break;
      }
    }
    novo->vertices[i].id = i+g->numeroVertices;
  }

   novo->numeroVertices--;
}

/** Esta funcao substitui um vertice que ja estava contraido por outro contraido, isso ocorre quando,
  ao contrair um novo caminho e se deparar com um vertice ja contraido, quer dizer que esse vertice antigo
  deve ser deletado, enquanto o novo representa tanto o seu caminho ate o antigo quanto o caminho do proprio antigo
 *
 *  novo eh o grafo com os novos vertices.
 *  g eh o grafo original.
 *  vertice eh o vertice a ser substituido.
 *  caminho eh o caminho que este vertice novo contrai.
 *  tamanho eh o tamanho do caminho.
 *  distanciaVolta eh a distancia do vertice novo para o vizinho de volta.
 *  distanciaIda eh a distancia do vertice novo para o vizinho de ida.
 */
void arrumaContraido2(Contraido *novo, Grafo *g, int vertice, int *caminho, int tam, double distanciaVolta, double distanciaIda)
{
  int i;
  //Copia o novo caminho para o vertice antigo e atualiza a distancia e o vertice de volta
  for(i=novo->verticesDuplo[vertice].tamanho-1; i>0; i--){
    novo->verticesDuplo[vertice].caminho[tam+i-1] = novo->verticesDuplo[vertice].caminho[i];
  }
  for(i=0; i<tam; i++){
    novo->verticesDuplo[vertice].caminho[i] = caminho[i];
  }
  novo->verticesDuplo[vertice].tamanho = novo->verticesDuplo[vertice].tamanho + tam - 1;
  novo->verticesDuplo[vertice].volta->chegada = caminho[0];
  novo->verticesDuplo[vertice].volta->distancia += distanciaVolta;
  novo->verticesDuplo[vertice].ida->distancia += distanciaIda;

  //Cria o novo vertice com as mesmas caracteristicas do antigo (com o novo caminho e nova distancia anteriormente atualizados)
  criaVertice2(novo,g,novo->verticesDuplo[vertice].volta->chegada,novo->verticesDuplo[vertice].ida->chegada,novo->verticesDuplo[vertice].volta->distancia,novo->verticesDuplo[vertice].volta->distancia,novo->verticesDuplo[vertice].caminho,novo->verticesDuplo[vertice].tamanho);
  //Libera a memoria do caminho do antigo vertice pois ele sera deletado
  free(novo->verticesDuplo[vertice].caminho);


  //Retira o vertice antigo do vetor, deslocando os vertices em um indice, alterando o id do vertice
  int v;
  for(i=vertice; i<novo->numeroVerticesDuplo-1; i++){
    novo->verticesDuplo[i] = novo->verticesDuplo[i+1];
    //Atualiza tambem os vertices que referenciam o vertice em questao
    v = novo->verticesDuplo[i].volta->chegada;
    for(int k=0; k<g->vertices[v].grauSaida; k++){
      if(g->vertices[v].saidas[k]->chegada == novo->verticesDuplo[i].id){
        g->vertices[v].saidas[k]->chegada = i+g->numeroVertices+novo->numeroVertices;
      }
    }
    v = novo->verticesDuplo[i].ida->chegada;
    for(int k=0; k<g->vertices[v].grauSaida; k++){
      if(g->vertices[v].saidas[k]->chegada == novo->verticesDuplo[i].id){
        g->vertices[v].saidas[k]->chegada = i+g->numeroVertices+novo->numeroVertices;
      }
    }
    novo->verticesDuplo[i].id = i+g->numeroVertices+novo->numeroVertices;
  }

  novo->numeroVerticesDuplo--;
}

/** Faz uma busca em profundidade em vertices de mao unica
 *
 *  g eh o grafo original.
 *  novo eh o grafo com os novos vertices.
 *  origem eh o vertice de origem do caminho.
 *  contraido eh o vertice contraido do caminho, caso ocorre do caminho ter um vertice contraido.
 *  vertice eh o vertice a ser feita a busca.
 *  caminho eh o caminho da busca.
 *  tamanho eh o tamanho do caminho.
 *  distancia eh a distancia total do caminho.
 */
void DFS(Grafo *g, Contraido *novo, int origem, int *contraido, int vertice, int *caminho, int *tam, double *distancia)
{
  //Pega o vertice vizinho
  int vizinho = g->vertices[vertice].saidas[0]->chegada;
  //Adiciona o vertice no caminho
  caminho[*tam] = vertice;
  (*tam)++;

  //Se o vertice eh do grafo original
  if(vizinho < g->numeroVertices){
    //Atualiza a distancia
    *distancia += g->vertices[vertice].saidas[0]->distancia;
    //Se o vizinho eh a origem do caminho, entao finaliza a busca, adicionando a origem novamente no caminho
    if(vizinho == origem){
      caminho[*tam] = vizinho;
      (*tam)++;
      //Se o caminho for maior que 3, o vertice eh isolado pois ele sera contraido
      if(*tam > 3) isolaVertice(g,vertice);
    }
    //Se o vizinho tem grau de saide e entrada 1, entao eh de mao unica, deve-se isola-lo e fazer uma busca nele
    else if(g->vertices[vizinho].grauSaida == 1 && g->vertices[vizinho].grauChegada == 1){
      isolaVertice(g,vertice);
      DFS(g,novo,origem,contraido,vizinho,caminho,tam,distancia);
    }
    //Caso contrario, o caminho chegou no fim
    else{
      caminho[*tam] = vizinho;
      (*tam)++;
      if(*tam > 3) isolaVertice(g,vertice);
    }
  }
  //Se o vertice eh do grafo contraido
  else{
    //Entao deve-se arrumar o vertice contraido
    arrumaContraido(novo,g,vizinho-g->numeroVertices,caminho,*tam,(*distancia)/2);
    isolaVertice(g,vertice);
    //O vertice contraido eh
    *contraido = g->numeroVertices+novo->numeroVertices - 1;
    //Atualiza a flag para avisar que foi arrumado um contraido
    flag = 0;
  }
}

/** Faz uma busca em profundidade em vertices de mao unica
 *
 *  g eh o grafo original.
 *  novo eh o grafo com os novos vertices.
 *  origem eh o vertice de origem do caminho.
 *  contraido eh o vertice contraido do caminho, caso ocorre do caminho ter um vertice contraido.
 *  vertice eh o vertice a ser feita a busca.
 *  caminho eh o caminho da busca.
 *  tamanho eh o tamanho do caminho.
 *  distanciaVolta eh a distancia do vertice novo para o vizinho de volta.
 *  distanciaIda eh a distancia do vertice novo para o vizinho de ida.
 *  proximo indica o indice no vertice.saidas[proximo] para saber qual eh o proximo vertice do caminho
 */
void DFS2(Grafo *g, Contraido *novo, int origem, int *contraido, int vertice, int *caminho, int *tam, double *distanciaIda, double *distanciaVolta, int proximo)
{
  //Pega o proximo vizinho
  int vizinho = g->vertices[vertice].saidas[proximo]->chegada;
  //Adiciona o vertice no caminho
  caminho[*tam] = vertice;
  (*tam)++;

  //Se o vizinho eh do grafo original
  if(vizinho < g->numeroVertices){
    //Se o vizinho eh a origem
    if(vizinho == origem){  
      //Busca se a origem tem retorno no vertice
      for(int i=0; i<g->vertices[origem].grauSaida; i++){
        //Se tem retorno, eh atualizado as distancias e a origem eh novamente adicionada no caminho
        if(g->vertices[origem].saidas[i]->chegada == vertice){
          *distanciaIda += g->vertices[vertice].saidas[proximo]->distancia; 
          *distanciaVolta += g->vertices[origem].saidas[i]->distancia; 
          caminho[*tam] = origem;
          (*tam)++;
          if(*tam > 3) isolaVertice(g,vertice);
          break;
        }
      }
    }
    //Se o vzinho tem grau de saide e entrada 2, ele pode fazer aprte do caminho
    else if(g->vertices[vizinho].grauSaida == 2 && g->vertices[vizinho].grauChegada == 2){
      //Se em algumas das arestas de saida do vizinho tem chegada no vertice, ele faz parte do caminho
      if(g->vertices[vizinho].saidas[0]->chegada == vertice){
        *distanciaIda += g->vertices[vertice].saidas[proximo]->distancia; 
        *distanciaVolta += g->vertices[vizinho].saidas[0]->distancia;
        DFS2(g,novo,origem,contraido,vizinho,caminho,tam,distanciaIda,distanciaVolta,1);
        if(*tam >=4) isolaVertice(g,vertice);
      }
      else if(g->vertices[vizinho].saidas[1]->chegada == vertice){
    *distanciaIda += g->vertices[vertice].saidas[proximo]->distancia; 
        *distanciaVolta += g->vertices[vizinho].saidas[1]->distancia;
        DFS2(g,novo,origem,contraido,vizinho,caminho,tam,distanciaIda,distanciaVolta,0);
        if(*tam >=4) isolaVertice(g,vertice);
      }
      //Caso contrario, nao faz parte do caminho
    }
    //Se tem um grau diferente
    else{
      //Ve se tem retorno do vizinho para o vertice
      for(int i=0; i<g->vertices[vizinho].grauSaida; i++){
        //Se tem retorno, faz parte do caminho
        if(g->vertices[vizinho].saidas[i]->chegada == vertice){
          *distanciaIda += g->vertices[vertice].saidas[proximo]->distancia; 
          caminho[*tam] = vizinho;
          (*tam)++;
          *distanciaVolta += g->vertices[vizinho].saidas[i]->distancia;
          if(*tam >=4) isolaVertice(g,vertice);
          break;
        }
      }
    }
  }
  //Se o vizinho eh um vertice contraido de mao unica
  else if(vizinho < novo->numeroVertices + g->numeroVertices){
    //Se tem retorno no vertice, ele eh adicionado no caminho
    vizinho = vizinho-g->numeroVertices;
    if(novo->vertices[vizinho].ida->chegada == vertice){
      *distanciaIda += g->vertices[vertice].saidas[proximo]->distancia; 
      *distanciaVolta += novo->vertices[vizinho].ida->distancia;
      caminho[*tam] = vizinho+g->numeroVertices;
      (*tam)++;
      if(*tam >=4) isolaVertice(g,vertice);
    }
  }
  //Se o vizinho eh um vertice contraido de mao dupla, deve-se arrumar
  else{
    //Arruma o vertice contraido
    arrumaContraido2(novo,g,vizinho-g->numeroVertices-novo->numeroVertices,caminho,*tam,(*distanciaVolta)/2,(*distanciaIda)/2);
    isolaVertice(g,vertice);
    //Arruma a referencia para ele
    int ida = novo->verticesDuplo[vizinho-g->numeroVertices-novo->numeroVertices].ida->chegada;
    for(int i=0; i<g->vertices[ida].grauSaida; i++){
      if(g->vertices[ida].saidas[i]->chegada == vizinho){
        g->vertices[ida].saidas[i]->chegada = g->numeroVertices+novo->numeroVertices+novo->numeroVerticesDuplo-1;
        g->vertices[ida].saidas[i]->distancia = novo->verticesDuplo[vizinho-g->numeroVertices-novo->numeroVertices].volta->distancia;
        break;
      }
    }
    //O vertice contraido
    *contraido = g->numeroVertices+novo->numeroVertices+novo->numeroVerticesDuplo-1;
    //Atualiza a flag
    flag = 0;
  }
}


/** Esta funcao avalia se um vertice faz parte do meio de um caminho de mao dupla
 *
 *  g eh o grafo original.
 *  vertice eh o vertice a avaliar.
 */
int meioDoCaminho(Grafo *g, Contraido *novo, int vertice)
{
  int vizinho;
  int flag2 = 0;
  //Ve se o vertice tem grau de ida e saida igual a 2
  if(g->vertices[vertice].grauSaida == 2 && g->vertices[vertice].grauChegada == 2){
    //Para cada vizinho, avalia se tem retorno para ele, caso tenha, eh meio de um caminho
    vizinho = g->vertices[vertice].saidas[0]->chegada;
    if(vizinho >= g->numeroVertices) return 0;
    for(int i=0; i<g->vertices[vizinho].grauSaida; i++){
      if(g->vertices[vizinho].saidas[i]->chegada == vertice)
        flag2 = 1;
    }
    if(flag2 == 0) return 0;
    vizinho = g->vertices[vertice].saidas[1]->chegada;
    if(vizinho >= g->numeroVertices) return 0;
    for(int i=0; i<g->vertices[vizinho].grauSaida; i++){
      if(g->vertices[vizinho].saidas[i]->chegada == vertice)
        return 1;
    }
  }
  return 0;
}


/** Faz uma busca em profundidade em vertices de mao unica
 *
 *  g eh o grafo original.
 *  file_contraido eh o arquivo para o grafo contraido.
 *  file_caminhos eh o arquivo para os caminhos contraidos.
 */
void contrair(Grafo *g, FILE *file_contraido, FILE *file_caminhos)
{
  int vertice, vizinho, j;
  int *caminho = malloc(sizeof(int)*g->numeroVertices);
  int tam;
  double distancia;
  int contraido;
  //Cria o grafo contraido
  Contraido *novo;
  novo = malloc(sizeof(Contraido));
  inicializaGrafoNovo(g,novo);
  //Inicia a busca por vertices de mao unica
  for(vertice=1; vertice<g->numeroVertices; vertice++){
    //Adiciona o vertice no caminho
    caminho[0] = vertice;
    tam = 1;
    //Para cada vizinho dele
    for(j=0; j<g->vertices[vertice].grauSaida; j++){
      vizinho = g->vertices[vertice].saidas[j]->chegada;
      //Avalia de o grau de saida e chegada eh 1, podendo ser um caminho
      if(vizinho < g->numeroVertices && g->vertices[vizinho].grauSaida == 1  && g->vertices[vizinho].grauChegada == 1){
        //Adiciona a distancia
        distancia = g->vertices[vertice].saidas[j]->distancia;
        flag = 1;
        //Faz uma busca em profundidade
        DFS(g,novo,vertice,&contraido,vizinho,caminho,&tam,&distancia);
        //Avalia se foi alterado um vertice contraido
        if(flag){
          //Se nao avalia se o tamanho do caminho eh maior que 3 e troca o vizinho e cria o vertice
          if(tam >= 4){
            trocaVizinho(g,novo,vertice,vizinho,g->numeroVertices+novo->numeroVertices,distancia/2);
            criaVertice(novo,g,caminho[tam-1],distancia/2,caminho,tam);
          }
        }
        //Se foi arrumado, deve-se trocar o vizinho para o contraido
        else{
          trocaVizinho(g,novo,vertice,vizinho,contraido,novo->vertices[contraido-g->numeroVertices].ida->distancia);  
        }
        //Reinicia o caminho
        tam = 1;
      }
    }
  }
  double distanciaIda, distanciaVolta;
  int volta, proximo;
  //Inicia a busca por vertices de mao dupla
  for(vertice=1; vertice<g->numeroVertices; vertice++){
    //Adiciona o vertice no caminho
    caminho[0] = vertice;
    tam = 1;
    //Se nao for meio do caminho
    if(!meioDoCaminho(g,novo,vertice)){
      //Para cada vizinho
      for(j=0; j<g->vertices[vertice].grauSaida; j++){
        vizinho = g->vertices[vertice].saidas[j]->chegada;
        //Avalia os graus do vizinho para um potencial caminho
        if(vizinho < g->numeroVertices && g->vertices[vizinho].grauSaida == 2  && g->vertices[vizinho].grauChegada == 2){
          //Verifica qual das saidas vai para o vertice
          if(g->vertices[vizinho].saidas[0]->chegada == vertice){
            proximo = 1;
            volta = 0;
          }
          else if(g->vertices[vizinho].saidas[1]->chegada == vertice){
            proximo = 0;
            volta = 1;
          }
          //Se nenhum for, nao eh caminho de mao dupla
          else continue;
          //Atualiza as distancias
          distanciaIda = g->vertices[vertice].saidas[j]->distancia;
          distanciaVolta = g->vertices[vizinho].saidas[volta]->distancia;
          flag = 1;
          //Faz a busca em profundidade
          DFS2(g,novo,vertice,&contraido,vizinho,caminho,&tam,&distanciaIda,&distanciaVolta,proximo);
          //Se nao foi arrumado um contraido
          if(flag){
            //Verifica o tamnho do caminho, troca os vizinhos e cria um vertice novo
            if(tam >= 4){
              trocaVizinho(g,novo,vertice,vizinho,g->numeroVertices+novo->numeroVertices+novo->numeroVerticesDuplo,distanciaIda/2);
              trocaVizinho(g,novo,caminho[tam-1],caminho[tam-2],g->numeroVertices+novo->numeroVertices+novo->numeroVerticesDuplo,distanciaVolta/2);
              criaVertice2(novo,g,vertice,caminho[tam-1],distanciaVolta/2,distanciaIda/2,caminho,tam);
            }
          }
          //Se um contraido foi arrumado
          else {
            trocaVizinho(g,novo,vertice,vizinho,contraido,novo->verticesDuplo[contraido-g->numeroVertices-novo->numeroVertices].ida->distancia);
          }
          //Reinicia o caminho
          tam = 1;
        }
      }
    }
  }
  //Imprime os caminhos e o grafo nos arquivos
  imprimeCaminhoNoArquivo(file_caminhos,novo);
  imprimeGrafoNoArquivo(file_contraido,g,novo);
  //Libera memoria
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
  
  //Abre osarquivos para escrita
  file_contraido = fopen(argv[2],"w");
  file_caminhos = fopen(argv[3],"w");

  //Contrai um grafo
  contrair(g,file_contraido,file_caminhos);

  //Libera memoria alocada
  fclose(file_contraido);
  fclose(file_caminhos);
  libera(g);

  return 1;
}