#include "./contabilidade.h"
#include <string.h>
#include <stdlib.h>
#include "./avl.h"


/*-----------------------STRUCTS----------------------*/

typedef char *strProduto;
/* Estrutura com array indexado por meses */
typedef struct contabilidade{
	struct avl_table *produtos;
}*CONTABILIDADE;

/* Nodo */
typedef struct registo{
	strProduto produto;
	int valores[12][3];
	double faturacao[12][2];
}*Registo;

typedef struct listaContabilidade{
	struct avl_traverser *travessia;
}*ListaContabilidade;

/* Função para comparar dois nodos */
int compare_contabilidade(const void *nodo1,const void *nodo2, void *avl_param){
	
	Registo n1, n2;
	n1 = (Registo)nodo1;
	n2 = (Registo)nodo2;

	return(strcmp(n1->produto,n2->produto));
}

void item_func_contabilidade(void *avl_item, void *avl_param){

	Registo n1 = (Registo)avl_item;
	free(n1->produto);
	free(n1);
}

CONTABILIDADE inicializa_contabilidade(){

	CONTABILIDADE nova_contabilidade = (CONTABILIDADE)malloc(sizeof(struct contabilidade));

	nova_contabilidade->produtos = avl_create(&compare_contabilidade, NULL, NULL);

	return nova_contabilidade;

}

void destroiCatalogoContabilidade(CONTABILIDADE contabilidade){

	if(!contabilidade) return;

	avl_destroy(contabilidade->produtos, &item_func_contabilidade);
	free(contabilidade);
}

void adicionarRegisto(CONTABILIDADE contabilidade, char *produto){

	if(!produto || !contabilidade) return;

	Registo aux;
	int i,j;
	aux = (Registo)malloc(sizeof(struct registo));
	aux->produto = (char *)malloc(sizeof(char) * strlen(produto) + 1);
	strcpy(aux->produto, produto);
	for(i=0; i < 12; i++){
		for(j=0;j<3;j++){
			aux->valores[i][j] = 0;
		}
		for(j=0;j<2;j++) aux->faturacao[i][j] = 0;
	}
	
	avl_insert(contabilidade->produtos, aux);

}

/* Faz update ao registo, caso nao exista retorna null, caso exista retorna-o */
Registo updateRegisto(CONTABILIDADE contabilidade, char *produto, int vendasN, double fatN, int vendasP, double fatP, int mes){
	Registo aProcurar,encontrado;

	if(!contabilidade || mes<1 || mes >12 || fatN <0 || fatP<0 || vendasN<0 || vendasP<0 || !produto) return;

	aProcurar = (Registo)malloc(sizeof(struct registo));
	aProcurar->produto =(char *)malloc(sizeof(char) * strlen(produto) + 1);
	strcpy(aProcurar->produto,produto);
	encontrado = avl_find(contabilidade->produtos, aProcurar);

	if(encontrado){
		encontrado->valores[mes-1][0] += vendasN;
		encontrado->faturacao[mes-1][0] += vendasN * fatN;
		encontrado->valores[mes-1][1] += vendasP;
		encontrado->faturacao[mes-1][1] += vendasP * fatP;
		encontrado->valores[mes-1][2] ++;
		free(aProcurar->produto);
		free(aProcurar);
	}
	else return NULL;
	return encontrado;
}

Registo encontra_registo(CONTABILIDADE contabilidade, char* prd){
	Registo registoaProcurar = (Registo)malloc(sizeof(struct registo));
	Registo encontrado;

	registoaProcurar->produto = (char *)malloc(sizeof(char) * strlen(prd) + 1);
	strcpy(registoaProcurar->produto,prd);

	encontrado = avl_find(contabilidade->produtos, registoaProcurar);
	
	return encontrado;
}

int daVendasN(Registo reg, int mes){
	return reg->valores[mes-1][0];
}

int daVendasP(Registo reg, int mes){
	if(!reg) return 0;
	return reg->valores[mes-1][1];
}

double daFaturacaoTotal(Registo reg, int mes){
	if(!reg) return 0;
	return (reg->faturacao[mes-1][0] + reg->faturacao[mes-1][1]);
}

int nrVendas(CONTABILIDADE contabilidade, int mes){
	int num_vendas=0, indice_mes = mes-1,i;
	if(mes<1 || mes > 12 || !contabilidade) return 0;
	struct avl_traverser* travessia = (struct avl_traverser*) malloc(sizeof(struct avl_traverser));
	Registo seguinte;

	avl_t_init(travessia, contabilidade->produtos);
	for(seguinte=avl_t_next(travessia); seguinte; seguinte=avl_t_next(travessia)){	
			num_vendas += seguinte->valores[indice_mes][2];
	}

	free(travessia);
	return num_vendas;	
}

double totalFaturado(CONTABILIDADE contabilidade, int mes){
	double total_faturado=0;
	int indice_mes = mes-1,i;
	if(mes<1 || mes > 12 || !contabilidade) return 0;
	struct avl_traverser* travessia = (struct avl_traverser*) malloc(sizeof(struct avl_traverser));
	Registo seguinte;

	avl_t_init(travessia, contabilidade->produtos);
	for(seguinte=avl_t_next(travessia); seguinte; seguinte=avl_t_next(travessia)){
		total_faturado += daFaturacaoTotal(seguinte, indice_mes+1);
	}
	
	free(travessia);
	return total_faturado;
}

ListaContabilidade produtosNaoComprados(CONTABILIDADE contabilidade){
	if(!contabilidade) return NULL;

	int indiceMes=0, comprado;
	struct avl_table *produtosNaoEncontrados = (struct avl_table *)malloc(sizeof(struct avl_table));
	ListaContabilidade lista = (ListaContabilidade)malloc(sizeof(struct listaContabilidade));
	lista->travessia = (struct avl_traverser *)malloc(sizeof(struct avl_traverser));
	
	struct avl_traverser* travessia = (struct avl_traverser*) malloc(sizeof(struct avl_traverser));
	Registo seguinte,aux;

	produtosNaoEncontrados = avl_create(&compare_contabilidade, NULL, NULL);

	avl_t_init(travessia, contabilidade->produtos);
	for(seguinte=avl_t_next(travessia); seguinte; seguinte=avl_t_next(travessia)){
		comprado = 0;
		for(indiceMes=0; indiceMes<12;indiceMes++){
			comprado += seguinte->valores[indiceMes][2];
		}
		if(comprado == 0){
			aux = (Registo)malloc(sizeof(struct registo));
			aux->produto = (char *)malloc(sizeof(char) * strlen(seguinte->produto) + 1);
			strcpy(aux->produto, seguinte->produto);
			avl_insert(produtosNaoEncontrados, aux);
		}
	}
	
        free(travessia);
	
	avl_t_init(lista->travessia, produtosNaoEncontrados);
	return lista;
}

int daNrCodigos(ListaContabilidade lista){
	if(!lista)return 0;
	return(lista->travessia->avl_table->avl_count);
}

char *proxCodigo(ListaContabilidade lista){
	if(!lista)return NULL;
	Registo aux;
	aux = avl_t_next(lista->travessia);
	if(aux)
		return(aux->produto);
	return NULL;
}

char *anteriorCodigo(ListaContabilidade lista){
	if(!lista) return NULL;
	Registo anterior;
	anterior = avl_t_prev(lista->travessia);
	return anterior->produto;
}

void destroiLista(ListaContabilidade lista){
	if(!lista) return;
	avl_destroy(lista->travessia->avl_table, &item_func_contabilidade);
	free(lista);
}
