#include "./compras.h"
#include "./avl.h"
#include <stdlib.h>
#include <string.h>

/* Compra */
typedef char *strCliente;
typedef char *strProduto;
typedef struct compra{
	strProduto produto;
	int vendasN,vendasP,vendas;
	int *meses;
}*Compra;

typedef struct cliente{
	strCliente cliente;
	struct avl_table *produtos;
}*Cliente;

typedef struct catalogocompras{
	struct avl_table *compras;
}*CatalogoCompras;

typedef struct lista{
	struct avl_traverser *travessia;
}*Lista, *ListaClientes, *ListaProdutos, *ListaTotal, **DuplaLista;



int compare_compras_clientes(const void *nodo1,const void *nodo2, void *avl_param){

	Cliente n1,n2;
	n1 = (Cliente)nodo1;
	n2 = (Cliente)nodo2;

	return(strcmp(n1->cliente,n2->cliente));
}

int compare_especial(const void *nodo1,const void *nodo2, void *avl_param){

	Compra n1,n2;
	n1 = (Compra)nodo1;
	n2 = (Compra)nodo2;
	int r1,r2;
	r1 = n1->vendasN + n1->vendasP;
	r2 = n2->vendasN + n2->vendasP;

	if(r1<r2) return 1;
	if(r2<r1) return -1;
	return(strcmp(n1->produto,n2->produto));
}
int compare_compras_produtos(const void *nodo1,const void *nodo2, void *avl_param){

	Compra n1,n2;
	n1 = (Compra)nodo1;
	n2 = (Compra)nodo2;

	return(strcmp(n1->produto,n2->produto));
}

void item_func_compra(void *avl_item, void *avl_param){
	Compra aux = (Compra)avl_item;

	free(aux->produto);
	if(aux->meses)	free(aux->meses);
	free(aux);
}
void item_func_cliente(void *avl_item, void *avl_param){

	Cliente aux = (Cliente)avl_item;
	free(aux->cliente);
	if(aux->produtos)
		avl_destroy(aux->produtos, &item_func_compra);
	free(aux);
}

/* Aloca memoria pra estrutura compras */
CatalogoCompras inicializaCompras(){

	int i,j;
	CatalogoCompras novoCatalogo = (CatalogoCompras)malloc(sizeof(struct catalogocompras));
	novoCatalogo->compras = avl_create(&compare_compras_clientes,NULL,NULL);

	return novoCatalogo;
}

void destroiCatalogoCompras(CatalogoCompras compras){
	if(!compras) return;
	avl_destroy(compras->compras, &item_func_cliente);
	free(compras);
}

/* Insere uma compra nas compras */
CatalogoCompras insereCompraCatalogo(CatalogoCompras compras, char* cliente, char* produto, int mes, int quantidade, char promocao, double preco){

	if(!compras || !cliente || !produto || mes < 1 || mes > 12) return NULL;

	int i;
	Cliente aux,avlCliente;
	Compra compra,aux2;

	avlCliente = (Cliente)malloc(sizeof(struct cliente));
	avlCliente->cliente = (char *)malloc(sizeof(char) * strlen(cliente) + 1);
	strcpy(avlCliente->cliente,cliente);
	aux = avl_find(compras->compras, avlCliente);

	if(aux == NULL){/*Ainda nao existe cliente*/
		avlCliente->produtos = avl_create(&compare_compras_produtos,NULL,NULL);

		compra = (Compra)malloc(sizeof(struct compra));
		compra->produto = (char *)malloc(sizeof(char) * strlen(produto) + 1);
		compra->vendas = 1;
		strcpy(compra->produto,produto);
		if(promocao == 'P'){
			compra->vendasP = quantidade;
			compra->vendasN = 0;
		}
		else{
			compra->vendasN = quantidade;
			compra->vendasP = 0;
		}
		compra->meses = (int*)malloc(sizeof(int)*12);
		for(i=0;i<12;i++) compra->meses[i]=0;
		compra->meses[mes-1] = quantidade;

		avl_insert(avlCliente->produtos, compra);

		avl_insert(compras->compras, avlCliente);
	}
	else{/*Existe cliente*/
		compra = (Compra)malloc(sizeof(struct compra));
		compra->produto = (char *)malloc(sizeof(char) * strlen(produto) + 1);
		strcpy(compra->produto,produto);

		aux2 = avl_find(aux->produtos, compra);

		if(aux2 == NULL){/*Nao existe produto*/
			if(promocao == 'P'){
				compra->vendasP = quantidade;
				compra->vendasN = 0;
			}
			else{
				compra->vendasN = quantidade;
				compra->vendasP = 0;
			}
			compra->meses = (int*)malloc(sizeof(int)*12);
			for(i=0;i<12;i++) compra->meses[i]=0;
			compra->meses[mes-1] = quantidade;
			compra->vendas = 1;
			avl_insert(aux->produtos,compra);
		}
		else{/*Ja existe produto*/
			if(promocao == 'P') aux2->vendasP += quantidade;
			else aux2->vendasN += quantidade;
			aux2->meses[mes-1] += quantidade;
			aux2->vendas++;

			free(compra->produto);free(compra);
		}

		free(avlCliente->cliente);free(avlCliente);
	}
	return compras;
}

int produtosCompradosClienteMes(CatalogoCompras compras,char* cliente, int mes){
	if(!compras || !cliente || mes<1 || mes>12) return 0;

	int resultado=0;
	Cliente aux, procura;
	Compra seguinte;
	procura = (Cliente)malloc(sizeof(struct cliente));
	procura->cliente = (char *)malloc(sizeof(char) * strlen(cliente) + 1);
	strcpy(procura->cliente,cliente);

	aux = avl_find(compras->compras,procura);	/* Verifica se o cliente existe */
	if(aux){
		struct avl_traverser* travessia  = (struct avl_traverser*) malloc(sizeof(struct avl_traverser));
		avl_t_init(travessia, aux->produtos);
		for(seguinte = avl_t_next(travessia);seguinte;seguinte = avl_t_next(travessia)){
			resultado += seguinte->meses[mes-1];
		}
	}
	else return -1;
	return resultado;
}

int daNrEltosLista(Lista lista){
	if(!lista)return 0;
	return(lista->travessia->avl_table->avl_count);
}

int daQtdProduto(Lista lista, int mes){
	if(!lista || mes < -1 || mes > 12)return 0;
	int qtd=0;
	if(lista->travessia->avl_node->avl_data){
		if(mes == 0)
			qtd = ((Compra)lista->travessia->avl_node->avl_data)->vendasN + ((Compra)lista->travessia->avl_node->avl_data)->vendasP;
		else
			qtd = ((Compra)lista->travessia->avl_node->avl_data)->meses[mes-1] ;
		return qtd;
	}
	return 0;
}

void destruirListaClientes(ListaClientes lista){
	if(!lista) return;
	avl_destroy(lista->travessia->avl_table, &item_func_cliente);
	free(lista);
}

void destruirListaProdutos(ListaProdutos lista){
	if(!lista) return;
	avl_destroy(lista->travessia->avl_table,&item_func_compra);
	free(lista);
}

void destruirListaTotal(ListaTotal lista){
	if(!lista) return;
	avl_destroy(lista->travessia->avl_table, &item_func_compra);
	free(lista);

}
void destruirDuplaLista(DuplaLista duplalista){
	free(duplalista);
}

Lista duplaListatoLista(DuplaLista lista, char promo){
	if(!lista) return NULL;
	if(promo == 'P') return lista[1];
	else if(promo == 'N') return lista[0];
	return NULL;
}

char *proxEltoListaProdutos(Lista lista){
	if(!lista) return NULL;

	Compra seguinte = avl_t_next(lista->travessia);

	if(!seguinte) return NULL;
	return (seguinte->produto);
}

char *proxEltoListaClientes(Lista lista){
	if(!lista) return NULL;

	Cliente seguinte = avl_t_next(lista->travessia);

	if(!seguinte) return NULL;
	return (seguinte->cliente);
}

char *anteriorEltoListaProdutos(Lista lista){

        if(!lista) return NULL;

        Compra anterior = avl_t_prev(lista->travessia);

        if(!anterior) return NULL;
        return (anterior->produto);
}

char *anteriorEltoListaClientes(Lista lista){

        if(!lista) return NULL;

        Cliente anterior = avl_t_prev(lista->travessia);

        if(!anterior) return NULL;
        return (anterior->cliente);
}

DuplaLista prodsCompradosPorCliente(CatalogoCompras compras, char *prod){
	if(!prod || !compras) return NULL;

	int i;
	DuplaLista lista = (DuplaLista)malloc(sizeof(Lista)*2);
	struct avl_table *listaN,*listaP;
	struct avl_traverser* travessia  = (struct avl_traverser*) malloc(sizeof(struct avl_traverser));
	Cliente seguinte,novo;
	Compra procura,aux;

	procura = (Compra)malloc(sizeof(struct compra));
	procura->produto = (char *)malloc(sizeof(char) * strlen(prod) + 1);
	strcpy(procura->produto,prod);
	listaN = avl_create(&compare_compras_clientes, NULL, NULL);
	listaP = avl_create(&compare_compras_clientes, NULL, NULL);
	for(i=0;i<2;i++){
		lista[i] = (Lista)malloc(sizeof(struct lista));
		lista[i]->travessia = (struct avl_traverser *)malloc(sizeof(struct avl_traverser));
	}

	avl_t_init(travessia,compras->compras);
	for(seguinte=avl_t_next(travessia);seguinte;seguinte=avl_t_next(travessia)){
		aux = avl_find(seguinte->produtos, procura);
		if(aux){
			if(aux->vendasP != 0){
				novo = (Cliente)malloc(sizeof(struct cliente));
				novo->cliente = (char *)malloc(sizeof(char) * strlen(seguinte->cliente) + 1);
				strcpy(novo->cliente, seguinte->cliente);
				novo->produtos = NULL;
				avl_insert(listaP, novo);
			}
			if(aux->vendasN != 0){
				novo = (Cliente)malloc(sizeof(struct cliente));
				novo->cliente = (char *)malloc(sizeof(char) * strlen(seguinte->cliente) + 1);
				strcpy(novo->cliente, seguinte->cliente);
				novo->produtos = NULL;
				avl_insert(listaN, novo);
			}
		}
	}
	free(travessia);
	avl_t_init(lista[0]->travessia, listaN);
	avl_t_init(lista[1]->travessia, listaP);

	return lista;
}

ListaProdutos produtosMaisCompradosClienteMes(CatalogoCompras cat_compras, char* cliente, int mes){
	if(!cat_compras || !cliente || mes < 1 || mes > 12) return NULL;

	int quantidade;
	Cliente aux, procurar = (Cliente)malloc(sizeof(struct cliente));
	procurar->cliente = (char *)malloc(sizeof(char) * strlen(cliente) + 1);
	strcpy(procurar->cliente,cliente);
	aux = avl_find(cat_compras->compras,procurar);
	if(!aux) return NULL;

	Lista lista = (Lista)malloc(sizeof(struct lista));
	lista->travessia = (struct avl_traverser *)malloc(sizeof(struct avl_traverser));
	struct avl_table *listaProdutos = avl_create(&compare_especial, NULL, NULL);
	Compra seguinte,novo;
	struct avl_traverser* travessia  = (struct avl_traverser*) malloc(sizeof(struct avl_traverser));

	avl_t_init(travessia,aux->produtos);
	for(seguinte=avl_t_next(travessia);seguinte;seguinte=avl_t_next(travessia)){
		if(seguinte->meses[mes-1] != 0){
			novo = (Compra)malloc(sizeof(struct compra));
			novo->produto = (char *)malloc(sizeof(char) * strlen(seguinte->produto) + 1);
			strcpy(novo->produto,seguinte->produto);
			novo->meses = NULL;
			avl_insert(listaProdutos, novo);
		}
	}

	free(travessia);
	free(procurar->cliente);
	free(procurar);
	avl_t_init(lista->travessia, listaProdutos);
	return lista;
}

ListaClientes clientesQCompraramTodosOsMeses(CatalogoCompras compras){
	if(!compras) return NULL;

	int aux,i;
	Lista lista = (Lista)malloc(sizeof(struct lista));
	lista->travessia = (struct avl_traverser *)malloc(sizeof(struct avl_traverser));
	struct avl_table *listaClientes = avl_create(&compare_compras_clientes, NULL, NULL);
	Cliente seguinte,novo;

	struct avl_traverser* travessia  = (struct avl_traverser*) malloc(sizeof(struct avl_traverser));
	avl_t_init(travessia,compras->compras);
	for(seguinte=avl_t_next(travessia);seguinte;seguinte=avl_t_next(travessia)){
		aux = 0;
		for(i=0;i<12;i++){
			aux += clienteCompraNumMes(seguinte,i);
		}
		if(aux==12){
			novo = (Cliente)malloc(sizeof(struct cliente));
			novo->cliente = (char *)malloc(sizeof(char) * strlen(seguinte->cliente) + 1);
			strcpy(novo->cliente, seguinte->cliente);
			novo->produtos = NULL;
			avl_insert(listaClientes,novo);
		}
	}
	free(travessia);

	avl_t_init(lista->travessia, listaClientes);
	return lista;
}

int clienteCompraNumMes(Cliente cliente,int mes){
	if(!cliente) return 0;

	Compra seguinte;
	struct avl_traverser* travessia  = (struct avl_traverser*) malloc(sizeof(struct avl_traverser));

	avl_t_init(travessia,cliente->produtos);
	for(seguinte=avl_t_next(travessia);seguinte;seguinte=avl_t_next(travessia))
		if(seguinte->meses[mes] != 0) return 1;
	return 0;
}

int clientesNumMes(CatalogoCompras catcompras,int mes){
	if(!catcompras || mes < 1 || mes >12) return 0;
	int res=0;
	Cliente seguinte;
	struct avl_traverser* travessia  = (struct avl_traverser*) malloc(sizeof(struct avl_traverser));
	avl_t_init(travessia,catcompras->compras);
	for(seguinte=avl_t_next(travessia);seguinte;seguinte=avl_t_next(travessia)){
		if(clienteCompraNumMes(seguinte,mes-1) == 1) res++;
	}
	return res;
}

ListaProdutos produtosMaisComprados(CatalogoCompras catcompras){
	if(!catcompras) return NULL;

	ListaTotal lista = (Lista)malloc(sizeof(struct lista));
	lista->travessia = (struct avl_traverser *)malloc(sizeof(struct avl_traverser));
	struct avl_table *listaProdutos = avl_create(&compare_compras_produtos, NULL, NULL);
	struct avl_table *listaProdutosFinal = avl_create(&compare_especial, NULL, NULL);
	Compra seguinteProduto, encontrado, novo;
	Cliente seguinteCliente;

	struct avl_traverser* travessiaClientes  = (struct avl_traverser*) malloc(sizeof(struct avl_traverser));
	struct avl_traverser* travessiaProdutos  = (struct avl_traverser*) malloc(sizeof(struct avl_traverser));
	avl_t_init(travessiaClientes,catcompras->compras);
	seguinteCliente=avl_t_next(travessiaClientes);
	if(!seguinteCliente) return NULL;

	/* Vai ao 1º Cliente e insere os produtos todos na estrutura */
	avl_t_init(travessiaProdutos,seguinteCliente->produtos);
	for(seguinteProduto=avl_t_next(travessiaProdutos);seguinteProduto;seguinteProduto=avl_t_next(travessiaProdutos)){
		novo = (Compra)malloc(sizeof(struct compra));
		novo->meses = (int*)malloc(sizeof(int)*12);
		novo->produto = (char *)malloc(sizeof(char) * strlen(seguinteProduto->produto) + 1);
		strcpy(novo->produto, seguinteProduto->produto);
		novo->vendasP = seguinteProduto->vendasP + seguinteProduto->vendasN;
		novo->vendasN = 0;
		novo->meses[0]=1;
		avl_insert(listaProdutos, novo);
	}

	/* Vai aos outros clientes e procura os produtos na estrutura pra os atualizar, se nao encontra adiciona o */
	for(seguinteCliente=avl_t_next(travessiaClientes);seguinteCliente;seguinteCliente=avl_t_next(travessiaClientes)){
		avl_t_init(travessiaProdutos,seguinteCliente->produtos);
		for(seguinteProduto=avl_t_next(travessiaProdutos);seguinteProduto;seguinteProduto=avl_t_next(travessiaProdutos)){
			encontrado = avl_find(listaProdutos, seguinteProduto);
			if(!encontrado){
				novo = (Compra)malloc(sizeof(struct compra));
				novo->meses = (int*)malloc(sizeof(int)*12);
				novo->produto = (char *)malloc(sizeof(char) * strlen(seguinteProduto->produto) + 1);
				strcpy(novo->produto, seguinteProduto->produto);
				novo->vendasP = seguinteProduto->vendasP + seguinteProduto->vendasN;
				novo->vendasN = 0;
				novo->meses[0]=1;
				avl_insert(listaProdutos, novo);
			}
			else{
				encontrado->vendasP += (seguinteProduto->vendasN + seguinteProduto->vendasP);
				encontrado->meses[0]++;
			}
		}
	}

	/* Copiar tudo pra nova struct */
	avl_t_init(travessiaProdutos,listaProdutos);
	for(seguinteProduto=avl_t_next(travessiaProdutos);seguinteProduto;seguinteProduto=avl_t_next(travessiaProdutos))
		avl_insert(listaProdutosFinal,seguinteProduto);

	/* destroi a antiga */
	avl_destroy(listaProdutos, NULL);
	free(travessiaClientes);
	free(travessiaProdutos);
	avl_t_init(lista->travessia, listaProdutosFinal);

	return lista;
}

int nrClientesCompraram(CatalogoCompras catcompras){
	if(!catcompras) return 0;

	if(catcompras->compras)
		return(catcompras->compras->avl_count);
	else
		return 0;
}

ListaProdutos produtosMaisCompradosCliente(CatalogoCompras catcompras, char *cliente){
	if(!catcompras || !cliente) return NULL;

	int quantidade;
	Lista lista = (Lista)malloc(sizeof(struct lista));
	lista->travessia = (struct avl_traverser *)malloc(sizeof(struct avl_traverser));
	Cliente procura,aux;
	Compra seguinte, novo;
	struct avl_table *listaProdutos = avl_create(&compare_especial, NULL, NULL);

	procura = (Cliente)malloc(sizeof(struct cliente));
	procura->cliente = (char *)malloc(sizeof(char) * strlen(cliente) + 1);
	strcpy(procura->cliente,cliente);

	aux = avl_find(catcompras->compras,procura);
	if(!aux)	return NULL;

	struct avl_traverser* travessia  = (struct avl_traverser*) malloc(sizeof(struct avl_traverser));
	avl_t_init(travessia,aux->produtos);

	for(seguinte=avl_t_next(travessia);seguinte;seguinte=avl_t_next(travessia)){
		novo = (Compra)malloc(sizeof(struct compra));
		novo->meses = NULL;
		novo->produto = (char *)malloc(sizeof(char) * strlen(seguinte->produto) + 1);
		strcpy(novo->produto, seguinte->produto);
		novo->vendasP = seguinte->vendasP + seguinte->vendasN;
		novo->vendasN = 0;
		avl_insert(listaProdutos, novo);
	}

	avl_t_init(lista->travessia, listaProdutos);
	free(travessia);
	free(procura->cliente);free(procura);
	return lista;
}
