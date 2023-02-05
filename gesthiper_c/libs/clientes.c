#include "./clientes.h"
#include "./avl.h"
#include <string.h>
#include <stdlib.h>

/* STRUCTS */
typedef char *strCliente;	
typedef struct structcliente {
	strCliente cliente;
}*Cliente;

typedef struct C_CLIENTE {
	struct avl_table *arrayAZ[26];
}*CAT_CLIENTES;

typedef struct travessiaClientes{
	struct avl_traverser *traverser;
}*ListaClientes;
								/* API CATALOGO_PRODUTOS */

int compare_clientes(const void *nodo1,const void *nodo2, void *avl_param){

	Cliente n1,n2;
	n1 = (Cliente)nodo1;
	n2 = (Cliente)nodo2;
	
	return(strcmp(n1->cliente,n2->cliente));
}

void item_func_Clientes(void *avl_item, void *avl_param){

	Cliente n1 = (Cliente)avl_item;
	free(n1->cliente);
	free(n1);
}

CAT_CLIENTES inicializaClientes(){

	int i;
	CAT_CLIENTES novocatalogo = (CAT_CLIENTES)malloc(sizeof(struct C_CLIENTE));
	avl_comparison_func *myComp = compare_clientes;

	for(i=0; i < 26; i++){
		novocatalogo->arrayAZ[i] = avl_create(myComp, NULL, NULL);	
	}
	return novocatalogo;
}

void destroiCatalogoClientes(CAT_CLIENTES catalogo){
	int i;
	avl_item_func *myItemFunc = item_func_Clientes;	
	for(i=0;i<26;i++)
		avl_destroy(catalogo->arrayAZ[i], myItemFunc);
	free(catalogo);
}

CAT_CLIENTES adicionarCliente(CAT_CLIENTES catalogo, char *cliente){

	if(!cliente) return NULL;
	if(!(isupper(cliente[0]))) return NULL;

	int indice = cliente[0] - 'A';

	Cliente novoCliente = (Cliente)malloc(sizeof(struct structcliente));
	novoCliente->cliente = (char *)malloc(sizeof(char) * strlen(cliente)+1);
	
	strcpy(novoCliente->cliente,cliente);

	novoCliente = avl_insert(catalogo->arrayAZ[indice], novoCliente);
	if(novoCliente == NULL){	/* Inseriu com sucesso */
		return catalogo;
	}
	else{
		free(novoCliente->cliente);free(novoCliente);
		return NULL;
	}
}

int nrCliLetra(CAT_CLIENTES catalogo, char letra){
	char minhaletra = toupper(letra);
	
	if(!(isupper(minhaletra)) || !catalogo) return 0;
	
	int indice = minhaletra - 'A';
	return (catalogo->arrayAZ[indice]->avl_count);
}

int existeCliente(CAT_CLIENTES catalogo, char *cliente){

	int indice = cliente[0] - 'A';
	Cliente aux = (Cliente)malloc(sizeof(struct structcliente));
	aux->cliente = (char *)malloc(sizeof(char) * strlen(cliente)+1);
	strcpy(aux->cliente,cliente);

	if( avl_find(catalogo->arrayAZ[indice], aux) ){
		free(aux->cliente);
		free(aux);
		return 1;
	}
	free(aux->cliente);
	free(aux);
	return 0;
}

ListaClientes inicializaTravessiaClientes(CAT_CLIENTES catalogo, char letra){
	int indice_letra = letra - 'A';
	ListaClientes nova_lista = (ListaClientes)malloc(sizeof(struct travessiaClientes));
	
	struct avl_traverser *travessia = (struct avl_traverser*)malloc(sizeof(struct avl_traverser));
	avl_t_init(travessia, catalogo->arrayAZ[indice_letra]);
	nova_lista->traverser = travessia;

	return nova_lista;
}

ListaClientes clientesPorLetra(CAT_CLIENTES catalogo, char l){
	ListaClientes lista;
	l = toupper(l);
	int num_clientes = nrCliLetra(catalogo, l);

	if(num_clientes == 0) return NULL;
	lista = inicializaTravessiaClientes(catalogo, l);

	return lista;
}

void destroiTravessiaClientes(ListaClientes lista_clientes){
	free(lista_clientes->traverser);
	free(lista_clientes);
}

char* proximoCliente(ListaClientes lista_clientes){
	Cliente seguinte = avl_t_next(lista_clientes->traverser);
	char* cliente;

	if(!seguinte) return NULL;
	return (seguinte->cliente);
}

int nrCli(CAT_CLIENTES catalogo){
	int  total=0;
	char letra = 'A';

	for(letra = 'A';letra<='Z';letra++)
		total += nrCliLetra(catalogo,letra);
	
	return total;
}

