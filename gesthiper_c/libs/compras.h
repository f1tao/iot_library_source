typedef struct catalogocompras *CatalogoCompras;
typedef struct lista *Lista, *Lista_Clientes, *Lista_Produtos, **DuplaLista;

/* API */
CatalogoCompras inicializaCompras();	/* Inicializa e aloca memoria para a estrutura das compras*/
void destroiCatalogoCompras(CatalogoCompras compras);	/* Liberta toda a memoria relativa à estrutura das compras*/						
CatalogoCompras insereCompraCatalogo(CatalogoCompras compras, char* cliente, char* produto, int mes, int quantidade, char promocao, double preco); /* Insere uma compra no catalogo -> Query1*/
int produtosCompradosClienteMes(CatalogoCompras compras,char* cliente, int mes);	/* Devolve o nr de produtos que um cliente comprou num mês -> query5*/
DuplaLista prodsCompradosPorCliente(CatalogoCompras compras, char *produto);	/* 2 listas com os clientes que comprar o 'produto', 1ª lista(compras em modo N), 2ª lista(compras em modo P)->Query8*/
Lista_Produtos produtosMaisCompradosClienteMes(CatalogoCompras cat_compras, char* cliente, int mes); /* Devolve os produtos mais comprados por um cliente num mês -> query9*/
Lista_Clientes clientesQCompraramTodosOsMeses(CatalogoCompras compras);	 			/* Devolve a lista dos clientes que realizam compras todo ano -> query10 */
int clientesNumMes(CatalogoCompras catcompras,int mes);					/*Devolve o nr de clientes q compraram num mes -> query11 */
Lista_Produtos produtosMaisComprados(CatalogoCompras catcompras);				/* Retorna a lista dos produtos mais comrpados (decrescente) -> query12 */
Lista_Produtos produtosMaisCompradosCliente(CatalogoCompras catcompras, char *cliente);		/* Devolve lista o com os produtos comprados por um cliente, ordenada pela qtd -> query13 */
int nrClientesCompraram(CatalogoCompras catcompras);					/* Retorna o nr de clientes que realizaram compras -> query14*/


/* API LISTAS */
/*lista*/
char *proxEltoListaProdutos(Lista_Produtos lista);	/*Devolve o próximo elemento de uma lista de produtos*/
char *proxEltoListaClientes(Lista_Clientes lista);	/*Devolve o próximo elemento de uma lista de clients*/
char *anteriorEltoListaProdutos(Lista_Produtos lista);	/*Devolve o elemento anterior de uma lista de produtos*/
char *anteriorEltoListaClientes(Lista_Clientes lista);	/*Devolve o elemento anterior de uma lista de clientes*/
int daNrEltosLista(Lista lista);			/* Devolve o numero de elementos de uma lista */
/* A funcao daQtdProduto quando é chamada em consequencia da lista retornada pela funcao produtosMaisComprados retorna o nr de compras (esc = 0) ou retorna o nr de clientes (esc = 1) */
int daQtdProduto(Lista lista, int esc);			/* Dá as vendas de um produto num mês, se for no ano inteiro mes = 0 */
void destruirListaProdutos(Lista_Produtos lista);	/* Liberta a memória associada a uma lista de produtos*/
void destruirListaClientes(Lista_Clientes lista);	/* Liberta a memória associada a uma lista de clientes*/
/* Duplalista */
void destruirDuplaLista(DuplaLista duplalista);		/* Liberta a memória associada a uma dupla lista*/
Lista duplaListatoLista(DuplaLista lista, char promo);	/* Converte uma dupla lista numa lista e devolve essa conversão*/				


