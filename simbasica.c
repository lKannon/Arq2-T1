//-----------------------------------------------------------------------------
// Simulador de cache básica: Um único nível de cache, unificada para instruções e dados (cache L1)
//
// Para compilar: gcc cache.c simbasica.c -o simbasica -Wall
// Para executar: simbasica arquivoConfiguracao arquivoAcessos
//-----------------------------------------------------------------------------
// Bibliotecas

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cache.h"	// Módulo que implementa cache genérica

//-----------------------------------------------------------------------------
// Variaveis

						// Medidas de desempenho
int nAcessosI,		// Número total de acessos a instruções
	 nAcessosD,		// Número total de acessos a dados
	 nFalhasL1;		// Número de falhas na cache L1

tCache cacheL1;	// Cache L1

FILE *arqAcessos;	// Arquivo de trace com acessos a memória

//-----------------------------------------------------------------------------
// Prototipos de funcoes

void inicializa(int, char **);
void finaliza(void);

//-----------------------------------------------------------------------------
// Programa principal

int main(int argc, char **argv)
{
	char tipoAcesso;	// 'I' (leitura de instrução), 'L' (LOAD = leitura de dado) ou 'S' (STORE = escrita de dado)
	int result,
		 resultAcesso,
		 endereco;		// Endereço de memória acessado

	inicializa(argc, argv);

	// Enquanto não termina arquivo de acessos
	while (! feof(arqAcessos))
	{
		// Lê um endereço do arquivo de acessos
		result = fscanf(arqAcessos, "%c %d\n", &tipoAcesso, &endereco);

		if ((result != 0) && (result != EOF))
		{
			if (tipoAcesso == 'I')	// Acesso é leitura de uma instrução
				nAcessosI++;
			else	// Acesso é leitura de dado ('L') ou escrita de dado ('S')
				nAcessosD++;

			// Procura bloco na cache L1 e, se não encontra, insere bloco na cache L1
			// Valor de retorno = 0 (acerto), 1 (falha sem substituição) ou 2 (falha com substituição)
			resultAcesso = buscaInsereCache(&cacheL1, endereco);

			if ((resultAcesso == 1) || (resultAcesso == 2))	// Se houve falha na cache L1
				nFalhasL1 ++;
		}
	}

	finaliza();

	return 0;
}
//-----------------------------------------------------------------------------
void inicializa(int argc, char **argv)
{
	char nomeArqConfig[100],
		  nomeArqAcessos[100];

	unsigned int nBlocosL1,				// Número total de blocos da cache L1
					 associatividadeL1,	// Número de blocos por conjunto da cache L1
					 nPalavrasBlocoL1;	// Número de palavras do bloco da cache L1

	FILE *arqConfig;	// Arquivo com configuração da cache

	if (argc != 3)
	{
		printf("\nUso: simbasica arquivoConfiguracao arquivoAcessos\n");
		exit(0);
	}

	// Lê arquivo de configuracao e obtem nBlocos, associatividade e nPalavrasBloco da cache L1
	strcpy(nomeArqConfig, argv[1]);
	arqConfig = fopen(nomeArqConfig, "rt");
	if (arqConfig == NULL)
	{
		printf("\nArquivo de configuração não encontrado\n");
		exit(0);
	}
	fscanf(arqConfig, "%d %d %d", &nBlocosL1, &associatividadeL1, &nPalavrasBlocoL1);
	fclose(arqConfig);

	// Abre arquivo de acessos
	strcpy(nomeArqAcessos, argv[2]);
   arqAcessos = fopen(nomeArqAcessos, "rt");
	if (arqAcessos == NULL)
	{
		printf("\nArquivo de acessos não encontrado\n");
		exit(0);
	}

	// Inicializa medidas de desempenho
	nAcessosI = 0;
	nAcessosD = 0;
	nFalhasL1 = 0;

	// Aloca e inicializa estrutura de dados da cache L1
	alocaCache(&cacheL1, nBlocosL1, associatividadeL1, nPalavrasBlocoL1);
}

//-----------------------------------------------------------------------------
void finaliza(void)
{
	// Fecha arquivo de acessos
	fclose(arqAcessos);

	// Imprime medidas de desempenho
	printf("nAcessosI: %d\n", nAcessosI);
	printf("nAcessosD: %d\n", nAcessosD);
	printf("nFalhasL1: %d\n", nFalhasL1);

	// Libera estrutura de dados da cache
	liberaCache(&cacheL1);
}

//-----------------------------------------------------------------------------
