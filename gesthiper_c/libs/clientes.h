typedef struct C_CLIENTE *CAT_CLIENTES;						/* Estrutura de dados para os catalogos de clientes */
typedef struct structcliente *CLIENTE;						/* Estrutura de dados para um cliente */
typedef struct travessiaClientes *ListaClientes;			/*Estrutura de dados de uma lista de clientes*/

/* API Catalogo de clientes */

CAT_CLIENTES inicializaClientes();						/* Inicializa e aloca memoria para um catalogo */

void destroiCatalogoClientes(CAT_CLIENTES catalogo);				/* Liberta toda a memoria relativa ao catalogo */

CAT_CLIENTES adicionarCliente(CAT_CLIENTES catalogo, char *cliente);		/* Adicionar um produto num catalago */

int nrCliLetra(CAT_CLIENTES catalogo, char letra);				/* Dado um 'catalogo' retorna o nr de clientes começados por uma 'letra' */

int existeCliente(CAT_CLIENTES catalogo, char *cliente);			/* Dado um 'catalogo' e um 'produto' verifica se existe(1) ou nao existe(0) */

ListaClientes clientesPorLetra(CAT_CLIENTES catalogo, char l);	/* Dada uma letra retorna a lista de clientes começados pela letra*/

char* proximoCliente(ListaClientes lista_clientes);				/*Da o próximo cliente da lista*/

void destroiTravessiaClientes(ListaClientes lista_clientes);		/* Liberta a estrutura de ListaClientes da memoria */

int nrCli(CAT_CLIENTES catalogo);						/*Da numero total de clientes*/
