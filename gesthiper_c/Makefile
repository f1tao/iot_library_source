all: main clean

main: produtos clientes compras contabilidade
	gcc -g -ansi -Wall -Wextra -pedantic -O0 -o main ./main.c ./produtos ./clientes ./compras ./avl ./contabilidade
produtos: avl
	gcc -c -g -O0 ./libs/produtos.c -o produtos
clientes: avl
	gcc -c -g -O0 ./libs/clientes.c -o clientes
compras: avl
	gcc -c -g -O0 ./libs/compras.c -o compras
contabilidade: avl
	gcc -c -g -O0 ./libs/contabilidade.c -o contabilidade
avl:
	gcc -c -g -O0 ./libs/avl.c -o avl
clean:
	rm avl produtos clientes compras contabilidade
