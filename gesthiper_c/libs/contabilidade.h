typedef struct contabilidade *CONTABILIDADE;								/* Estrutura de dados para o modulo da Contabilidade */
typedef struct registo *Registo;									/* Estrutura que representa um registo da contabilidade */
typedef struct listaContabilidade *ListaContabilidade;	/*Estrutura que permite percorrer um conjunto de registos da contabilidade*/


CONTABILIDADE inicializa_contabilidade();								/* Inicializa e aloca memória para a estrutura da Contabilidade */
void destroiCatalogoContabilidade(CONTABILIDADE contabilidade);							/* Liberta a estrutura da Contabilidade da memória */

void adicionarRegisto(CONTABILIDADE contabilidade, char* produto);					/* Dado um mês e um registo adiciona-o à Contabilidade  */
Registo updateRegisto(CONTABILIDADE contabilidade, char *produto, int vendasN, double fatN, int vendasP, double fatP, int mes);		/* Atualiza os valores de um registo na contabilidade */
Registo encontra_registo(CONTABILIDADE contabilidade, char* prd);				/* Dada uma contabilidade e um código de produto, verifica se existe esse código na contabilidade*/
int daVendasN(Registo reg, int mes);										/*Dado um registo da contabilidade, devolve o número de vendas em modo N*/
int daVendasP(Registo reg, int mes);										/*Dado um registo da contabilidade, devolve o número de vendas em modo P*/
double daFaturacaoTotal(Registo reg, int mes);									/*Dado um registo da contabilidade, devolve a faturação total desse registo*/
int nrVendas(CONTABILIDADE contabilidade, int mes);						/*Dada uma estrutura da contabilidade, um índice correspondente a um mês e um índice correspondente à primeira letra de um código de produto devolve o número de vendas total*/
double totalFaturado(CONTABILIDADE contabilidade, int indice_mes);			/*Dada uma estrutura da contabilidade, um índice correspondente a um mês e um índice correspondente à primeira letra de um código de produto devolve a faturação total*/
ListaContabilidade produtosNaoComprados(CONTABILIDADE contabilidade);							/* Retorna o nr de produtos que nao foram comprados */

/* API LISTA */
int daNrCodigos(ListaContabilidade lista);											/* Dada um lista devolve o numero de registos */
char *proxCodigo(ListaContabilidade lista);										/* Dada uma lista devolve proximo registo */
char *anteriorCodigo(ListaContabilidade lista);									/* Dada uma lista devolve anterior registo */
void destroiLista(ListaContabilidade lista);											/* Liberta a estrutura de ListaContabilidade da memoria */
