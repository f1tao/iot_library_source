typedef struct C_PRD *CAT_PRODUTOS;					/* Estrutura de dados de um catalogo de produtos */
typedef struct structproduto *PRODUTO;					/* Estrutura de dados para um produto */
typedef struct travessiaProdutos *LISTA;				/* Estrutura de dados que permite a travessia numa lista de produtos com a mesma letra*/


/* API CATALOGO DE PRODUTOS */

CAT_PRODUTOS inicializaProdutos();					/* Inicializa e aloca a memoria necessaria para um catalogo */

void destroiCatalogoProdutos(CAT_PRODUTOS catalogo);			/* Liberta toda a memoria relativa ao catalogo */

CAT_PRODUTOS adicionarProduto(CAT_PRODUTOS catalogo, char *produto);	/* Adiciona um produto num catalogo */
		
int existeProduto(CAT_PRODUTOS catalogo, char *produto);		/* Retorna 1 caso exista o produto, 0 caso nao exista */

LISTA produtosPorLetra(CAT_PRODUTOS catalogo, char letra);		/* Retorna a lista de produtos começados pela letra passada como parametro (alfabeticamente) */

int nrPorLetra(CAT_PRODUTOS catalogo, char letra);			/* Retorna o nr de produtos dado uma letra */

int nrItensCatalogo(CAT_PRODUTOS catalogo);				/* Nr total de produtos no catalogo */


/* API DA LISTA */

char* proximoProduto(LISTA listaProdutos);				/* Dado uma lista de produtos, retorna o proximo, ou NULL caso nao exista proximo */

char* anteriorProduto(LISTA listaProdutos);				/* Dado uma lista de produtos, retorna o anterior, ou NULL caso nao exista anterior */

void destroiTravessia(LISTA lista);							/* Liberta a estrutura de LISTA da memoria */
