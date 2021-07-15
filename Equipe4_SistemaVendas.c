//
// main.c
// Trabalho final de Linguagem de Programação 1
// Versao	v1.0.2409
//
// Created by Vitor Manoel, Jessica Cardoso, Filipe Silva e Reinilson Bispo on 23/09/19.
// Copyright © 2019. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <conio.h>

#define DIF_TABULACAO 40
#define N_GRUPOS 25
/*
*	crie o arquivo produtos.txt na pasta
*	depois so pegar e rodar
*/

//Novidades da versão
/*

	* Simplificação da funcao de cadastro de vendas, correçao de bugs e mais
	* implementada nova forma, simplificada, de gravar o grupo do produto
	* adicionada a frase "SEM MOVIMENTO" nos relatorios de produto, caso o mesmo nao contenha informacao
	* modificado os campos de variáveis auxiliares nas funcoes de produto, passamos a utilizar a prorpia
estrutura como variaveis auxiliares

	att. 20-09
	* modificado todas as funcoes que envolvem produtos
		----- pendencias ----
		* todas as funcoes que eram do tipo void passaram a retornar um short
		* funcoes que serao propicias a erro, retornarao 0 se houver erro na execucao e 1 caso executem de forma adequada
		* novas nomenclaturas para os ponteiros globais pArq_Venda, pArq_Produto, pArq_Cliente


	att. 23-09
		* adicionado o relatorio produto mais vendido
		* funcao alterar valor de um produto foi modificada para perguntar se deseja alterar tambem a pontuacao do produto
*/
// Rel exibir produtos cadastrados

typedef struct cliente{
	char cpf[12];
	char nome[50];
	int dataNascimento;
	float pontuacao;
}InfoCliente;

typedef struct produto{
	int codigoBarras;
	char nome[50];
	char undMedida[3];
	int grupo;
	float pontuacao;
	float valor;
}InfoProduto;

typedef struct RegistroVenda{
	char verificador;
	long int codigoVenda;
	unsigned int data;
	char codigoCliente[12];
	float descAplicado;
	double valorTotal;
}RegistroVenda;

typedef struct ExtratoVenda{
	char verificador;
	int codigoProduto;
	unsigned int qtdProduto;
	float valorProduto;
	float ptsProduto;
	double valorTotal;
}ExtratoVenda;

typedef struct item{
	ExtratoVenda extrato;
	struct item *proxItem;
	char nomeProduto[50];
}Item;

typedef struct ListaClientes{
    char nome[50];
    double valorCompra;
    unsigned nVisitas;
    struct ListaClientes *proxCliente;
    struct ListaClientes *predCliente;
}ListaClientes;

typedef struct ListaProdutos{
	char nome[50];
	int codProduto;
	unsigned qtdProdVendidos;
	struct ListaProdutos *proxProduto;
	struct ListaProdutos *predProduto;
}ListaProdutos;

/*Ponteiros para manipulacao dos arquivos do sistema*/
FILE *pArq_Venda;
FILE *pArq_Produto;
FILE *pArq_Cliente;


/*Funcoes de manipulacao de produtos*/
short cadastrarProduto();
short alterarValor_Produto(int codInformado);
short checkProduto(int codProduto, unsigned codGrupo);
	//Funcoes de menu de produtos
		void menu_GrupoProduto();
		void menu_RelProdutosPorValor();


/*Funcoes de manipulacao de clientes*/
void cadastrarCliente(void);
short checkCliente(char codigoCliente[]);
void atualizarPontuacao(char cod_Cliente[], float pontuacao);


/*Funcoes de manipulacao de vendas*/
short cadastrarVenda();
unsigned int gerarData(unsigned int dataRegistrada);
	//Funcoes de manipulacao de itens
		short selecaoProdutos(Item **primeiroItem, short idadeCliente);
		short checkIdadeCliente(char codCliente[], unsigned dataVenda);
		short adicionarItem(Item **primeiroItem, ExtratoVenda extrato, char nomeProduto[]);
		float gravarItens(RegistroVenda *novaVenda, Item **primeiroItem);
		short removerItens(Item **primeiroItem, int codItem, unsigned qtdRemovida);
		void listarItens(Item *itens);
	//Funcoes de manipulacao de clientes
		short loginCliente(char codCliente[], char opcao);
		float aplicarDesconto(RegistroVenda *novaVenda, char opcao);


/*Relatorios do programa*/
	//Relatorios de clientes
		void rel_clientesCadastrados();
		void rel_Clientes();
		void adicionarCliente_rel(ListaClientes **primeiroCliente);
		void encadearClientes(ListaClientes **primeiroCliente, char nomeCliente[], double valorTotal);
		void relClienteOrdenado(ListaClientes **primeiroCliente, char opcao);
	//Relatorios de produtos
		void rel_produtosPorValor();
		void rel_produtosCadastrados();
		short rel_produtoPorGrupo(int grupoInformado);
		void rel_produtosMaisVendidos();
		short gerarLista_Produtos(ListaProdutos **primeiroProduto);
		void encadearProdutos(ListaProdutos **primeiroProduto, ExtratoVenda prodVendido);
	//Relatorios de faturamento
		void faturamentoPeriodo();


//Funcoes auxiliares
short formatarNome(char nome[], char delimitador);
void normalizarString(char strNormal[]);
void retirarUnderlineStr(char strUnderline[]);
void espacamentoColunas(char strBase[]);
unsigned int dataInvertida(unsigned int data);





int main(){
	char opcaoInf_Produto;
	char opcaoInf_Rel_Produto = 0;
	short opcaoInf;
	int codProduto, codGrupo;


	if(!(pArq_Venda = fopen("Equipe4_venda.txt", "a+"))) exit(1);
	char checkEOF = fgetc(pArq_Venda);
	if(fgetc(pArq_Venda) == EOF)
		fprintf(pArq_Venda, "%s\n", "  1000000 20190823 00000000000 0.00 0.00");
	fclose(pArq_Venda);



	for(opcaoInf = -1;opcaoInf != 0;){ //inicio laço infinito
		system("cls");
		printf("____________\n\n      BEM VINDO A REDE DE LOJAS VALMARTE\n   _____________\n\n");
		printf("==========================================\n\n");
		printf(" 1-> =========== REALIZAR VENDAS =========\n\n");
		printf(" 2-> ==== CADASTRAR/ATUALIZAR PRODUTOS ===\n\n");
		printf(" 3-> ========== CADASTRAR CLIENTES =======\n\n");
		printf(" 4-> ============ RELATORIOS =============\n\n");
		printf(" 0-> ============== SAIR =================\n\n");
		printf("Informe uma opcao: ");
		scanf(" %d", &opcaoInf);

		switch(opcaoInf){
			case 1:
				cadastrarVenda();
			break;
			case 2:
				system("cls");
				do{
					printf(" 1-> ============== CADASTRAR PRODUTO ==============\n\n");
					printf(" 2-> ======= ATUALIZAR O VALOR DE UM PRODUTO =======\n\n");
					printf(" 0-> =================== SAIR ======================\n\n");
					printf("Informe uma opcao: ");
					scanf(" %c", &opcaoInf_Produto);
					system("cls");
				}while((opcaoInf_Produto != '0') && (opcaoInf_Produto != '1') && (opcaoInf_Produto != '2'));
					system("cls");
					switch(opcaoInf_Produto){
						case '1':
							system("cls");
							setbuf(stdin, NULL);
							cadastrarProduto();
						break;
						case '2':
							system("cls");
							setbuf(stdin, NULL);

							do{
								system("cls");
								menu_GrupoProduto();
								printf("\nInforme um grupo: ");
								scanf(" %d", &codGrupo);
								rel_produtoPorGrupo(codGrupo);
							}while(codGrupo < 1 || codGrupo > 25);

							do{
	       						printf("\nInforme o codigo do produto que deseja atualizar o valor: ");
	        					scanf(" %d", &codProduto);
	    					}while(!checkProduto(codProduto, codGrupo));
							alterarValor_Produto(codProduto);
						break;
					}
			break;

			case 3:
				cadastrarCliente();
			break;

			case 4:
				system("cls");
				setbuf(stdin, NULL);
				char opRelatorio;
				do{
					printf("\n1-> ======== RELATORIO DE FATURAMENTO =====\n");
					printf("\n2-> ========= RELATORIOS DE PRODUTOS ======\n");
					printf("\n3-> ========= RELATORIOS DE CLIENTES ======\n");
					printf("\n0-> ================ SAIR =================\n");
					printf("\nInforme uma opcao: ");
					scanf(" %c", &opRelatorio);
					system("cls");
				}while((opRelatorio != '0') && (opRelatorio != '1') && (opRelatorio != '2') && (opRelatorio != '3'));
				switch(opRelatorio){
					case '1':
						faturamentoPeriodo();
					break;

					case '2':
						system("cls"); //RELATORIOS PRODUTOS
						do{
							printf("\n1-> ======= PRODUTOS POR VALOR =======\n");
							printf("\n2-> ======= PRODUTOS POR GRUPO =======\n");
							printf("\n3-> ======== TODOS OS PRODUTOS =======\n");
							printf("\n4-> ===== PRODUTOS MAIS VENDIDOS =====\n");
							printf("\n0-> ============== SAIR ==============\n");
							printf("\nInforme uma opcao: ");
							scanf(" %c", &opcaoInf_Rel_Produto);
							system("cls");
						}while((opcaoInf_Rel_Produto != '0') && (opcaoInf_Rel_Produto != '1') && (opcaoInf_Rel_Produto != '2')
							&& (opcaoInf_Rel_Produto != '3') && opcaoInf_Rel_Produto != '4');
							switch (opcaoInf_Rel_Produto){
								case '1':
									system("cls");
									rel_produtosPorValor();
								break;

								case '2':
									system("cls");
									int grupoInformado;
									printf("========== PRODUTOS POR SESSAO ==========\n\n");
									menu_GrupoProduto();
									printf("\nInforme um grupo: ");
									scanf(" %d", &grupoInformado);
									rel_produtoPorGrupo(grupoInformado);
								break;

								case '3':
									rel_produtosCadastrados();
								break;

								case '4':
									rel_produtosMaisVendidos();
								break;
						}
					break;
					case '3':
						rel_Clientes();
					break;
				}
			break;
		}
    	system("pause");
	}//FIM laço infinito

	return 0;
}


/*Funcoes de manipulacao de produtos*/
short cadastrarProduto(){
	/*Captura todas as informacoes referentes a um novo produto que se deseje cadastrar
	  Confere se o produto informado ja existe, caso nao exista, grava o novo produto no arquivo de produtos
	  retorn 1 se obtiver sucesso em todas as operacoes*/
	InfoProduto cadastroProduto, auxProduto;
	char v_EhVazio;

	setbuf(stdin, NULL); //limpeza do buffer do teclado

	//nome do produto
	printf("Informe o nome do produto: ");
	scanf("%[^\n]s", cadastroProduto.nome);
	normalizarString(cadastroProduto.nome);

	//gera um novo codigo e valida a entrada do produto pelo nome
	if(!(pArq_Produto = fopen("Equipe4_produto.txt", "a+"))) exit(1);

	if((v_EhVazio = fgetc(pArq_Produto)) == EOF) auxProduto.codigoBarras = 9999;
	else{
		while((fscanf(pArq_Produto, "%d %s %d %f %f\n", &auxProduto.codigoBarras, auxProduto.nome, &auxProduto.grupo,
			&auxProduto.pontuacao, &auxProduto.valor)) != EOF){
				if(strcmp(auxProduto.nome, cadastroProduto.nome) == 0){
				printf("JA EXISTE UM PRODUTO CADASTRADO COM ESSE NOME!\n");
				return 0; //retorna 0 se o produto a ser cadastrado ja existe no arquivo
			}
		}//FIM while
	}
	fclose(pArq_Produto);
	auxProduto.codigoBarras++;

	//grupo do produto
	system("cls");
	int opInf_GpProduto = 0;
	menu_GrupoProduto();
	printf("\nInforme o grupo do produto: ");
	scanf(" %d", &cadastroProduto.grupo);

	//pontuacao do produto
	system("cls");
	printf("Informe a pontuacao do produto: ");
	scanf(" %f", &cadastroProduto.pontuacao);

	//valor do produto
	printf("\nInforme o valor do produto: ");
	scanf(" %f", &cadastroProduto.valor);

	//escrever os dados no arquivo
	if(!(pArq_Produto = fopen("Equipe4_produto.txt", "a"))) exit(1);

	//codigo produto
	if(auxProduto.codigoBarras == 10000) //se for o primeiro produto. há um bug ao inserir no arquivo a primeira linha sem o espaço no inicio
		fprintf(pArq_Produto, " %d ", auxProduto.codigoBarras);
	else
		fprintf(pArq_Produto, "%d ", auxProduto.codigoBarras);

	//restante dos dados
	fprintf(pArq_Produto, "%s %d %.2f %.2f\n", cadastroProduto.nome, cadastroProduto.grupo * 100,
		cadastroProduto.pontuacao, cadastroProduto.valor);

	fclose(pArq_Produto);
	return 1;
}//FIM function

short alterarValor_Produto(int codInformado){
	/*Recebe o codito referente ao produto que se deseja alterar o valor
	  Cria um arquivo temporario onde sao armazenadas copias dos demais produtos
	  reescreve o produto de codigo informado com o novo valor informado
	  retorna 1 caso obtenha sucesso em todas as operacoes*/
	FILE *pArq_ProdutoTemp;
	InfoProduto produtoArq;
	float novoValor, novaPontuacao;
	short alterarPnts = 0;
    char strPrint[50];


	system("cls");
	if(!(pArq_ProdutoTemp = fopen("auxEquipe4_produto.txt", "a"))) exit(1);
	if(!(pArq_Produto = fopen("Equipe4_produto.txt", "r"))) exit(1);

	while((fscanf(pArq_Produto, "%d %s %d %f %f\n", &produtoArq.codigoBarras, produtoArq.nome, &produtoArq.grupo,
		&produtoArq.pontuacao, &produtoArq.valor)) != EOF){
		if(codInformado == produtoArq.codigoBarras){
			printf("========================== PRODUTO =======================\n\n");
			strcpy(strPrint, produtoArq.nome);
			retirarUnderlineStr(strPrint);
			printf("%d - %s %.2f R$%.2f\n", produtoArq.codigoBarras, strPrint, produtoArq.pontuacao, produtoArq.valor); //print do produto
			printf("\n==========================================================\n");
			printf("\nInforme o novo valor: ");
			scanf(" %f", &novoValor);

			printf("\nDeseja tambem alterar a pontuacao do produto?\n\n1->SIM\n2->NAO\n");
			do{
				printf("\nInforme uma opcao: ");
				scanf(" %d", &alterarPnts);
			}while(alterarPnts < 1 || alterarPnts > 2);
			//alterar pontuacao
			if(alterarPnts == 1){
				printf("\nInforme a nova pontuacao do produto: ");
				scanf(" %f", &novaPontuacao);
				fprintf(pArq_ProdutoTemp, "%d %s %d %.2f %.2f\n", produtoArq.codigoBarras, produtoArq.nome, produtoArq.grupo,
		novaPontuacao, novoValor);
			}else
			fprintf(pArq_ProdutoTemp, "%d %s %d %.2f %.2f\n", produtoArq.codigoBarras, produtoArq.nome, produtoArq.grupo,
		produtoArq.pontuacao, novoValor);
		}else
			fprintf(pArq_ProdutoTemp, "%d %s %d %.2f %.2f\n", produtoArq.codigoBarras, produtoArq.nome, produtoArq.grupo,
		produtoArq.pontuacao, produtoArq.valor);
	}//FIM while

	fclose(pArq_ProdutoTemp); fclose(pArq_Produto);
	remove("Equipe4_produto.txt");
	rename("auxEquipe4_produto.txt", "Equipe4_produto.txt");
	return 1;
}//FIM function

short checkProduto(int codProduto, unsigned codGrupo){
	/*Recebe como parametro um numero inteiro contendo o codigo e o grupo do produto que se deseja validar;
	  Realiza busca no arquivo de texto que contem os registros de produtos;
	  Retorna 1 caso encontre o codigo e grupo de produto igual ao qual se buscava e 0 caso contrario.*/
    InfoProduto produtoArq;

    if(!(pArq_Produto = fopen("Equipe4_produto.txt", "r"))){
    	printf("Erro ao abrir o arquivo - Equipe4_produto.txt");
    	exit(1);
    }
    codGrupo *= 100;
	while(fscanf(pArq_Produto,"%d %s %d %f %f" , &produtoArq.codigoBarras,produtoArq.nome,
			&produtoArq.grupo, &produtoArq.pontuacao, &produtoArq.valor) != EOF){
		if(produtoArq.codigoBarras == codProduto && produtoArq.grupo == codGrupo){
            fclose(pArq_Produto);
			return 1;
		}
	}
    fclose(pArq_Produto);
	return 0;
}

void menu_GrupoProduto(){
	/*Rotina de impressao de grupos dos produtos*/
	printf("01-> Acondicionamento e Embalagem\n");
	printf("02-> Alimento\n");
	printf("03-> Alimento Animal\n");
	printf("04-> Arte, Papelaria e Armarinho\n");
	printf("05-> Bebida\n");
	printf("06-> Bebidas Alcoolicas\n");
	printf("07-> Celulares e Telefones\n");
	printf("08-> Drogas\n");
	printf("09-> Eletrodomesticos\n");
	printf("10-> Equipamento de Audio e Video\n");
	printf("11-> Equipamento de Informatica\n");
	printf("12-> Esporte e Fitness\n");
	printf("13-> Ferramenta de Manutencao em Geral\n");
	printf("14-> Games\n");
	printf("15-> Higienizacao Pessoal\n");
	printf("16-> Hotalicas\n");
	printf("17-> Joias e Relogios\n");
	printf("18-> Livros, Revistas e Comics\n");
	printf("19-> Material de Cozinha\n");
	printf("20-> Material de Escritorio\n");
	printf("21-> Material de Limpeza\n");
	printf("22-> Medicamento\n");
	printf("23-> Musica, Filmes e Seriados\n");
	printf("24-> Semente, Muda de Plantas\n");
}//FIM function

void menu_RelProdutosPorValor(){
	/*Menu de selecao de intervalo para impressao de produtos por valor*/
	system("cls");
	printf("========== RELATORIO DE PRODUTOS POR VALOR ==========\n\n");
	printf("1-> MAIOR OU IGUAL AO VALOR INFORMADO\n");
	printf("2-> MENOR OU IGUAL AO VALOR INFORMADO\n");
	printf("3-> IGUAL AO VALOR INFORMADO\n");
	printf("4-> ENTRE UM INTERVALO INFORMADO\n");
	printf("\n========================================================\n");
}//FIM funtion




/*Funcoes de manipulacao de clientes*/
void cadastrarCliente(void){

	/*Essa função não recebe nada como parâmetro, e é do tipo Void, pois não irá retornar nada.
	Passando uma variavél do tipo da struct (infoClientes) inserimos os dados de entrada nas
	variaveis e inserimos no arquivo.
	Obs-1: Ao inserirmos o cpf a funcão (checkCliente) para verificar se o cliente já esta ou não cadastrado
	Obs: A função tem um mascará na hora de digitar data de nascimento e cpf(codigo do cliente)
	que irá adicionar "/" na data e ". -" nos cpf(código do cliente)*/

	system("cls");
	InfoCliente cliente;

	cliente.pontuacao = 0.0;

    FILE *ptrArquivo = fopen("Equipe4_cliente.txt", "a");
		if(ptrArquivo == NULL) exit(1);

	printf("Digite o nome do cliente:\n");
	scanf(" %[^\n]s", cliente.nome);
	normalizarString(cliente.nome);

    printf("Digite o codigo do cliente:\n");
	for(short indice = 0; indice < 11; indice++){
        cliente.cpf[indice] = getche();
        if(indice == 2 || indice == 5)
            printf(".");
        else if(indice == 8)
            printf("-");
	}
	cliente.cpf[11] = '\0';
	printf("\n");

	//Verificação do cliente por código.

	if(checkCliente(cliente.cpf)){
		 printf("\n\nErro, cliente ja cadastrado\n\n");
		 system("pause");
		return;
	}

	printf("Digite o data de nascimento do cliente:\n");
	cliente.dataNascimento = 0;
    for(short indice = 0; indice < 8; indice++){
        cliente.dataNascimento *= 10;
        cliente.dataNascimento += (getche() - 48);
        if(indice == 1 || indice == 3)
            printf("/");
	}
	printf("\n");

	system("cls");
	//coloquei espaço no lugar dos ;
	//pontuação do cliente irá para o reltório com duas casas decimais. %.2f
	fprintf(ptrArquivo,"%s %s %d %.2f\n", cliente.cpf, cliente.nome, cliente.dataNascimento, cliente.pontuacao);
	fclose(ptrArquivo);
	printf("\t\t\tDADOS INSERIDOS NO REGISTRO:\n\n");
	printf("%s\n", cliente.nome);
	printf("%s\n", cliente.cpf);
	printf("%d\n",cliente.dataNascimento);

}

short checkCliente(char codCliente[]){
	/*Recebe como parametro uma string contendo o codigo de cliente que se deseja validar;
	  Realiza busca no arquivo de texto que contem os registros de clientes;
	  Retorna 1 caso encontre o codigo de cliente ao qual se buscava e 0 caso contrario.*/
	char codCadastrado[20];
	char strResto[100];

	if(!(pArq_Cliente = fopen("Equipe4_cliente.txt", "r"))){
		printf("Erro ao abrir o arquivo - Equipe4_cliente.txt");
		exit(1);
	}
	while((fscanf(pArq_Cliente,"%s %[^\n]s", codCadastrado, strResto) != EOF)){
		if(strcmp(codCadastrado, codCliente) == 0){
            fclose(pArq_Cliente);
			return 1;
		}
	}
    fclose(pArq_Cliente);
	return 0;
}

void atualizarPontuacao(char cod_Cliente[], float pontuacaoAtt){
	/*Recebe uma string referente ao codigo do cliente, e a nova pontuacao do cliente
	  Cria um arquivo temporario onde sao copiados todos os registros de cliente do arquivo de cliente
	  escreve no arquivo temporario, o cliente cujo codigo foi informado com sua nova pontuacao
	  por fim o aquivo temporario passa a ser o arquivo principal de registro de clientes*/
	FILE *pArq_Aux;

	pArq_Cliente = fopen("Equipe4_cliente.txt", "r");
	if(pArq_Cliente == NULL) exit(EXIT_FAILURE);
	pArq_Aux = fopen("Equipe4_clienteTemp.txt", "a");
	if(pArq_Aux == NULL) exit(EXIT_FAILURE);

	char cpf[12];
	char nome[50];
	int dataNascimento;
	float pontuacao;


	while(fscanf(pArq_Cliente, "%s %s %d %f", cpf, nome, &dataNascimento, &pontuacao) != EOF){

		if(!strcmp(cpf, cod_Cliente))
			fprintf(pArq_Aux, "%s %s %d %.2f\n", cpf, nome, dataNascimento, pontuacaoAtt);
		else
			fprintf(pArq_Aux, "%s %s %d %.2f\n", cpf, nome, dataNascimento, pontuacao);
	}
	fclose(pArq_Cliente); fclose(pArq_Aux);
	remove("Equipe4_cliente.txt");
	rename("Equipe4_clienteTemp.txt", "Equipe4_cliente.txt");
}//FIM function




/*Funcoes de manipulacao de vendas*/
short cadastrarVenda(){
	/*Apresenta todos os menus de selecao, e interacao com o cliente capturando os dados e os escrevendo no arquivo de vendas
	  Retorna 1 caso obtenha sucesso em todas as operacoes*/
	RegistroVenda vendaArq;
	Item *primeiroItem = NULL;

    //Verificando se existem produts cadastrados
    if(!(pArq_Produto = fopen("Equipe4_produto.txt", "r"))){
		printf("Erro ao abrir o arquivo - Equipe4_produto.txt");
		exit(1);
	}
    if(fgetc(pArq_Produto) == EOF){
        printf("\n\nNao eh possivel cadastrar vendas pois nao existem produtos cadastrados.\n\n");
        system("pause");
        return 0;
    }
	//Capturando informacoes da ultima venda cadastrada - ultima linha do arquivo de texto
	if(!(pArq_Venda = fopen("Equipe4_venda.txt", "r"))){
		printf("Erro ao abrir o arquivo - Equipe4_venda.txt");
		exit(1);
	}
	while(fgetc(pArq_Venda) != EOF)
		fscanf(pArq_Venda,"%c %ld %u %s %f %lf" , &vendaArq.verificador, &vendaArq.codigoVenda, &vendaArq.data, vendaArq.codigoCliente, &vendaArq.descAplicado, &vendaArq.valorTotal);
    fclose(pArq_Venda);

	//Declaração e inicialização da estrutura de registro da nova venda
	RegistroVenda novaVenda;
	novaVenda.data = gerarData(vendaArq.data);
	novaVenda.codigoVenda = ++vendaArq.codigoVenda;
	novaVenda.valorTotal = 0;
	novaVenda.descAplicado = 0;

	//Recebe os pontos referente aos produtos selecionados adicionado aos pontos contidos no registro de cliente
	float pontosAtualizadosCliente = 0;

	char opcao;

	system("cls");
	printf("\t\t========= SISTEMA DE VENDAS DA REDE DE LOJAS VALMARTE =========\n\n");
	short tipoCliente;
	do{
		printf("\t\t========= SECAO DE IDENTIFICACAO DO CLIENTE =========\n\n");
		printf("\n\nEscolha uma das opcoes de login?\n\n[1]Realizar Cadastro.      [2]Inserir Codigo.      [3]Nao se identificar.      [0]Sair do sistema.");
		printf("\n\nInforme a opcao desejada: ");
		scanf(" %c", &opcao);
		system("cls");
		if(opcao != '1' && opcao != '2' && opcao != '3')
			printf("A opcao informada eh invalida, tente outra vez\n");
		else if(!opcao)
			return 0;
		else
			tipoCliente = loginCliente(novaVenda.codigoCliente, opcao);
		system("cls");
	}while(opcao != '0' && opcao != '1' && opcao != '2' && opcao != '3');


	printf("\t\t========= NAVEGUE PELAS SECOES DE PRODUTOS E ESCOLHA SEUS ITENS =========\n\n");
	short idadeCliente = 0;
	if(tipoCliente)
        idadeCliente = checkIdadeCliente(novaVenda.codigoCliente, novaVenda.data);

	do{
		printf("\n\nDeseja selecionar produtos?\n\n[1]Sim, desejo.      [2]Nao, estou satisfeito.      [3]Ver carrinho.");
		printf("\n\nInforme a opcao desejada: ");
		scanf(" %c", &opcao);
		if(opcao != '1' && opcao != '2' && opcao != '3')
			printf("\nA opcao informada eh invalida, tente outra vez\n");
		else if(opcao == '1')
			selecaoProdutos(&primeiroItem, idadeCliente);
		else if(opcao == '3')
			listarItens(primeiroItem);
		system("pause");
		system("cls");
	}while(opcao != '2');

	//Nenhum produto selecionado
	if(!primeiroItem){
		printf("\nNao existem produtos para a compra ser concluida.\n\n");
		system("pause");
		return 0;
	}

	printf("\t\t========= CARRINHO DE COMPRAS =========\n\n");
	int codItem;
	unsigned qtdRemovida;
	do{
		listarItens(primeiroItem);
		printf("\n\nDeseja alterar algum item?\n\n[1]Sim, desejo.      [2]Nao, estou satisfeito.");
		printf("\n\nInforme a opcao desejada: ");
		scanf(" %c", &opcao);
		if(opcao == '1'){
            system("cls");
            listarItens(primeiroItem);
			printf("\n\nDigite o codigo e a quantidade do produto que se deseja alterar:\n");
			printf("\n\nDigite o codigo: ");
			scanf("%d", &codItem);
			printf("\n\nDigite a quantidade: ");
			scanf("%u", &qtdRemovida);
			system("cls");
			removerItens(&primeiroItem, codItem, qtdRemovida);
		}
		if(opcao != '1' && opcao != '2')
			printf("\nA opcao informada eh invalida, tente outra vez\n");
		system("cls");
	}while(opcao != '2');

	//Todos os produtos foram deletados
	if(!primeiroItem){
		printf("\n\nNao existem produtos para a compra ser concluida.\n\n");
		system("pause");
		return 0;
	}

	listarItens(primeiroItem);
	printf("\n\nDeseja confirmar a compra?\n\n[1]Sim, Pagar.      [2]Nao, Desistir da compra.");
	printf("\n\nInforme a opcao desejada: ");
	scanf(" %c", &opcao);
	printf("\n\t\t========= OBRIGADO POR UTILIZAR NOSSO SISTEMA =========\n\n");
	system("pause");
	system("cls");
	if(opcao == '1'){
		pontosAtualizadosCliente = gravarItens(&novaVenda, &primeiroItem);
		if(tipoCliente){
			printf("====== BEM VINDO AO SISTEMA DE DESCONTO E FIDELIDADE DA LOJAS VALMARTE ======\n\n");
			printf("\n\nDeseja participar do programa?\n\n[1]Sim, atualizar minha pontuacao.      [2]Nao, nao ligo.");
			printf("\n\nInforme a opcao desejada: ");
			scanf(" %c", &opcao);
			if(opcao == '1'){
				pontosAtualizadosCliente += aplicarDesconto(&novaVenda, opcao);
       			atualizarPontuacao(novaVenda.codigoCliente, pontosAtualizadosCliente);
       		}
		}
		if(!(pArq_Venda = fopen("Equipe4_venda.txt", "a")))
			exit(1);
		fprintf(pArq_Venda, "~ %ld %d %s %.2f %.2lf\n",novaVenda.codigoVenda,novaVenda.data,novaVenda.codigoCliente,novaVenda.descAplicado,novaVenda.valorTotal);
		fclose(pArq_Venda);
	}
	return 1;
}

unsigned int gerarData(unsigned int dataRegistrada){
	/*Recebe a ultima data de vendas registrada;
	Gera um valor aleatorio para o campo dia entre 1 e 31;
	Valida a data gerada, segundo o calendario gregoriano e ultima data registrada;
	Formata a nova data, a tornando do tipo aaaammdd*/

	unsigned int dataGerada = 1 + rand() % 31;
	//enum mesesDoAno{Janeiro = 1, Fevereiro, Marco, Abril, Maio, Junho, Julho, Agosto, Setembro, Outubro, Novembro, Dezembro}mesRegistrado;

	//Segmentando a data registrada no arquivo em: dia, mes, ano
	short diaRegistrado = dataRegistrada%100;
	short mesRegistrado = (dataRegistrada/100)%100;
	short anoRegistrado = dataRegistrada/10000;
	//Validacao e formantacao a nova data
	//****************Utilizar enum para comparacao de meses****************
	if((dataGerada <= diaRegistrado) || (dataGerada == 31 && (mesRegistrado == 4 || mesRegistrado == 6 || mesRegistrado == 9 || mesRegistrado == 11)
		|| (mesRegistrado == 2 && dataGerada > 28))){
		if(mesRegistrado == 12)
			dataGerada = (anoRegistrado*10000)+10101;
		else
			dataGerada = (anoRegistrado*10000)+(mesRegistrado*100)+101;
	}
	else
		dataGerada = (anoRegistrado*10000)+(mesRegistrado*100)+dataGerada;
	return dataGerada;
}

short selecaoProdutos(Item **primeiroItem, short idadeCliente){
	/* Recebe um ponteiro marcador de primeiro item da lista de produtos selecionados, e um inteiro referente a idade do cliente que ira selecionar os produtos
	   Exibe ao cliente os produtos desejados, e chama a funcao de adicionar itens apos o cliente selecionar um produto
	   retorna 1 caso obtenha sucesso em todas as operacoes*/

	ExtratoVenda extrato;
	InfoProduto produtoArq;
	produtoArq.codigoBarras = 0;
	unsigned grupoProdutos;
	char opcao;

    do{
    	system("cls");
    	printf("\t\t=============== ESCOLHA A SECAO QUE CONTEM O(S) PRODUTO(S) DESEJADOS =============== \n\n");
    	menu_GrupoProduto();
    	printf("\n\nInforme o grupo desejado: ");
    	scanf("%d", &grupoProdutos);
    	if(((grupoProdutos == 6 || grupoProdutos == 8) && idadeCliente < 18) && idadeCliente)
    		printf("\n\nVoce ainda nao tem idade para isso, escolha outra secao\n\n");
        if(!grupoProdutos || grupoProdutos > N_GRUPOS)
            printf("\n\nA opcao digitada eh invalida, tente novamente\n");
        system("pause");
	}while((!grupoProdutos || grupoProdutos > N_GRUPOS) || ((grupoProdutos == 6 || grupoProdutos == 8) && idadeCliente < 18 && idadeCliente));

		do{
			rel_produtoPorGrupo(grupoProdutos);
			printf("\n\nO produto desejado esta nesta secao?\n[1]Sim, selecionar produto.      [2]Nao, sair da secao.");
			printf("\n\nInforme a opcao desejada: ");
			scanf(" %c", &opcao);

			if(opcao == '1'){
                rel_produtoPorGrupo(grupoProdutos);
				printf("\n\nInforme o codigo do produto: ");
				scanf("%d", &extrato.codigoProduto);
				system("cls");

				//Busca das iformacoes do produto no arquivo, caso ele esteja cadastrado
				if(!(checkProduto(extrato.codigoProduto, grupoProdutos))){
                    printf("\n\nO codigo informado nao esta vinculado a nehum produto da secao informada\n\n");
					system("pause");
				}
				else{
					if(!(pArq_Produto = fopen("Equipe4_produto.txt", "r"))){
						printf("Erro ao abrir o arquivo - Equipe4_produto.txt");
						exit(1);
					}

					while(produtoArq.codigoBarras != extrato.codigoProduto)
						fscanf(pArq_Produto,"%d %s %d %f %f" , &produtoArq.codigoBarras,produtoArq.nome, &produtoArq.grupo, &extrato.ptsProduto, &extrato.valorProduto);
					fclose(pArq_Produto);

					retirarUnderlineStr(produtoArq.nome);
					printf("Produto Selecionado: [%s];\nValor Unitario: [%.2f];\nPontuacao do Produto: [%.2f].", produtoArq.nome, extrato.valorProduto, extrato.ptsProduto);

					printf("\n\nInforme a quantidade: ");
					scanf("%u", &extrato.qtdProduto);
					extrato.valorTotal = extrato.qtdProduto * extrato.valorProduto;

					printf("\n\nO VALOR TOTAL DESTE ITEM EH: [%.2lf]", extrato.valorTotal);

					printf("\nDESEJA CONFIRMAR A INSERCAO DO ITEM NO CARRINHO?\n[1]Sim, desejo.       [2]Nao, selecionar outro item.");
					printf("\n\nInforme a opcao desejada: ");
					scanf(" %c", &opcao);
					if(opcao == '1')
						adicionarItem(primeiroItem, extrato, produtoArq.nome);
				}
			system("cls");
			printf("Ainda deseja selecionar itens desta secao?\n[1]Sim, desejo.       [2]Nao, sair da secao.");
			printf("\n\nInforme a opcao desejada: ");
			scanf(" %c", &opcao);
			system("cls");
			}
		}while(opcao == '1');
	return 1;
}

short checkIdadeCliente(char codCliente[], unsigned dataVenda){
	//Calcula e retorna a idade do cliente, a partir da sua data de nascimento cadastrada e data vigente da venda
	InfoCliente clienteArq;

	if(!(pArq_Cliente = fopen("Equipe4_cliente.txt", "r"))){
		printf("Erro ao abrir o arquivo - Equipe4_cliente.txt");
		exit(1);
	}
	while((fscanf(pArq_Cliente,"%s %s %u %f", clienteArq.cpf, clienteArq.nome, &clienteArq.dataNascimento, &clienteArq.pontuacao) != EOF)){
		if(!(strcmp(clienteArq.cpf, codCliente))){
            fclose(pArq_Cliente);
            clienteArq.dataNascimento = dataInvertida(clienteArq.dataNascimento);
            fclose(pArq_Cliente);
			return (dataVenda - clienteArq.dataNascimento)/10000;
		}
	}
}

short adicionarItem(Item **primeiroItem, ExtratoVenda extrato, char nomeProduto[]){
	/* Recebe uma estrutura do tipo extrato que contem os dados referentes a um produto selecionado
	aloca memoria para um novo item e o aloca no final da lista de produtos selecionados, caso ja nao esteja na lista
	caso ja esteja os campos do item original sao apenas alterados
	retorna 1 caso obtenha sucesso em todas as operacoes*/
	Item *itemNovo;
	Item *itemAux;

	itemNovo = (Item*)malloc(sizeof(Item));
	if(itemNovo == NULL){
		printf("Falha no processo de alocacao de memoria para lista de itens");
		exit(1);
	}

	itemNovo->extrato = extrato;
	strcpy(itemNovo->nomeProduto, nomeProduto);
	itemNovo->proxItem = NULL;

	if(!(*primeiroItem)){
		*primeiroItem = itemNovo;
		return 1;
	}

  	itemAux = *primeiroItem;
	while(itemAux->proxItem && itemAux->extrato.codigoProduto != itemNovo->extrato.codigoProduto)
		itemAux = itemAux->proxItem;

    if(itemAux->extrato.codigoProduto == itemNovo->extrato.codigoProduto){
        itemAux->extrato.qtdProduto += itemNovo->extrato.qtdProduto;
        itemAux->extrato.valorTotal += itemNovo->extrato.valorTotal;
        free(itemNovo);
    }
    else
        itemAux->proxItem = itemNovo;
	return 1;
}

float gravarItens(RegistroVenda *novaVenda, Item **primeiroItem){
	/* Realiza a gravação, no arquivo de venda, de todos os produtos selecionados. Ao gravar um produto, libera amemoria ocupada por ele
	   Soma a pontuacao gerada por cada item selecionado pelo usuario e retorna a pontuacao referente a venda atual.*/

	Item *limparMemoria;
    float pontuacaoGerada = 0;

	if(!(pArq_Venda = fopen("Equipe4_venda.txt", "a"))){
		printf("Erro ao abrir o arquivo - Equipe4_venda.txt");
		exit(1);
	}
	while(*primeiroItem){
		limparMemoria = *primeiroItem;
		fprintf(pArq_Venda,"* %d %.2f %d %.2f %.2lf\n" , (*primeiroItem)->extrato.codigoProduto, (*primeiroItem)->extrato.valorProduto, (*primeiroItem)->extrato.qtdProduto, (*primeiroItem)->extrato.ptsProduto, (*primeiroItem)->extrato.valorTotal);
		(*novaVenda).valorTotal +=(*primeiroItem)->extrato.valorTotal;
		pontuacaoGerada += (*primeiroItem)->extrato.qtdProduto * (*primeiroItem)->extrato.ptsProduto;
		*primeiroItem = (*primeiroItem)->proxItem;
		free(limparMemoria);
	}
		fclose(pArq_Venda);
		primeiroItem = NULL;
		return pontuacaoGerada;
}

short removerItens(Item **primeiroItem, int codItem, unsigned qtdRemovida){
	/*Recebe um ponteiro marcador de primeiro item da lista de produtos selecionados
	  Recebe o codigo correspondente ao item desejado, e a quantidade a ser removida
	  Verifica se o item e a quantidade informada sao validas, e caso sim alteram a lista de itens
	  Retorna 1 caso obtenha sucesso em todas as operacoes*/

	Item *itemRemovido;
	Item *itemAnterior;

    itemRemovido = *primeiroItem;

	//Validacao do codigo - verificando existencia na lista ("carrinho")
	while(itemRemovido && (itemRemovido->extrato.codigoProduto != codItem)){
		itemAnterior = itemRemovido;
		itemRemovido = itemRemovido->proxItem;
	}
	if(!itemRemovido){
		printf("\n\nO codigo digitado nao esta associado a nenhum item do carrinho.\n\n");
		system("pause");
		return 0;
	}

	formatarNome(itemRemovido->nomeProduto, '_');

	//Caso seja necessario eliminar o item
	if(qtdRemovida == itemRemovido->extrato.qtdProduto){
		if(itemRemovido == *primeiroItem)
			*primeiroItem = (*primeiroItem)->proxItem;
		else if(!itemRemovido->proxItem)
			itemAnterior->proxItem = NULL;
		else
			itemAnterior->proxItem = itemRemovido->proxItem;

		printf("\t\t=============== ITEM REMOVIDO ===============\n");
        printf("%s %*s %*s %*s %*s","Codigo", 10, "Nome", 45, "Valor unitario", 18, "quantidade", 18, "valor total");
		printf("\n%d %*s %*.2f %*d %*.2lf", itemRemovido->extrato.codigoProduto, 10, itemRemovido->nomeProduto, 50-strlen(itemRemovido->nomeProduto),
                itemRemovido->extrato.valorProduto, 20, itemRemovido->extrato.qtdProduto, 20, itemRemovido->extrato.valorTotal);
		free(itemRemovido);
	}
	//Alteração da quantidade do item
	else if(qtdRemovida < itemRemovido->extrato.qtdProduto){
		itemRemovido->extrato.qtdProduto -= qtdRemovida;
		itemRemovido->extrato.valorTotal = (itemRemovido->extrato.valorProduto)*(itemRemovido->extrato.qtdProduto);
		printf("\t\t=============== ITEM ALTERADO ===============\n");
		printf("%s %*s %*s %*s %*s","Codigo", 10, "Nome", 45, "Valor unitario", 18, "quantidade", 18, "valor total");
		printf("\n%d %*s %*.2f %*d %*.2lf", itemRemovido->extrato.codigoProduto, 10, itemRemovido->nomeProduto, 50-strlen(itemRemovido->nomeProduto),
                itemRemovido->extrato.valorProduto, 20, itemRemovido->extrato.qtdProduto, 20, itemRemovido->extrato.valorTotal);
	}
	else
        printf("\n\nA quantidade digitada eh invalida, o item nao sofreu alteracoes.\n\n");
    system("pause");
	return 1;
}

void listarItens(Item *itens){
	/*Imprime todos os itens da lista de de produtos selecionados*/
	system("cls");
	double valorTotal = 0;
	printf("\t\t=============== ITENS NO CARRINHO =============== \n\n");
	printf("%s %*s %*s %*s %*s","Codigo", 10, "Nome", 45, "Valor unitario", 18, "quantidade", 18, "valor total");
	while(itens){
		formatarNome(itens->nomeProduto, '_');
		printf("\n%d %*s %*.2f %*d %*.2lf", itens->extrato.codigoProduto, 10, itens->nomeProduto, 50-strlen(itens->nomeProduto), itens->extrato.valorProduto, 20, itens->extrato.qtdProduto, 20, itens->extrato.valorTotal);
		valorTotal += (itens->extrato.qtdProduto * itens->extrato.valorProduto);
		itens = itens->proxItem;
	}
	printf("\n\nTOTAL DO CARRINHO: %*s%.2lf.\n",73 , "R$", valorTotal);
}

short loginCliente(char codCliente[], char opcao){
	/*Identifica o comprador, verificando se seu cadastro eh valido, se necessario
	  Recebe a string onde sera gravado o codigo do cliente
	  Retorna 1 caso o cliente tenha se identificado e 0 caso o contrario*/
	short tipoCliente;

	if(opcao == '1' || opcao == '2'){
		if(opcao == '1')
			cadastrarCliente();
		do{
			printf("\t\t======================== DIGITE O CODIGO DO CLIENTE ==========================");
			printf("\n\nInforme o codigo: ");
			for(short indice = 0; indice < 11; indice++){
                		codCliente[indice] = getche();
                		if(indice == 2 || indice == 5)
               			printf(".");
                		else if(indice == 8)
               			printf("-");
            		}
            codCliente[11] = '\0';
            printf("\n");
			setbuf(stdin,NULL);

			if(!(checkCliente(codCliente))){
				printf("\n\nO codigo digitado nao corresponde a nenhum cliente cadastrado.\n[1]Inserir Novamente o codigo.      [0]Nao fazer login.");
				printf("\n\nInforme a opcao desejada: ");
				scanf(" %c", &opcao);
			}
		//Verificacao de existencia do cliente
		system("cls");
		}while(!(checkCliente(codCliente)) && opcao != '0');
			tipoCliente = 1;
	}
	if(opcao == '3' || opcao == '0'){
        strcpy(codCliente, "00000000000");
		tipoCliente = 0;
	}

	return tipoCliente;
}

float aplicarDesconto(RegistroVenda *novaVenda, char opcao){
	/*A rotina recebe uma estrutura contendo as informacoes, sobre a venda vigente
	  Eh feito calculo de desconto sobre o valor total da venda e retorna o resto da pontucao registrada do cliente
	  O cliente anonimo nao participa do sistema de pontuacao e desconto
	  Verifica tambem se eh aniversario do cliente, se sim, ganha 10.00 pontos*/

	InfoCliente clienteArq;

	//Busca dos dados do cliente no arquivo
	if(!(pArq_Cliente = fopen("Equipe4_cliente.txt", "r"))){
		printf("Erro ao abrir o arquivo - Equipe4_cliente.txt");
		exit(1);
	}
	while(strcmp(clienteArq.cpf, (*novaVenda).codigoCliente))
		fscanf(pArq_Cliente,"%s %s %d %f" , clienteArq.cpf, clienteArq.nome, &clienteArq.dataNascimento, &clienteArq.pontuacao);
	fclose(pArq_Cliente);

	formatarNome(clienteArq.nome, '_');
	//Adicao de pontos caso seja aniversario do cliente
	clienteArq.dataNascimento = dataInvertida(clienteArq.dataNascimento);
	if(((*novaVenda).data%10000) == (clienteArq.dataNascimento%10000)){
		printf("\n\nFeliz aniversario sr(a) %s.\nO sr(a) ganhou R$10.00 que ja pode ser aplicado nesta compra",clienteArq.nome);
		clienteArq.pontuacao += 10.0;
	}

	printf("\n\nA pontuacao do sr(a) %s eh:\t\t%.2f.", clienteArq.nome, clienteArq.pontuacao);
	printf("\n\nDeseja aplicar a pontuacao sobre o valor da compra atual?\n[1]Sim.      [2]Nao.");
	printf("\n\nInforme a opcao desejada: ");
	scanf(" %c", &opcao);

	if(opcao == '1'){
		if(clienteArq.pontuacao <= (*novaVenda).valorTotal){
			(*novaVenda).valorTotal -= clienteArq.pontuacao;
			(*novaVenda).descAplicado = clienteArq.pontuacao;
			clienteArq.pontuacao = 0;
        }
		else{
				clienteArq.pontuacao -= (*novaVenda).valorTotal;
				(*novaVenda).descAplicado = (*novaVenda).valorTotal;
				(*novaVenda).valorTotal = 0;
		}

		printf("\n\nO valor total da sua compra agora eh: R$ %.2lf.\n", (*novaVenda).valorTotal);
	}

	return clienteArq.pontuacao;
}



/*Relatorios do programa*/
void rel_clientesCadastrados(){
	/*Imprime todos os clientes cadastrados no arquivo de clientes*/
	InfoCliente cliente;
	short encontrou = 0;

	int dia, mes, ano;

	if(!(pArq_Cliente = fopen("Equipe4_cliente.txt", "r"))) exit(1);

	printf("==================== LISTAGEM DE CLIENTES CADASTRADOS ===================\n\n");
	printf("\nCPF ======= NOME =================================== DT NASC = PONTUACAO\n\n");
	while((fscanf(pArq_Cliente, "%s %s %d %f", cliente.cpf, cliente.nome, &cliente.dataNascimento, &cliente.pontuacao))!= EOF){
		encontrou = 1;
		retirarUnderlineStr(cliente.nome);
		espacamentoColunas(cliente.nome);

		printf("%s %s %d/%d/%d %.2f\n", cliente.cpf, cliente.nome,
		(cliente.dataNascimento / 1000000), ((cliente.dataNascimento / 10000) % 100), (cliente.dataNascimento % 10000), cliente.pontuacao);
	}

	if(encontrou == 0)
		printf("====================== SEM MOVIMENTO =======================\n");
	printf("\n========================================================================\n");
	fclose(pArq_Cliente);
}//FIM function

void rel_Clientes(){
	/*Verifica se existem clientes cadastrados, e caso sim gera a lista de clientes
	  Faz a verificacao do tipo de relatorio - por valor ou numero de visitas
	  imprime a lista de clientes ordenada e apaga a memoria alocada para a lista*/
	ListaClientes *primeiroCliente = NULL;
	ListaClientes *listaAux;
	char opcao;
	int indiceRank;

    FILE *ptrCliente = fopen("Equipe4_cliente.txt", "r");
    if(fgetc(ptrCliente) == EOF)
		printf("\n\nNao foi possivel gerar o relatorio pois nao existe nenhum cliente cadastrado\n\n");
    else{
        adicionarCliente_rel(&primeiroCliente);
        fclose(ptrCliente);
        do{
        	system("pause");
			system("cls");
            printf("Qual o tipo de relatorio o sr(a) deseja consultar - clientes que:\n       [1]Que compraram mais;       [2]Que mais visitaram;       [0]Sair\n");
            printf("Informe a opcao desejada: ");
            scanf(" %c", &opcao);
            system("cls");
            if(opcao == '1'){
                indiceRank = 1;
                relClienteOrdenado(&primeiroCliente, opcao);
                listaAux = primeiroCliente;
                printf("============= RANKING DE CLIENTES QUE GERARAM MAIOR RECEITA =============\n\n");
                while(listaAux){
                    printf("%d --- [%s]\t[%.2lf]\n", indiceRank++, listaAux->nome, listaAux->valorCompra);
                    listaAux = listaAux->proxCliente ;
                }
            }
            else if(opcao == '2'){
                indiceRank = 1;
                relClienteOrdenado(&primeiroCliente, opcao);
                listaAux = primeiroCliente;
                printf("============= RANKING DE CLIENTES QUE MAIS USARAM O SISTEMA =============\n\n");
                while(listaAux){
                    printf("%d --- [%s]\t[%u]\n", indiceRank++, listaAux->nome, listaAux->nVisitas);
                    listaAux = listaAux->proxCliente ;
                }
            }
        }while(opcao != '0');
    }

	//Livrar memoria alocada
	while(primeiroCliente){
		listaAux = primeiroCliente;
		primeiroCliente = primeiroCliente->proxCliente;
		free(listaAux);
	}
}

void adicionarCliente_rel(ListaClientes **primeiroCliente){
    /*Recebe um ponteiro marcador de primeiro cliente da lista
	  Le os dados referentes a um registro de compra, e
	  chama a rotina que encadeia esses dados na lista de cliente*/
	//Variaveis para leitura de arquivo de vendas;
    char verificador;
    long int codigoVenda;
    int data;
    long long int codigoCliente;
    float descAplicado;
    double valorTotal;
    //Variaveis para leitura de arquivo de clientes;
    long long int codigoCadastradoCliente;
    char nomeCliente[50];
    char stringInutil[99];

    FILE *ptrCliente;
    FILE *ptrVenda = fopen("Equipe4_venda.txt", "r");
	if(ptrVenda == NULL) exit(EXIT_FAILURE);
	while(fscanf(ptrVenda, "%c %d %u %lld %f %lf",&verificador, &codigoVenda, &data, &codigoCliente, &descAplicado, &valorTotal) != EOF){
       if(verificador == '~' && codigoCliente){       //Verificando se eh a linha que contem o codigo do cliente e que o cdigo nao eh a do cliente anonimo
            ptrCliente = fopen("Equipe4_cliente.txt", "r");
            if(ptrVenda == NULL) exit(EXIT_FAILURE);
            while(fscanf(ptrCliente, "%lld %s %[^\n]s", &codigoCadastradoCliente, nomeCliente, stringInutil) != EOF  && (codigoCadastradoCliente != codigoCliente));
        formatarNome(nomeCliente, '_');
        encadearClientes(primeiroCliente, nomeCliente, valorTotal);
        fclose(ptrCliente);
       }
	}
fclose(ptrVenda);

}//FIM function

void encadearClientes(ListaClientes **primeiroCliente, char nomeCliente[], double valorTotal){
	/*Recebe um ponteiro marcador de primeiro cliente da lista, o nome correspondente ao cliente informado, e o valor total de uma compra do clientes
      Aloca memoria para o cliente a ser encadeado.
	  Caso o cliente ja esteja na lista, seus dados sao apenas alterados (somados)
	  Caso o contrario o cliente e encadeado*/
    ListaClientes *novoCliente = (ListaClientes*)malloc(sizeof(ListaClientes));
    if(!novoCliente)
        exit(1);
    novoCliente->valorCompra = valorTotal;
    strcpy(novoCliente->nome, nomeCliente);
    novoCliente->nVisitas = 1;
    novoCliente->proxCliente = NULL;
	novoCliente->predCliente = NULL;

    //Encadeamento
    if(!(*primeiroCliente))
        *primeiroCliente = novoCliente;
    else{
        ListaClientes *auxCliente = *primeiroCliente;
        while(auxCliente->proxCliente  && strcmp(auxCliente->nome, novoCliente->nome))
            auxCliente = auxCliente->proxCliente ;
        if(!(strcmp(auxCliente->nome,novoCliente->nome))){
             auxCliente->nVisitas++;
             auxCliente->valorCompra += novoCliente->valorCompra;
        }
        else
            auxCliente->proxCliente = novoCliente;
			novoCliente->predCliente = auxCliente;
    }

}

void relClienteOrdenado(ListaClientes **primeiroCliente, char opcao){
	/*Recebe um ponteiro marcador de primeiro cliente da lista, e um char correspondente ao tipo de ordenacao
	  Ordena a lista de clientes conforme opcao informada pelo usuario - Ordenacao por valor ou por visitas*/
	ListaClientes *ordenador;
	if(opcao == '1'){	//Ordenacao por valor das compras
		while((*primeiroCliente)->proxCliente){
			ordenador = *primeiroCliente;
			while(ordenador){
				if(ordenador->valorCompra > (*primeiroCliente)->valorCompra){
					if(!ordenador->proxCliente)
						ordenador->predCliente->proxCliente = NULL;
					else{
						ordenador->predCliente->proxCliente = ordenador->proxCliente;
						ordenador->proxCliente->predCliente = ordenador->predCliente;
					}
					ordenador->proxCliente = *primeiroCliente;
					if((*primeiroCliente)->predCliente){
						(*primeiroCliente)->predCliente->proxCliente = ordenador;
						ordenador->predCliente = (*primeiroCliente)->predCliente;
					}
					else
						ordenador->predCliente = NULL;
					(*primeiroCliente)->predCliente = ordenador;
					*primeiroCliente = ordenador;
				}
				ordenador = ordenador->proxCliente;
			}
			(*primeiroCliente) = (*primeiroCliente)->proxCliente;
		}
	}
    else{	//Ordenacao por numero de visitas
		while((*primeiroCliente)->proxCliente){
			ordenador = *primeiroCliente;
			while(ordenador){
				if(ordenador->nVisitas > (*primeiroCliente)->nVisitas){
					if(!ordenador->proxCliente)
						ordenador->predCliente->proxCliente = NULL;
					else{
						ordenador->predCliente->proxCliente = ordenador->proxCliente;
						ordenador->proxCliente->predCliente = ordenador->predCliente;
					}
					ordenador->proxCliente = *primeiroCliente;
					if((*primeiroCliente)->predCliente){
						(*primeiroCliente)->predCliente->proxCliente = ordenador;
						ordenador->predCliente = (*primeiroCliente)->predCliente;
					}
					else
						ordenador->predCliente = NULL;
					(*primeiroCliente)->predCliente = ordenador;
					*primeiroCliente = ordenador;
				}
				ordenador = ordenador->proxCliente;
			}
			(*primeiroCliente) = (*primeiroCliente)->proxCliente;
		}
	}
	//Setando o primeiro cliente como inicio da listas
    while((*primeiroCliente)->predCliente)
		*primeiroCliente = (*primeiroCliente)->predCliente;
	return;
}


void rel_produtosPorValor(){
	/*Imprime todos os produtos cadastrados, conforme um intervalo de valor especificado pelo usuario*/
	InfoProduto produtoArq;
	int tipoConsulta = 0;
	float valorInf1, valorInf2;
	short encontrouProduto = 0;

	menu_RelProdutosPorValor();
	printf("\nInforme o tipo de consulta: ");
	scanf(" %d", &tipoConsulta);

	//tipo de consulta 4 pede um intervalo de valores
	if(tipoConsulta == 4){
		printf("\nMaior que: ");
		scanf(" %f", &valorInf1);
		printf("\nMenor que: ");
		scanf(" %f", &valorInf2);
	}else{
		printf("Informe um valor: ");
		scanf(" %f", &valorInf1);
	}

	system("cls");
	if(!(pArq_Produto = fopen("Equipe4_produto.txt", "r"))) exit(1);
	printf("================== PRODUTOS POR VALOR ==================\n\n");
	printf("\nCOD   = PRODUTO ================================= VALOR\n\n");
	while((fscanf(pArq_Produto, "%d %s %d %f %f\n", &produtoArq.codigoBarras, produtoArq.nome, &produtoArq.grupo,
		&produtoArq.pontuacao, &produtoArq.valor)) != EOF){

		//formatacao do nome do produto
		retirarUnderlineStr(produtoArq.nome);
		espacamentoColunas(produtoArq.nome);

		if((tipoConsulta == 1) && (produtoArq.valor >= valorInf1)){
			encontrouProduto = 1;
			printf("%d - %s R$%.2f\n", produtoArq.codigoBarras, produtoArq.nome, produtoArq.valor);
		}
		else if((tipoConsulta == 2) && (produtoArq.valor <= valorInf1)){
			encontrouProduto = 1;
			printf("%d - %s R$%.2f\n", produtoArq.codigoBarras, produtoArq.nome, produtoArq.valor);
		}
		else if((tipoConsulta == 3) && (produtoArq.valor == valorInf1)){
			encontrouProduto = 1;
			printf("%d - %s R$%.2f\n", produtoArq.codigoBarras, produtoArq.nome, produtoArq.valor);
		}
		else if((tipoConsulta == 4) && (produtoArq.valor >= valorInf1 && produtoArq.valor <= valorInf2)){
			encontrouProduto = 1;
			printf("%d - %s R$%.2f\n", produtoArq.codigoBarras, produtoArq.nome, produtoArq.valor);
		}
	}//FIM do laço percorrer o arquivo produtos

	fclose(pArq_Produto);
	if(encontrouProduto == 0)
		printf("   ================= SEM MOVIMENTO =================\n");
	printf("\n========================================================\n");
}//FIM function

void rel_produtosCadastrados(){
	/*Imprime todos os produtos cadastrados no arquivo de produtos*/
	InfoProduto produtoArq;
	short encontrouProduto = 0;

	system("cls");
	if(!(pArq_Produto = fopen("Equipe4_produto.txt", "r"))) exit(1);
	printf("============ LISTAGEM DE PRODUTOS CADASTRADOS ===========\n\n");
	printf("\nCOD   = PRODUTO ================================= VALOR\n\n");
	while((fscanf(pArq_Produto, "%d %s %d %f %f\n", &produtoArq.codigoBarras, produtoArq.nome, &produtoArq.grupo,
		&produtoArq.pontuacao, &produtoArq.valor)) != EOF){
		encontrouProduto = 1;
		retirarUnderlineStr(produtoArq.nome);
		espacamentoColunas(produtoArq.nome);
		printf("%d - %s R$%.2f\n", produtoArq.codigoBarras, produtoArq.nome, produtoArq.valor);
	}
	if(encontrouProduto == 0)
		printf("   ================= SEM MOVIMENTO =================\n");
	printf("\n========================================================\n");
	fclose(pArq_Produto);
}//FIM function

short rel_produtoPorGrupo(int grupoInformado){
	/*Recebe como parametro um inteiro correspondente ao grupo desejado
	  Percorre o arquivo de produtos e imprime todos os produtos referentes ao grupo informadao
	  Retorna 1 caso obtenha sucesso em todas as operacoes*/
	//verificar se o grupo existe
	if(grupoInformado < 1 || grupoInformado > 24){
		printf("GRUPO NAO EXISTENTE!\n");
		return 0; //retorna 0 se o grupoInformado nao estiver dentro do intervalo [1, 24]
	}

    InfoProduto auxProduto;
	short encontrouProduto = 0;

	grupoInformado = grupoInformado * 100;


	if(!(pArq_Produto = fopen("Equipe4_produto.txt", "r"))) exit(1);
	system("cls");
	printf("============ PRODUTOS POR SESSAO ============\n\n");
	printf("\nCOD   = PRODUTO ================================= VALOR\n\n");
	while((fscanf(pArq_Produto, "%d %s %d %f %f\n", &auxProduto.codigoBarras, auxProduto.nome, &auxProduto.grupo,
		 &auxProduto.pontuacao, &auxProduto.valor)) != EOF){
		if(grupoInformado == auxProduto.grupo){
			encontrouProduto = 1; //verificar se tem produto vinculado ao grupo que foi passado
			retirarUnderlineStr(auxProduto.nome);
			espacamentoColunas(auxProduto.nome);
			printf("%d - %s R$%.2f\n", auxProduto.codigoBarras, auxProduto.nome, auxProduto.valor);
		}
	}
	if(encontrouProduto == 0){
		printf("   ================= SEM MOVIMENTO =================\n");
		return 0;
	}//retorna 0 se n achar nenhum produto vinculado aquele grupo.. retorno eh usado em algumas funcoes
	printf("\n========================================================\n");
	fclose(pArq_Produto);
	return 1;

}//FIM function

void rel_produtosMaisVendidos(){
	/*Imprime os produtos que se encontram na lista gerada de produtos mais vendidos
	  Faz a linkedicao do codigo e nome do produto correspondente, e libera a memoria alocada pela lista*/
	ListaProdutos *primeiroProduto = NULL;
	int indiceRank = 0;

	if(!gerarLista_Produtos(&primeiroProduto)){
		printf("\n\nNAO EXISTEM REGISTROS DE VENDAS\n\n");
		return;
	}

	//linkedição dos nomes correspondentes ao produtos da lista, impressão e liberação de memória
	ListaProdutos *limparMemoria;
	FILE *ptrArqProduto;
	InfoProduto prodCadastrado;
	printf("================ LISTA DE PRODUTOS VENDIDOS POR UNIDADE ================\n\n");
	printf("POS = COD ==== NOME ============================== QNTVENDIDA ==========\n\n");

	while(primeiroProduto){
		limparMemoria = primeiroProduto;
		ptrArqProduto = fopen("Equipe4_produto.txt", "r");
        if(ptrArqProduto == NULL) exit(EXIT_FAILURE);
        while(fscanf(ptrArqProduto, "%d %s %d %f %f ", &prodCadastrado.codigoBarras, prodCadastrado.nome,
            &prodCadastrado.grupo, &prodCadastrado.pontuacao, &prodCadastrado.valor) != EOF  && (prodCadastrado.codigoBarras != primeiroProduto->codProduto));
        ///linkedicao dos nomes
        if(prodCadastrado.codigoBarras == primeiroProduto->codProduto)
            strcpy(primeiroProduto->nome, prodCadastrado.nome);
        fclose(ptrArqProduto);

		//impressao da lista
        retirarUnderlineStr(primeiroProduto->nome);
        espacamentoColunas(primeiroProduto->nome);
        printf("%d-> %d %s [%u]\n",++indiceRank, primeiroProduto->codProduto, primeiroProduto->nome, primeiroProduto->qtdProdVendidos);
		primeiroProduto = primeiroProduto->proxProduto;
		//Liberacao do produto
		free(limparMemoria);
	}
	printf("\n========================================================================\n");
}

short gerarLista_Produtos(ListaProdutos **primeiroProduto){
	/*Recebe como parametro um marcador para o primeiro produto da lista de produtos vendidos
	  Percorre o arquivo de vendas, encadeando todo produto registrado
	  Apos encadear todos os produtos registrados a funcao ordena este produto baseado na quantidade vendida
	  Retorna 1 caso toda a rotina tenha obtido sucesso*/
	ExtratoVenda prodVendido;
	short haVendas = 0;

	if(!(pArq_Venda = fopen("Equipe4_venda.txt", "r"))){
		printf("Erro ao abrir o arquivo - Equipe4_venda.txt");
		exit(EXIT_FAILURE);
	}


	while(fscanf(pArq_Venda, "%c %d %f %u %f %lf",&prodVendido.verificador, &prodVendido.codigoProduto, &prodVendido.valorProduto, &prodVendido.qtdProduto, &prodVendido.ptsProduto, &prodVendido.valorTotal) != EOF){
       if(prodVendido.verificador == '*'){       //Verificando se a linha lida contem os dados referentes ao produto
        haVendas = 1;
        encadearProdutos(primeiroProduto, prodVendido);
       }
	}

	//Verificação da existencia de vendas cadastradas
		if(!haVendas)
			return 0;

	//ordenação crescente da lista por produtos mais comprados
	ListaProdutos *ordenador;
	while((*primeiroProduto)->proxProduto){
			ordenador = *primeiroProduto;
			while(ordenador){
				if(ordenador->qtdProdVendidos > (*primeiroProduto)->qtdProdVendidos){
					if(!ordenador->proxProduto)
						ordenador->predProduto->proxProduto = NULL;
					else{
						ordenador->predProduto->proxProduto = ordenador->proxProduto;
						ordenador->proxProduto->predProduto = ordenador->predProduto;
					}
					ordenador->proxProduto = *primeiroProduto;
					if((*primeiroProduto)->predProduto){
						(*primeiroProduto)->predProduto->proxProduto = ordenador;
						ordenador->predProduto = (*primeiroProduto)->predProduto;
					}
					else
						ordenador->predProduto = NULL;
					(*primeiroProduto)->predProduto = ordenador;
					*primeiroProduto = ordenador;
				}
				ordenador = ordenador->proxProduto;
			}
			(*primeiroProduto) = (*primeiroProduto)->proxProduto;
		}

	//Setando o ponteiro do primeiroProduto no inicio da lista
	while((*primeiroProduto)->predProduto)
		*primeiroProduto = (*primeiroProduto)->predProduto;

	return 1;
	}

void encadearProdutos(ListaProdutos **primeiroProduto, ExtratoVenda prodVendido){
	/*Recebe como parametros o marcador de primeiro produto da lista de produtos, e uma estrutura contendo as informacoes sobre um produto vendido
	  aloca e verifica se o produto recebido ja esta na lista.
	  caso esteja soma os campos referentes ao produto duplicado e livra a memoria alocada
	  caso nao esteja na lista, encadeia o novo produto no final da lista*/
    ListaProdutos *novoProduto = (ListaProdutos*)malloc(sizeof(ListaProdutos));
    if(!novoProduto)
        exit(1);
    novoProduto->codProduto = prodVendido.codigoProduto;
    novoProduto->qtdProdVendidos = prodVendido.qtdProduto;
    novoProduto->proxProduto = NULL;
	novoProduto->predProduto = NULL;

    //Encadeamento
    if(!(*primeiroProduto))
        *primeiroProduto = novoProduto;
    else{
        ListaProdutos *auxProduto = *primeiroProduto;
        while(auxProduto->proxProduto  && auxProduto->codProduto != novoProduto->codProduto)
            auxProduto = auxProduto->proxProduto ;
        if(auxProduto->codProduto == novoProduto->codProduto){
             auxProduto->qtdProdVendidos += novoProduto->qtdProdVendidos;
             free(novoProduto);
         }
        else
            auxProduto->proxProduto = novoProduto;
			novoProduto->predProduto = auxProduto;
    }
}


void faturamentoPeriodo(){
	/*Elenca todas os registros de venda dentro de um determinado intervalo de tempo*/

	unsigned dataInicial = 0;
	unsigned dataFinal = 0;
	double faturamento = 0;
	RegistroVenda vendaArq;

	FILE *pArq_Temp;

	system("cls");
	printf("============== FATURAMENTO POR PERIODO =============\n\n");
	printf("Digite a data inicial que voce quer o faturamento: ");
    for(short indice = 0; indice < 8; indice++){
        dataInicial *= 10;
        dataInicial += (getche() - 48);
        if(indice == 1 || indice == 3)
            printf("/");
	}
	printf("\nDigite a data final que voce quer o faturamento: ");
	for(short indice = 0; indice < 8; indice++){
        dataFinal *= 10;
        dataFinal += (getche() - 48);
        if(indice == 1 || indice == 3)
            printf("/");
	}
	dataInicial = dataInvertida(dataInicial);
	dataFinal = dataInvertida(dataFinal);


	if (!(pArq_Venda = fopen("Equipe4_venda.txt", "r"))) exit(EXIT_FAILURE);

	if (!(pArq_Temp = fopen("Equipe4_faturamento.txt", "w"))) exit(EXIT_FAILURE);

	system("cls");
	printf("[DATA INICIAL: %u] ===== [DATA FINAL: %u]\n\n", dataInicial, dataFinal);
	while(fscanf(pArq_Venda,"%c %ld %u %s %f %lf", &vendaArq.verificador, &vendaArq.codigoVenda, &vendaArq.data, vendaArq.codigoCliente, &vendaArq.descAplicado, &vendaArq.valorTotal) != EOF){
			setbuf(stdin, NULL);
		  	if ((vendaArq.verificador == '~') && (vendaArq.data >= dataInicial && vendaArq.data <= dataFinal)){
				fprintf(pArq_Temp,"%.2lf %u \n", vendaArq.valorTotal, vendaArq.data);
				faturamento += vendaArq.valorTotal;
				printf("Em [%u/%u/%u] o faturamento foi [%.2lf]\n\n", ((vendaArq.data%10000)%100), ((vendaArq.data%10000)/100), (vendaArq.data/10000), vendaArq.valorTotal);
			}

	}

	if (!faturamento){
		printf("\n\nNao houve vendas no periodo informado\n\n");
	}else{
		printf("\nFaturamento total do periodo [%.2lf]\n", faturamento);
   	 	fprintf(pArq_Temp, "%s%.2lf\n", "TOTAL: R$", faturamento);
	}
    system("pause");
	fclose(pArq_Venda);
    fclose(pArq_Temp);
}//FIM function




//Funcoes auxiliares
void normalizarString(char strNormal[]){
	/*Recebe uma string digitada pelo usuario - isto eh com espacamentos
	  Remove os espacamentos da string, substituindo-os por underlines*/
	short tamStr = strlen(strNormal);
	for(size_t iNome = 0; iNome < tamStr; iNome++){
		if(strNormal[iNome] >= 97 && strNormal[iNome] <= 122)
			strNormal[iNome] = strNormal[iNome] - 32;

		if(strNormal[iNome] == 32)
			strNormal[iNome] =  '_';
	}
}//FIM function

void retirarUnderlineStr(char strUnderline[]){
	/*Recebe uma string padronizada conforme os arquivos de texto - isto eh com underlines inves de espacos em branco
	  Remove as underlines da string informada*/
	unsigned short tamStr = strlen(strUnderline);
	for(size_t iStr = 0; iStr < tamStr; iStr++)
		if(strUnderline[iStr] == '_')
			strUnderline[iStr] = ' ';
}//FIM function

void espacamentoColunas(char strBase[]){
	/*Recebe uma string base contendo de 0 a 49 caracteres
	  Completa a string com espacos em branco para formatacao de prints*/
	short tamStr = strlen(strBase);
	short difTab = DIF_TABULACAO - tamStr;
	char strEspaco[difTab + 1];
	for(size_t iStr = 0; iStr < difTab; iStr++)
		strEspaco[iStr] = ' ';
    strEspaco[difTab] = '\0';
	strcat(strBase, strEspaco);
}

short formatarNome(char nome[], char delimitador){
    /*Recebe uma string e um delimitador como paramatro
	  remove o delimitador da string formatando-a com espaco ou underline
	  poe o conteudo da string em caixa alta.
      o nome sao dividos em partes com a funcao strtok
      as partes do nome sao concatenados em uma nova string onde e inserido o espaco onde ha underline, e vice-versa
      por fim a string concatenada e copiada para a string original, e os caracteres sao formatados para caixa alta.
	  retorna 1 caso obtenha sucesso nas operacoes.
	*/
    char *nomePart;
    char nomeFormatado[50] = {'\0'};

    //Se o delimitador da string for o caracter '_', ele eh substituido por ' '
    if(delimitador == '_'){
        nomePart = strtok (nome,"_");
        while (nomePart){
            strcat(nomeFormatado, nomePart);
            nomePart = strtok (NULL, "_");
            if(nomePart)
                strcat(nomeFormatado, " ");
        }
    }
    //Caso o contrario eh o caracter ' ' que eh substituido por '_'
    else{
        nomePart = strtok (nome," ");
        while (nomePart){
            strcat(nomeFormatado, nomePart);
            nomePart = strtok (NULL, " ");
            if(nomePart)
                strcat(nomeFormatado, "_");
        }
    }

    strcpy(nome, nomeFormatado);
    //Formatacao do nome para caixa alta
    strupr(nome);
    return 1;
}

unsigned int dataInvertida(unsigned int data){
	/*Recebe um numero inteiro correspondente a uma data  - isto eh possue 8 digitos na forma ddmmaaaa
	  Faz manipulacoes aritmeticas para formatar a data segundo a notacao utilizada no sistema (aaaammdd)
	  Retorna a data formatada*/
	unsigned int dia, ano, mes, dataInvertida;

	dia = data / 1000000;
	ano = data % 10000;
	mes = (data / 10000) % 100;
	dataInvertida = (ano * 10000) + (mes * 100) + dia;

	return dataInvertida;
}

//fim do programa
