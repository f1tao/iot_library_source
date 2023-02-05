#include "./produtos.h"
#include "./avl.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* STRUCTS */

typedef char *strProduto;
typedef struct travessiaProdutos{
	struct avl_traverser *traverser;
}*LISTA;

typedef struct structproduto {
	strProduto produto;
}*Produto;

typedef struct C_PRD {
	struct avl_table *arrayAZ[26]; /* Array de apontadores para avl_table's de A-Z */
}*CAT_PRODUTOS;


								/* API TRAVESSIA */

LISTA inicializaTravessia(CAT_PRODUTOS catalogo, char letra){

	int indice = letra - 'A';

	struct avl_traverser* travessia  = (struct avl_traverser*) malloc(sizeof(struct avl_traverser));
	avl_t_init (travessia, catalogo->arrayAZ[indice]);

	LISTA novalista = (LISTA)malloc(sizeof(struct travessiaProdutos));
	novalista->traverser = travessia;

	return novalista;	
}

char* proximoProduto(LISTA listaProdutos){
	Produto seguinte = avl_t_next (listaProdutos->traverser);
	char *produto;

	if(!seguinte) return NULL;
	return (seguinte->produto);		
}

char* anteriorProduto(LISTA listaProdutos){
	Produto anterior = avl_t_prev (listaProdutos->traverser);
	char *produto;

	if(!anterior) return NULL;
	return (anterior->produto);	
}

void destroiTravessia(LISTA lista){
	free(lista->traverser);
	free(lista);
}


								/* API CATALOGO_PRODUTOS */

int compare_produtos(const void *nodo1,const void *nodo2, void *avl_param){

	Produto n1,n2;
	n1 = (Produto)nodo1;
	n2 = (Produto)nodo2;
	
	return(strcmp(n1->produto,n2->produto));
}

void item_func_produtos(void *avl_item, void *avl_param){
	Produto aux = (Produto)avl_item;
	free(aux->produto);
	free(aux);
}

CAT_PRODUTOS inicializaProdutos(){

	int i;
	CAT_PRODUTOS novocatalogo = (CAT_PRODUTOS)malloc(sizeof(struct C_PRD));
	avl_comparison_func *myComp = compare_produtos;

	for(i=0; i < 26; i++)
		novocatalogo->arrayAZ[i] = avl_create(myComp, NULL, NULL);	
	
	return novocatalogo;
}

LISTA produtosPorLetra(CAT_PRODUTOS catalogo, char l){
	
	LISTA lista;
	char letra = toupper(l);					/* Muda para letra maiuscula, se ja maiucula mantem maiuscula */
	int num_prod = nrPorLetra(catalogo,letra);			/* Quantidade produtos começados pela 'letra' */
	if(num_prod == 0) return NULL;
	lista = inicializaTravessia(catalogo,letra);

	return lista;
}

CAT_PRODUTOS adicionarProduto(CAT_PRODUTOS catalogo, char *produto){

	if(!produto) return NULL;
	if(!(isupper(produto[0]))) return NULL;

	int indice = produto[0] - 'A';
	if(indice<0 || indice > 25 || !strlen(produto)) return NULL;
	Produto novoProduto = (Produto)malloc(sizeof(struct structproduto));
	novoProduto->produto = (char *)malloc(sizeof(char) * strlen(produto) + 1);
	
	strcpy(novoProduto->produto,produto);

	novoProduto = avl_insert(catalogo->arrayAZ[indice], novoProduto);
	if(novoProduto == NULL){	/* Inseriu com sucesso */
		return catalogo;
	}
	else{
		free(novoProduto->produto);free(novoProduto);
		return NULL;
	}
}

int nrPorLetra(CAT_PRODUTOS catalogo, char letra){
	char minhaletra = toupper(letra);
	if(!(isupper(minhaletra))) return 0;
	int indice = letra - 'A';
	
	return (catalogo->arrayAZ[indice]->avl_count);
}

int existeProduto(CAT_PRODUTOS catalogo, char *produto){

	int indice = produto[0] - 'A';
	Produto aux = (Produto)malloc(sizeof(struct structproduto));
	aux->produto = (char *)malloc(sizeof(char) * strlen(produto) + 1);
	strcpy(aux->produto,produto);

	if( avl_find(catalogo->arrayAZ[indice], aux) ){
		free(aux->produto);	
		free(aux);
		return 1;
	}

	free(aux->produto);
	free(aux);
	return 0;
}

int nrItensCatalogo(CAT_PRODUTOS catalogo){
	int nritens,i;
	
	for(i=0;i<26;i++)
		nritens = nritens + (catalogo->arrayAZ[i])->avl_count;
	return (nritens);	
}

void destroiCatalogoProdutos(CAT_PRODUTOS catalogo){
	int i;
	avl_item_func *myItemFunc = item_func_produtos;	
	for(i=0;i<26;i++)
		avl_destroy(catalogo->arrayAZ[i], myItemFunc);
	free(catalogo);
}
