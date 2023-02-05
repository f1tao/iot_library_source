#include "./libs/produtos.h"
#include "./libs/clientes.h"
#include "./libs/compras.h"
#include "./libs/contabilidade.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#define linhasApresentar 20

typedef struct listaMain{
	
	char** lista;
	int tamanho,posicao;
	int **parametros;

}*ListaMain;

/*Funçoes que fazem o print em paginas */
int print_pagina(ListaMain lista_main, int flag){
	int i=0, j=1, res=0, total=0;
	char *aux;

	if(!lista_main) return 0;
	if(flag==0){
	printf(" ||=============||==============||\n"); 
	printf(" ||\t//\t||\tCÓDIGO\t||\n");
        printf(" ||=============||==============||\n"); 	
	for(i=lista_main->posicao; i<linhasApresentar + lista_main->posicao; i++){
		aux = lista_main->lista[i];
		if(aux && i != lista_main->tamanho){
			printf(" ||\t%d\t||\t%s\t||\n", j, aux);
			j++;
		}
		else break;
	}
	printf(" ||=============||==============||\n"); 
	lista_main->posicao = i;

	}else if(flag==1){
	printf(" ||=============||==============||\n"); 
	printf(" || \tMês\t||\tNr\t||\n");
        printf(" ||=============||==============||\n");	
	for(i=lista_main->posicao; i<linhasApresentar + lista_main->posicao; i++){
		res =  lista_main->parametros[0][i];
		if(lista_main->parametros && i != lista_main->tamanho){
			printf(" ||\t%d\t||\t%d\t||\n", j,  res);
			j++;
			total += res;
		}
		else break;
	}
	printf(" ||=============||==============||\n");
	printf(" ||\tTotal\t||\t%d\t||\n", total);
	printf(" ||=============||==============||\n");
	lista_main->posicao = i;
	}
	return i;
}

int print_pagina2(ListaMain lista_main){
	int i=0, j=1;
	char *aux;

	if(!lista_main) return 0;
	printf(" ||=============||==============||==============||==============||\n"); 
	printf(" ||\t//\t||\tCÓDIGO\t||\tCOMPRAS\t||NR CLIENTES\t||\n");
        printf(" ||=============||==============||==============||==============||\n");	
	for(i=lista_main->posicao; i<linhasApresentar + lista_main->posicao; i++){
		aux = lista_main->lista[i];
		if(aux && i != lista_main->tamanho){
			printf(" ||\t%d\t||\t%s\t||\t%d\t||\t%d\t||\n", j, aux,lista_main->parametros[0][i],lista_main->parametros[1][i]);
			j++;
		}
		else break;
	}
	printf(" ||=============||==============||==============||==============||\n");
	lista_main->posicao = i;
	return i;
}

int print_pagina3(ListaMain lista_main){
	int i=0;
	char *aux;

	if(!lista_main) return 0;
	printf(" ||=============||==============||==============||\n"); 
	printf(" ||\t//\t||\tCÓDIGO\t||\tCOMPRAS\t||\n");
        printf(" ||=============||==============||==============||\n");	
	for(i=lista_main->posicao; i<linhasApresentar + lista_main->posicao; i++){
		aux = lista_main->lista[i];
		if(aux && i != lista_main->tamanho){
			printf(" ||\t%d\t||\t%s\t||\t%d\t||\n", i+1, aux,lista_main->parametros[0][i]);
		}
		else break;
	}
	printf(" ||=============||==============||==============||\n");
	lista_main->posicao = i;
	return i;
}


int acertaPagina(int pag_atual, int pag_pretendida, int total_pags,  ListaMain lista_main){
	int resto, andarTras = 0, andarFrente = 0;
	
	if(!lista_main)return 0;
	
	if(pag_atual == pag_pretendida || pag_pretendida < 0 || pag_pretendida >= total_pags) return -1;

	if(pag_atual == total_pags-1){				/* Caso particular da ultima pagina*/
		resto = (lista_main->tamanho%linhasApresentar);
		printf("-------%d------\n",resto);
		if(resto == 0)
			andarTras = linhasApresentar + (linhasApresentar*(pag_atual - pag_pretendida));
		else
			andarTras = resto + (linhasApresentar*(pag_atual - pag_pretendida));
	}

	else if(pag_atual < pag_pretendida){			/* Andar para frente */
		if((pag_pretendida - pag_atual) == 1) return pag_pretendida;
		andarFrente = linhasApresentar*(pag_pretendida-1 - pag_atual);

	}
	else if(pag_atual > pag_pretendida){			/* Andar para trás */
		andarTras = linhasApresentar + linhasApresentar*(pag_atual - pag_pretendida);
	}

	/* Acerto da posicao */
	if(andarTras){
		lista_main->posicao -= andarTras;
	}

	if(andarFrente){
		lista_main->posicao += andarFrente;
	}

	return pag_pretendida;
}

void printGeral(ListaMain lista_main, int flag, int flag2){
	int nr_pags=0, pagina=0, escolha_pagina = 0;
	char cursor = 'D';	

	if(!lista_main) return;	

	if(lista_main->tamanho%linhasApresentar == 0)     nr_pags = lista_main->tamanho/linhasApresentar;   /* Calcular nr de paginas */
        else nr_pags = lista_main->tamanho/linhasApresentar + 1;
	
	if(flag == 0 && flag2==1)	print_pagina(lista_main,0);
	else if(flag==1 && flag2==0) print_pagina(lista_main, 1);
	else if(flag==0 && flag2==0) print_pagina3(lista_main);                              
	else print_pagina2(lista_main);
        printf("(Página: %d/%d)\n",pagina+1,nr_pags);

        while(cursor != 'S' && pagina >= 0 && pagina < nr_pags){

                printf("\nIntroduza um dos seguintes caracteres:\nProxima -> 'P' \tAnterior -> 'A'\tSair -> 'S'\nSe pretender ir a uma página especifica introduza 'N'\n");
                scanf(" %c", &cursor);
                printf("\nEscolheu %c.\n",cursor);
                if(!(cursor == 'P' || cursor == 'A' || cursor == 'S' || cursor == 'N')){
                        printf("Comando inválido.\n");
                        return;
                }

                switch(cursor){
                        case 'N':
                                printf("Introduza o numero da pagina: \n");
                                scanf(" %d", &escolha_pagina);
                                escolha_pagina--;

                                if(escolha_pagina < 0 || escolha_pagina >= nr_pags || escolha_pagina == pagina){
                                        printf("Erro na escolha de página.\n");
                                        break;
                                }

                                pagina = acertaPagina(pagina, escolha_pagina, nr_pags, lista_main);
                                if(flag == 0)	print_pagina(lista_main,0);                                      
				else print_pagina2(lista_main);
                                printf("(Página: %d/%d)\n",pagina+1,nr_pags);

                        break;
			
			case 'P':
                                if(pagina == nr_pags-1){
                                        printf("Nao existem mais páginas.\n");
                                        break;
                                }
                                pagina = acertaPagina(pagina, (pagina+1), nr_pags, lista_main);
                                if(flag == 0)	print_pagina(lista_main,0);                                      
				else print_pagina2(lista_main);
                                printf("(Página: %d/%d)\n",pagina+1,nr_pags);

                        break;

                        case 'A':
                                if(pagina == 0){
                                        printf("Nao existe página anterior.\n");
                                        break;
                                }
                                pagina = acertaPagina(pagina, (pagina-1), nr_pags, lista_main);
                                if(flag == 0)	print_pagina(lista_main,0);                                      
				else print_pagina2(lista_main);
                                printf("(Página: %d/%d)\n",pagina+1,nr_pags);
                        break;
                }
        }
}

/* Ler os 3 ficheiros, se houver problemas retorna 1 , se não retorna 0 */
int lerProdutos(CAT_PRODUTOS catalogo, CONTABILIDADE contabilidade, char *fileprodutos){
	
	int i = 0;
	char aux[10],ficheiro[30];
	char *token;
	FILE *d;

	clock_t ini = clock();

	strcpy(ficheiro,"../files/");
	strcat(ficheiro,fileprodutos);

	d = fopen(ficheiro, "r");
	if (d == NULL){ 
		printf("Erro ao abrir o ficheiro!\n");
		return 1;
	}
	printf("\nA ler produtos...\n");
	while(fgets(aux, 10, d)!=NULL){
		token = strtok(aux,"\n\r ");
		if(token){
			catalogo = adicionarProduto(catalogo,token);
			if(!catalogo)
				return 1; /* Erro ao inserir produto no catalogo */
			
			adicionarRegisto(contabilidade,token);
			i++;
		}
	}
	fclose(d);
	printf("\nFicheiro: %s\nLinhas lidas: %d\n", ficheiro, i);
	printf("Funcao executou em %.3f segundos\n", ((double)clock() - ini) / CLOCKS_PER_SEC);
	
	return 0;
}

int lerClientes(CAT_CLIENTES catalogo, char *fileclientes){
	
	int i = 0;
	char aux[10],ficheiro[30];
	char *token;
	FILE *d;
	
	clock_t ini = clock();

	strcpy(ficheiro,"../files/");
	strcat(ficheiro,fileclientes);

	d = fopen(ficheiro, "r");
	if (d == NULL){ 
		printf("Erro ao abrir o ficheiro!\n");
		return 1;
	}
	printf("\nA ler clientes...\n");
	while(fgets(aux, 10, d)!=NULL){
		token = strtok(aux,"\n\r ");
		if(token){
			catalogo = adicionarCliente(catalogo,token);
			if(!catalogo)
				return 1; /* Erro ao inserir cliente no catalogo */
			
			i++;
		}
	}
	fclose(d);
	printf("\nFicheiro: %s\nLinhas lidas: %d\n", ficheiro, i);
	printf("Funcao executou em %.3f segundos\n", ((double)clock() - ini) / CLOCKS_PER_SEC);

	return 0;
}

int lerCompras(CatalogoCompras compras, CAT_CLIENTES catalogoClientes, CAT_PRODUTOS catalogoProdutos, CONTABILIDADE contabilidade, char *filecompras){

	int i, esc, erro, certas = 0, erradas= 0, quantidade=0,mes=0;
	double preco=0;
	char produto[10],cliente[10],promocao='N', auxiliar[60],*token,ficheiro[30];
	FILE *d;

	clock_t ini = clock();

	strcpy(ficheiro,"../files/");
	strcat(ficheiro,filecompras);

	d = fopen(ficheiro, "r");
	
	if (d == NULL){ 
		printf("Erro ao abrir o ficheiro!\n");
		return 1;
	}
	printf("\nA ler compras...\n");
	/* Le do ficheiro e guarda nas compras */
	while(fgets(auxiliar, 60, d)!=NULL){
		erro = 0;

		/* Primeira particao da string */
		token = strtok(auxiliar, " \n\r");
		if(token){
			strcpy(produto,token);
			if(existeProduto(catalogoProdutos, produto) == 0) 
				erro = 1;


			/* Partições seguintes */
			esc = 0;
			while(token != NULL && erro == 0){
				esc++;
				token = strtok(NULL, " \n\r");

				switch (esc){
					/* preco */
					case 1:
						strcpy(auxiliar, token);
						preco = atof(auxiliar);
					break;
					/* quantidade */
					case 2: 
						strcpy(auxiliar, token);
						quantidade = atoi(auxiliar);
						if(quantidade<0) erro = 1;
					break;
					/* promocao */
					case 3: 
						promocao = token[0];
						if(!(promocao == 'N' || promocao == 'P')) erro = 1;
					break;
					/* cliente */
					case 4: 
						strcpy(cliente, token);
						if(existeCliente(catalogoClientes, cliente)==0)
							erro=1;
					break;
					/* mes */
					case 5: 
						strcpy(auxiliar, token);
						mes = atoi(auxiliar);
						if(!(mes > 0 && mes <= 12)) erro=1;
					break;
				}
			}
			if(esc<=6){/*Assegura-se que em cada linha leu os 5 campos*/
				if(erro == 0){
					certas++;i++;
					/* Insere no catalogo compras */
					insereCompraCatalogo(compras,cliente,produto,mes,quantidade,promocao,preco);
					/* Insere no catalogo contablidade */
					if(promocao == 'N'){
						updateRegisto(contabilidade, produto, quantidade, preco, 0, 0,mes);
					}
					else{
						updateRegisto(contabilidade, produto, 0, 0,quantidade, preco,mes);
					}
				}
				else erradas++;
			}
			else return 0; /* Problema com o ficheiro que tem mais que 6 strings na mesma linha */
		}
	}
	fclose(d);
	printf("\nFicheiro: %s\nCompras válidas: %d-%d\n",ficheiro, certas,(certas+erradas));
	printf("Funcao executou em %.3f segundos\n", ((double)clock() - ini) / CLOCKS_PER_SEC);
	return 0;
}

void query2(CAT_PRODUTOS catprod){

	char letra;
	char* aux;
	int i=0,num_prods = 0;
	LISTA listaQ2;
	ListaMain lista;
		
	clock_t ini = clock();
	
	printf("Escolha a letra:\n");scanf(" %c",&letra); letra = toupper(letra);	/* Escolha letra */
	
	listaQ2 = produtosPorLetra(catprod, letra);					/* Buscar lista */
	
	if(listaQ2 == NULL){ 
		printf("Não existem produtos começados por essa letra.\n");
		return;
	}

	num_prods =nrPorLetra(catprod, letra);						/* Buscar nr de produtos começados pela letra */

	lista = (ListaMain)malloc(sizeof(struct listaMain));
	lista->lista = (char**)malloc(sizeof(char*)*num_prods);
	lista->tamanho = num_prods;
	lista->posicao = 0;	

	for(i=0;i<num_prods;i++){
		aux = proximoProduto(listaQ2);
		if(aux) lista->lista[i] = aux;	
		else break;
	}
	
	printf("Existem %d produtos iniciados por %c.\n\n", num_prods, letra);

	printGeral(lista,0,1);

	destroiTravessia(listaQ2);

	free(lista);	

	printf("Funcao executou em %.3f segundos\n", ((double)clock() - ini) / CLOCKS_PER_SEC);
}

void query3(CONTABILIDADE contabilidade){
	int vendasN, vendasP, mes;
	double faturacao;
	Registo reg_aux;
	char prd_aux[10];

	clock_t ini = clock();

	printf("Indique o mês:\n");
	scanf("%d", &mes);
	if(mes > 12 || mes < 1){
		printf("Introduza um mês válido\n");
		return;
	}
	printf("Indique o produto:\n");
	scanf("%s",prd_aux);
	if(strlen(prd_aux) != 6){
		printf("O código de produto não tem o tamanho certo\n");
		return;
	}
	prd_aux[0] = toupper(prd_aux[0]);
	prd_aux[1] = toupper(prd_aux[1]);
	if((!(isalpha(prd_aux[0]))) || (!(isalpha(prd_aux[1]))) || (!(isdigit(prd_aux[2]))) || (!(isdigit(prd_aux[3]))) || (!(isdigit(prd_aux[4]))) || (!(isdigit(prd_aux[5])))){
		printf("Introduza um código de produto com 2 letras e 4 números\n");
		return;
	}

	/*'reg_aux' fica a NULL se o produto não for encontrado, ou seja, não existir*/
	/*Se o produto existir, 'reg_aux' fica a apontar para esse registo da contabilidade*/
	reg_aux = encontra_registo(contabilidade, prd_aux);

	if(!(reg_aux)){
		printf("O produto não existe\n");
		return;
	}

	/*Para proteger a abstração de dados*/
	vendasN = daVendasN(reg_aux,mes);
	vendasP = daVendasP(reg_aux,mes);
	faturacao = daFaturacaoTotal(reg_aux, mes);

	printf("Vendas em modo N: %d\nVendas em modo P: %d\nFaturação total: %.3f\n", vendasN, vendasP, faturacao);

	printf("Funcao executou em %.3f segundos\n", ((double)clock() - ini) / CLOCKS_PER_SEC);
}

void query4(CONTABILIDADE contabilidade){
	int totalNaoComprados = 0,i=0;
	char *aux;
	ListaContabilidade lista = produtosNaoComprados(contabilidade);
	ListaMain lista_main;	

	clock_t ini = clock();

	totalNaoComprados = daNrCodigos(lista);
	
	lista_main = (ListaMain)malloc(sizeof(struct listaMain));
        lista_main->lista = (char**)malloc(sizeof(char*)*totalNaoComprados);
        lista_main->tamanho = totalNaoComprados;
	lista_main->posicao = 0;

        for(i=0;i<totalNaoComprados;i++){
                aux = proxCodigo(lista);
                if(aux) lista_main->lista[i] = aux;
                else break;
        }

	printf("O numero de produtos que nunca foram comprados são: %d \n", totalNaoComprados);
	
	printGeral(lista_main,0,1);	

	destroiLista(lista);

    	free(lista_main);

	printf("Funcao executou em %.3f segundos\n", ((double)clock() - ini) / CLOCKS_PER_SEC);
}

void query5(CatalogoCompras compras){
        int indiceMes=1, valor=0 ,valores[12];
        char cli[10],resposta;

	clock_t ini = clock();	

        FILE *f;

	if(!compras) return;

        printf("Indique o código de cliente:\n");
        scanf("%s",cli);

	printf("\nMês -> Produtos\n");
	for(indiceMes=1;indiceMes<=12;indiceMes++){
		valor = produtosCompradosClienteMes(compras,cli,indiceMes);
		if(valor == -1){
			printf("Nao existe cliente.\n");
			return;
		}
		if(valor >= 0){ 
			valores[indiceMes-1] = valor;
			printf("%d -> %d\n", indiceMes, valor);
		}
	}
	
        printf("Pretende escrever estes resultados num ficheiro txt ? S ou N?\n");
        scanf(" %c", &resposta);

        if(resposta=='S'){

                if((f=fopen("../files/fichQ5.txt", "w")) == NULL) {
                        printf("Erro ao abrir o ficheiro\n");
                        return;
                }
                fprintf(f, " ||    Mês      ||      Nr      ||\n");
		for(indiceMes=1; indiceMes<=12; indiceMes++){
			valor = valores[indiceMes-1]; 
			fprintf(f, " ||\t%d\t||\t%d\t||\n", indiceMes, valor);
		}
		fclose(f);
	}
        
	printf("Funcao executou em %.3f segundos\n", ((double)clock() - ini) / CLOCKS_PER_SEC);
}

void query6(CAT_CLIENTES catalogo){
	int i=0, total=0;
	char letra, *aux;
	ListaClientes lista;
	ListaMain lista_main;
	
	clock_t ini = clock();
	
	printf("Insira a letra:\n");
	scanf(" %c",&letra);

	lista = clientesPorLetra(catalogo,letra);

	total = nrCliLetra(catalogo, letra);
	
	lista_main = (ListaMain)malloc(sizeof(struct listaMain));
        lista_main->lista = (char**)malloc(sizeof(char*)*total);
        lista_main->tamanho = total;
	lista_main->posicao = 0;

        for(i=0;i<total;i++){
                aux = proximoCliente(lista);
                if(aux) lista_main->lista[i] = aux;
                else break;
        }
	
	if(!lista){
		printf("Não existem clientes começados por essa letra\n");
		return;
	}
	
	printf("Códigos de cliente comecados por %c : %d\n",letra,total);
	
	printGeral(lista_main,0,1);
		
	destroiTravessiaClientes(lista);
	
	free(lista_main);
	
	printf("Funcao executou em %.3f segundos\n", ((double)clock() - ini) / CLOCKS_PER_SEC);
}

void query7(CONTABILIDADE contabilidade){
	int mes1, mes2, num_total_vendas=0, indiceMes;
	double total_faturado=0;

	clock_t ini = clock();

	printf("Indique o primeiro mês(1 a 12):\n");
	scanf("%d",&mes1);
	printf("Indique o segundo mês(1 a 12):\n");
	scanf("%d",&mes2);

	if(mes2 > 12 || mes2 < 1 || mes1 > 12 || mes1 < 1 || mes1 > mes2){
		printf("Erro na introducao dos meses.\n");
		return;
	}

	for(indiceMes=mes1; indiceMes<=mes2; indiceMes++){
		total_faturado += totalFaturado(contabilidade,indiceMes);
		num_total_vendas += nrVendas(contabilidade,indiceMes);
	}
	printf("Número de vendas: %d\nTotal faturado: %.2f\n",num_total_vendas,total_faturado);
	printf("Funcao executou em %.3f segundos\n", ((double)clock() - ini) / CLOCKS_PER_SEC);
}

void query8(CatalogoCompras compras){
	char prod[8], escolha='0', *token, *aux; 
	int total=0, i=0, clientesP=0, clientesN=0;
	DuplaLista dlista;
	Lista_Clientes ListaP,ListaN;
	ListaMain lista_main;

	clock_t ini = clock();

	printf("Indique produto:\n");
	scanf("%s", prod);
	token = strtok(prod,"\n\r ");

	printf("1->Número total de compras desse produto\n2->Número total de compras desse produto em modo N\n3->Número total de compras desse produto em modo P\n");
	scanf(" %c", &escolha);
	if(escolha !='1' && escolha!='2' && escolha!='3') {
		printf("Opção invalida\n");
		return;
	}
	dlista = prodsCompradosPorCliente(compras, token);
	ListaP = duplaListatoLista(dlista,'P');
	ListaN = duplaListatoLista(dlista,'N');
	
	
	switch(escolha){
        	case '1':
			clientesP = daNrEltosLista(ListaP);
			clientesN = daNrEltosLista(ListaN);
			total = clientesP + clientesN;
			printf("Total: %d\n",total);
			
			lista_main = (ListaMain)malloc(sizeof(struct listaMain));
			lista_main->lista = (char**)malloc(sizeof(char*)*total);
    			lista_main->tamanho = total;
    			lista_main->posicao = 0;

			for(i=0;i<clientesP;i++){
                		aux = proxEltoListaClientes(ListaP);
                		if(aux) lista_main->lista[i] = aux;
                		else break;
        		}
			for(i=clientesP;i<total;i++){
                                aux = proxEltoListaClientes(ListaN);
                                if(aux) lista_main->lista[i] = aux;
                                else break;
                        }
                	break;
		case '2':
			total =  daNrEltosLista(ListaN);
			printf("Total: %d\n",total);
                        
			lista_main = (ListaMain)malloc(sizeof(struct listaMain));
		    	lista_main->lista = (char**)malloc(sizeof(char*)*total);
			lista_main->tamanho = total;
			lista_main->posicao = 0;
			
			for(i=0;i<total;i++){
                                aux = proxEltoListaClientes(ListaN);
                                if(aux) lista_main->lista[i] = aux;
                                else break;
                        }
			break;						
		case '3':
			total =  daNrEltosLista(ListaP);
                        printf("Total: %d\n",total);

			lista_main = (ListaMain)malloc(sizeof(struct listaMain));
			lista_main->lista = (char**)malloc(sizeof(char*)*total);
			lista_main->tamanho = total;
			lista_main->posicao = 0;

			for(i=0;i<total;i++){
                                aux = proxEltoListaClientes(ListaP);
                                if(aux) lista_main->lista[i] = aux;
                                else break;
                        }

                        break;
	}
	if(escolha=='1'||escolha=='2'||escolha=='3') 
		printGeral(lista_main,0,1);

    	free(lista_main);
	destruirListaClientes(ListaP);
	destruirListaClientes(ListaN);
	destruirDuplaLista(dlista);

	printf("Funcao executou em %.3f segundos\n", ((double)clock() - ini) / CLOCKS_PER_SEC);
}

void query9(CatalogoCompras cat_compras){
	char cliente[10], *aux;
	int mes, total_elementos, i=0;
	Lista_Produtos lista;
	ListaMain lista_main;

	clock_t ini = clock();

	printf("Insira um código de cliente:\n");
	scanf("%s",cliente);
	printf("Insira um mês:\n");
	scanf("%d",&mes);

	lista = produtosMaisCompradosClienteMes(cat_compras,cliente,mes);
	if(!lista){
		printf("Erro na introdução de dados.\n");
		return;
	}
	total_elementos = daNrEltosLista(lista);

	printf("Numero de produtos: %d\n",total_elementos);


	lista_main = (ListaMain)malloc(sizeof(struct listaMain));
	lista_main->lista = (char**)malloc(sizeof(char*)*total_elementos);
	lista_main->tamanho = total_elementos;
	lista_main->posicao = 0;

	for(i=0;i<total_elementos;i++){
		aux = proxEltoListaProdutos(lista);
		if(aux) lista_main->lista[i]=aux;
		else break;
	}

	printGeral(lista_main,0,1);
	
	destruirListaProdutos(lista);
	free(lista_main);
	printf("Funcao executou em %.3f segundos\n", ((double)clock() - ini) / CLOCKS_PER_SEC);
}

void query10(CatalogoCompras cat_compras){
	int totalClientes = 0, i=0;
	ListaMain lista_main;
	char* aux;

	clock_t ini = clock();

	Lista_Clientes lista = clientesQCompraramTodosOsMeses(cat_compras);
	totalClientes = daNrEltosLista(lista);

	lista_main = (ListaMain)malloc(sizeof(struct listaMain));
	lista_main->lista = (char**)malloc(sizeof(char*)*totalClientes);
	lista_main->tamanho = totalClientes;
	lista_main->posicao = 0;

	for(i=0;i<totalClientes;i++){
		aux = proxEltoListaClientes(lista);
		if(aux) lista_main->lista[i]=aux;
		else break;
	}

	printf("O numero de clientes que realizaram compras em todos os meses são: %d\n", totalClientes);
	
	printGeral(lista_main,0,1);
	
	destruirListaClientes(lista);

	free(lista_main);

	printf("Funcao executou em %.3f segundos\n", ((double)clock() - ini) / CLOCKS_PER_SEC);
}

void query11(CatalogoCompras cat_compras, CONTABILIDADE contabilidade){
	int mes,nrvendas,nrclientes;

	clock_t ini = clock();

	FILE *f=fopen("../files/query11.csv","w");
	if(!f) return ;

	fprintf(f, "Mês,Vendas,Clientes\n");
	for(mes = 1; mes <=12; mes++){
		nrvendas = nrVendas(contabilidade, mes);
		nrclientes = clientesNumMes(cat_compras,mes);
		fprintf(f, "%d,%d,%d\n", mes,nrvendas,nrclientes);
	}
	fclose(f);

	printf("Funcao executou em %.3f segundos\n", ((double)clock() - ini) / CLOCKS_PER_SEC);
}

void query12(CatalogoCompras compras){
	char *aux;
	int i, escolha,total;
	Lista_Produtos listaMaisComprados;
	ListaMain lista_main;

	clock_t ini = clock();
	
	if(!compras) return;
	lista_main = (ListaMain)malloc(sizeof(struct listaMain));
	listaMaisComprados = produtosMaisComprados(compras);

	printf("Insira o nr de produtos:\n");
	scanf(" %d", &escolha);

	total = daNrEltosLista(listaMaisComprados);
	if(escolha > total) escolha = total;

	lista_main->tamanho = escolha;
	lista_main->lista = (char**)malloc(sizeof(char*)*lista_main->tamanho);
	lista_main->posicao = 0;
	lista_main->parametros = (int**)malloc(sizeof(int *)*2);
	lista_main->parametros[0] = (int *)malloc(sizeof(int)*lista_main->tamanho);
	lista_main->parametros[1] = (int *)malloc(sizeof(int)*lista_main->tamanho);



	for(i=0;i<lista_main->tamanho;i++){
		aux = proxEltoListaProdutos(listaMaisComprados);
		if(aux){ 
			lista_main->lista[i]=aux;
			lista_main->parametros[0][i] = daQtdProduto(listaMaisComprados,0);
			lista_main->parametros[1][i] = daQtdProduto(listaMaisComprados,1);
		}
		else break;
	}
	printGeral(lista_main,1,1);

	free(lista_main->parametros[0]);
	free(lista_main->parametros[1]);
	free(lista_main->parametros);
	free(lista_main);
	destruirListaProdutos(listaMaisComprados);

	printf("Funcao executou em %.3f segundos\n", ((double)clock() - ini) / CLOCKS_PER_SEC);
}

void query13(CatalogoCompras compras){
	char cliente[10],*aux;
	int i = 0;
	Lista_Produtos listaMaisComprados;
	ListaMain lista_main;

	clock_t ini = clock();

	printf("Insira o cliente: \n");
	scanf("%s", cliente);

	listaMaisComprados = produtosMaisCompradosCliente(compras, cliente);
	if(!listaMaisComprados) printf("Nao existe lista");
	aux = proxEltoListaProdutos(listaMaisComprados);
	if(!aux) printf("Nao existe eltos");
	
	lista_main = (ListaMain)malloc(sizeof(struct listaMain));
	lista_main->lista = (char**)malloc(sizeof(char*)*3);
	lista_main->tamanho = 3;
	lista_main->posicao = 0;
	lista_main->parametros = (int**)malloc(sizeof(int *));
	lista_main->parametros[0] = (int *)malloc(sizeof(int)*lista_main->tamanho);

	while(aux && i<3){
		lista_main->lista[i]=aux;
		lista_main->parametros[0][i] = daQtdProduto(listaMaisComprados,0);
		aux = proxEltoListaProdutos(listaMaisComprados);
		i++;
	}

	printGeral(lista_main,0,0);

	free(lista_main->parametros[0]);
	free(lista_main->parametros);
	free(lista_main);
	destruirListaProdutos(listaMaisComprados);

	printf("Funcao executou em %.3f segundos\n", ((double)clock() - ini) / CLOCKS_PER_SEC);
}

void query14(CatalogoCompras compras, CAT_CLIENTES catcli, CONTABILIDADE contabilidade){
	int totalCliNaoCompraram=0,totalClientes =0, totalCliCompraram=0, totalNaoComprados = 0;
	ListaContabilidade lista;

	clock_t ini = clock();

	totalClientes = nrCli(catcli);
	totalCliCompraram = nrClientesCompraram(compras);
	totalCliNaoCompraram = totalClientes - totalCliCompraram;
	
	printf("O número de clientes que nao realizaram compras são: %d\n",totalCliNaoCompraram);

	lista = produtosNaoComprados(contabilidade);
	totalNaoComprados = daNrCodigos(lista);
	
	printf("O número de produtos que nunca foram comprados são: %d\n", totalNaoComprados);
	destroiLista(lista);

	printf("Funcao executou em %.3f segundos\n", ((double)clock() - ini) / CLOCKS_PER_SEC);
}

void destroiCatalogos(CAT_PRODUTOS catprod, CAT_CLIENTES catcli, CatalogoCompras compras, CONTABILIDADE contabilidade){
	printf("\nA apagar dados...\n");
	destroiCatalogoProdutos(catprod);
	destroiCatalogoClientes(catcli);
	destroiCatalogoContabilidade(contabilidade);
	destroiCatalogoCompras(compras);
}

int main(){

	CAT_PRODUTOS catprod = NULL; 	
	CAT_CLIENTES catcli =  NULL; 	
	CatalogoCompras compras =  NULL; 	
	CONTABILIDADE contabilidade =  NULL; 	

	int ficheiros_carregados=0,escolha,erros,escolhaficheiros;
	char filecompras[15],fileprodutos[15],fileclientes[15];

	do{	/*Opções disponíveis para o utilizador*/
		printf("||========||\n");
		printf("||  MENU  ||\n");	
		printf("||=============================================================================||\n");
		printf("||\t0\tSair\t\t\t\t\t\t\t\t||\n");
		printf("||-----------------------------------------------------------------------------||\n");
		printf("||\t1\tLer os 3 ficheiros\t\t\t\t\t\t||\n");
		printf("||-----------------------------------------------------------------------------||\n");
		printf("||\t2\tProdutos por letra\t\t\t\t\t\t||\n");
		printf("||-----------------------------------------------------------------------------||\n");
		printf("||\t3\tCompras de um produto num mês\t\t\t\t\t||\n");
		printf("||-----------------------------------------------------------------------------||\n");
		printf("||\t4\tProdutos não comprados\t\t\t\t\t\t||\n");
		printf("||-----------------------------------------------------------------------------||\n");
		printf("||\t5\tProdutos comprados por cliente\t\t\t\t\t||\n");
		printf("||-----------------------------------------------------------------------------||\n");
		printf("||\t6\tClientes por letra\t\t\t\t\t\t||\n");
		printf("||-----------------------------------------------------------------------------||\n");
		printf("||\t7\tCompras por intervalo de meses\t\t\t\t\t||\n");
		printf("||-----------------------------------------------------------------------------||\n");
		printf("||\t8\tClientes que compraram um certo produto\t\t\t\t||\n");
		printf("||-----------------------------------------------------------------------------||\n");
		printf("||\t9\tProdutos mais comprados por um certo cliente num mês\t\t||\n");
		printf("||-----------------------------------------------------------------------------||\n");
		printf("||\t10\tClientes que compraram em todos os meses do ano\t\t\t||\n");
		printf("||-----------------------------------------------------------------------------||\n");
		printf("||\t11\tGerar ficheiro Excel com compras mensais\t\t\t||\n");
		printf("||-----------------------------------------------------------------------------||\n");
		printf("||\t12\tN produtos mais vendidos no ano\t\t\t\t\t||\n");
		printf("||-----------------------------------------------------------------------------||\n");
		printf("||\t13\t3 produtos mais comprados por cliente\t\t\t\t||\n");
		printf("||-----------------------------------------------------------------------------||\n");
		printf("||\t14\tClientes que não compraram e produtos não comprados\t\t||\n");
		printf("||=============================================================================||\n");
		printf("Escolha:\n");
		scanf("%d", &escolha);

		switch(escolha){		
			case 0:
				break;	
			case 1:
				if(ficheiros_carregados){
					destroiCatalogos(catprod,catcli,compras,contabilidade);
					ficheiros_carregados = 0;
				}
				catprod = inicializaProdutos(); 	
				catcli = inicializaClientes(); 	
				compras = inicializaCompras();
				contabilidade = inicializa_contabilidade();
				
				erros=0;
				printf("Pressione:\n'1' -> Introduzir nomes dos ficheiros.\n'0' -> Ficheiros default.\n");
				scanf(" %d", &escolhaficheiros);
				if(!escolhaficheiros){
					strcpy(filecompras,"compras.txt");strcpy(fileprodutos,"produtos.txt");strcpy(fileclientes,"clientes.txt");

					printf("\nA ler os ficheiros...\n");
					erros += lerProdutos(catprod,contabilidade,fileprodutos);
					erros += lerClientes(catcli,fileclientes);
					erros += lerCompras(compras, catcli,catprod,contabilidade,filecompras);
				}
				else{
					printf("Ficheiros default: 'produtos.txt' 'clientes.txt' 'compras.txt'\n");
					printf("Atenção: Os ficheiros devem estar na pasta 'files'!\n");
					printf("Ficheiro de produtos: \n");
					scanf("%s", fileprodutos);
					printf("Ficheiro de clientes: \n");
					scanf("%s", fileclientes);
					printf("Ficheiro de compras: \n");
					scanf("%s", filecompras);
					
					printf("\nA ler os ficheiros...\n");
					erros += lerProdutos(catprod,contabilidade,fileprodutos);
					erros += lerClientes(catcli,fileclientes);
					erros += lerCompras(compras, catcli,catprod,contabilidade,filecompras);
				}
				if(erros){
					destroiCatalogos(catprod,catcli,compras,contabilidade);
					printf("Erro ao ler os ficheiros!\n");
					ficheiros_carregados = 0;
				}
				else ficheiros_carregados = 1;
				break;
			case 2:
				if(!ficheiros_carregados){printf("Atenção! Ainda não carregou os ficheiros!\n"); break;}
				query2(catprod);
				break;
			case 3:
				if(!ficheiros_carregados){printf("Atenção! Ainda não carregou os ficheiros!\n"); break;}
				query3(contabilidade);
				break;
			case 4:
				if(!ficheiros_carregados){printf("Atenção! Ainda não carregou os ficheiros!\n"); break;}
				query4(contabilidade);
				break;
			case 5:
				if(!ficheiros_carregados){printf("Atenção! Ainda não carregou os ficheiros!\n"); break;}
				query5(compras);
				break;
			case 6:
				if(!ficheiros_carregados){printf("Atenção! Ainda não carregou os ficheiros!\n"); break;}
				query6(catcli);
				break;
			case 7:
				if(!ficheiros_carregados){printf("Atenção! Ainda não carregou os ficheiros!\n"); break;}
				query7(contabilidade);
				break;
			case 8:
				if(!ficheiros_carregados){printf("Atenção! Ainda não carregou os ficheiros!\n"); break;}
                                query8(compras);
				break;
			case 9:
				if(!ficheiros_carregados){printf("Atenção! Ainda não carregou os ficheiros!\n"); break;}
				query9(compras);
				break;
			case 10:
				if(!ficheiros_carregados){printf("Atenção! Ainda não carregou os ficheiros!\n"); break;}
				query10(compras);
				break;
			case 11:
				if(!ficheiros_carregados){printf("Atenção! Ainda não carregou os ficheiros!\n"); break;}
				query11(compras,contabilidade);
				break;
			case 12:
				if(!ficheiros_carregados){printf("Atenção! Ainda não carregou os ficheiros!\n"); break;}
				query12(compras);
				break;
			case 13:
				if(!ficheiros_carregados){printf("Atenção! Ainda não carregou os ficheiros!\n"); break;}
				query13(compras);
				break;
			case 14:
				if(!ficheiros_carregados){printf("Atenção! Ainda não carregou os ficheiros!\n"); break;}
				query14(compras, catcli, contabilidade);	
				break;
			default:	/*Se a "escolha" não for um número válido das opções*/
				printf("Introduza uma opção válida\n");
				break;
		}
	
	}while(escolha);	/*Entra no ciclo se a "escolha" for diferente de 0*/
	
	if(ficheiros_carregados)
		destroiCatalogos(catprod,catcli,compras,contabilidade);
	printf("Programa terminado.\n");
	return 0;
}
