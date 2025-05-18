//-----------------------------------------------------------------------------
// Simulador de cache dois niveis: Um nível de cache, separada para instruções e dados (cache I1 e D1) e um nivel L2
//
// Para compilar: gcc cache.c simniveis.c -o simniveis -Wall
// Para executar: simniveis arquivoConfiguracao arquivoAcessos
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
	 nAcessosL2,
	 nFalhasTotal;		// Número de falhas Totais nas caches 

tCache cacheI1, cacheD1, cacheL2;	// Cache L1 e Cache D1

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
		 endereco,		// Endereço de memória acessado
		 nFalhasI1 = 0,
		 nFalhasD1 = 0,
		 nFalhasL2 = 0;

	inicializa(argc, argv);

	// Enquanto não termina arquivo de acessos
	while (! feof(arqAcessos))
	{
		// Lê um endereço do arquivo de acessos
		result = fscanf(arqAcessos, "%c %d\n", &tipoAcesso, &endereco);

		if ((result != 0) && (result != EOF))
		{
			if (tipoAcesso == 'I')	// Acesso é leitura de uma instrução
            {
				nAcessosI++;
                resultAcesso = buscaInsereCache(&cacheI1, endereco);
				if ( resultAcesso != 0)
					nFalhasI1 ++;
					nFalhasTotal ++; // Atualiza o numero de falhas global
            // Procura bloco na cache I1 e, se não encontra, insere bloco na cache I1
			// Valor de retorno = 0 (acerto), 1 (falha sem substituição) ou 2 (falha com substituição)
			// Atualiza variavel contatora de falhas na cache I1
            }
			else	// Acesso é leitura de dado ('L') ou escrita de dado ('S')
            {
				nAcessosD++;
                resultAcesso = buscaInsereCache(&cacheD1, endereco);
				if ( resultAcesso != 0)
					nFalhasD1 ++;
					nFalhasTotal ++; // Atualiza o numero de falhas global
			// Procura bloco na cache D1 e, se não encontra, insere bloco na cache D1
			// Valor de retorno = 0 (acerto), 1 (falha sem substituição) ou 2 (falha com substituição)
			// Atualiza variavel contatora de falhas na cache D1
            }
			

			if ((resultAcesso == 1) || (resultAcesso == 2))	// Se houve falha na cache I1 ou D1
			{
				nAcessosL2++;
				resultAcesso = buscaInsereCache(&cacheL2, endereco); // Chama a busca na L2 e atualiza a o resultado de acesso
				if ( resultAcesso != 0) // Se houve falha na busca
					nFalhasL2 ++; //Atualiza contador especifico
					nFalhasTotal ++; // Atualiza o numero de falhas global
			}
		}
	}

	finaliza();
	printf("Falhas I1: %d\n", nFalhasI1);
	printf("Falhas D1: %d\n", nFalhasD1);
	printf("Falhas L2: %d\n", nFalhasL2);
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
	nFalhasTotal = 0;

	// Aloca e inicializa estrutura de dados da cache L1
	alocaCache(&cacheI1, nBlocosL1, associatividadeL1, nPalavrasBlocoL1);
	alocaCache(&cacheD1, nBlocosL1, associatividadeL1, nPalavrasBlocoL1);
	alocaCache(&cacheL2, nBlocosL1, associatividadeL1, nPalavrasBlocoL1);
}

//-----------------------------------------------------------------------------
void finaliza(void)
{
	// Fecha arquivo de acessos
	fclose(arqAcessos);

	// Imprime medidas de desempenho
	printf("nAcessosI: %d\n", nAcessosI);
	printf("nAcessosD: %d\n", nAcessosD);
	printf("nFalhasTotal: %d\n", nFalhasTotal);

	// Libera estrutura de dados da cache
	liberaCache(&cacheI1);
    liberaCache(&cacheD1);
	liberaCache(&cacheL2);
}

//-----------------------------------------------------------------------------
