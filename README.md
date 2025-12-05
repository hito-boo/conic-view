# Visualização Cônica - Projeção Perspectiva
Trabalho desenvolvido na disciplina de Computação Gráfica, durante a graduação em Ciência da Computação na Universidade Estadual de Maringá. O código realiza uma projeção cônica a partir da definição do objeto tridimensional, do plano de projeção e das coordenadas do observador.

# Entradas e Saídas
A entrada é processada por meio da execução do programa principal deste repositório e diz respeito aos elementos essenciais para a realização da projeção perspectiva. Em um primeiro momento, é necessário definir um objeto tridimensional, portanto as seguintes características são solicitadas:

* Número de vértices
  - Número inteiro.
* Coordenadas X, Y e Z de cada vértice
  - Três números inteiros.
* Número de faces
  - Número inteiro.
* Número de vértices que definem cada face
  - Número inteiro.
* Sequência dos números dos vértices de cada face
  - Números inteiros.
  - A sequência deve criar uma face fechada. Por exemplo: a face retangular definida pelos vértices 3, 6, 2 e 1 pode ser desenhada seguindo a mesma ordem (3 → 6 → 2 → 1 → 3).

Após a correta definição do objeto tridimensional, será necessário caracterizar o plano de projeção. Esse processo é realizado por meio da solicitação de três pontos (que devem ser distintos e não colineares):

* Três pontos com coordenadas X, Y e Z
  - Três números inteiros para cada ponto.

Por fim, uma vez que o plano de projeção é definido, há a solicitação das coordenadas que definem o ponto de vista, isto é, o observador:

* Coordenadas X, Y e Z do observador
  - Três números inteiros.

Após todas essas entradas serem definidas, uma sequência de operações é executada de forma a realizar os cálculos algébricos necessários para definir a projeção do objeto tridimensional no plano de acordo com o observador. A projeção é salva em um arquivo de saída nomeado como "resultado.svg", o qual é gerado no diretório do arquivo principal após a execução.