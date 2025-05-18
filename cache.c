//-----------------------------------------------------------------------------
// Módulo cache: Implementa uma cache genérica
//-----------------------------------------------------------------------------
// Bibliotecas

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "cache.h"

//-----------------------------------------------------------------------------
// Aloca e inicializa cache, de acordo com configuração fornecida

void alocaCache(tCache *cache, int nBlocos, int associatividade, int nPalavrasBloco)
{
	// Seta parâmetros da configuração da cache
	(*cache).nBlocos         = nBlocos;
	(*cache).associatividade = associatividade;
	(*cache).nPalavrasBloco  = nPalavrasBloco;
	(*cache).nConjuntos      = nBlocos / associatividade;

	(*cache).conjunto = calloc((*cache).nConjuntos, sizeof(tConjunto));

	if ((*cache).conjunto == NULL)
	{
		printf("Erro na alocacao de estrutura de dados\n");
		exit(0);
	}

	for (int i = 0; i < (*cache).nConjuntos; i ++)
	{
		(*cache).conjunto[i].via = calloc(associatividade, sizeof(tPosicao));

		if ((*cache).conjunto[i].via == NULL)
		{
			printf("Erro na alocacao de estrututra de dados\n");
			exit(0);
		}

		for (int j = 0; j < associatividade; j ++)
		{
			(*cache).conjunto[i].via[j].bitValidade = 0;
			(*cache).conjunto[i].via[j].tag         = 0;
			(*cache).conjunto[i].via[j].ordemAcesso = 0;
		}
	}
}

//-----------------------------------------------------------------------------
// Busca dado na cache e, se não encontra, insere-o
// Se necessário faz substituição de blocos
// Valor de retorno: 0 (acerto), 1 (falha SEM substituição) ou 2 (falha COM substituição)

int buscaInsereCache(tCache *cache, int endereco)
{
	int enderecoPalavra,
		 enderecoBloco,
		 indice,
		 tag,
		 iAcerto,	// Posição que causou acerto
		 iLivre,		// Primeira posição livre encontrada no conjunto
		 iLRU,		// Posição com menor valor de ordemAcesso no conjunto (bloco LRU)
		 ordemMin,	// Dentre as posições ocupadas no conjunto, menor valor de ordemAcesso
		 ordemMax,	// Dentre as posições ocupadas no conjunto, maior valor de ordemAcesso
		 result;

	// Interpreta bits do endereço de memória acessado
	enderecoPalavra = endereco / N_BYTES_PALAVRA;
	enderecoBloco   = enderecoPalavra / (*cache).nPalavrasBloco;
	indice          = enderecoBloco   % (*cache).nConjuntos;
	tag             = enderecoBloco   / (*cache).nConjuntos;

	// Procura dado na cache
	iAcerto  = -1;
	iLivre   = -1;
	iLRU     = -1;
	ordemMin = INT_MAX;
	ordemMax = -1;

	for (int i = 0; i < (*cache).associatividade; i ++)
	{
		if ((*cache).conjunto[indice].via[i].bitValidade == 1) // Posição possui dado válido
		{
			if ((*cache).conjunto[indice].via[i].tag == tag) // Acerto
				iAcerto = i;

			if (ordemMax < (*cache).conjunto[indice].via[i].ordemAcesso)
				ordemMax = (*cache).conjunto[indice].via[i].ordemAcesso;
	
			if (ordemMin > (*cache).conjunto[indice].via[i].ordemAcesso)
			{
				iLRU     = i;
				ordemMin = (*cache).conjunto[indice].via[i].ordemAcesso;
			}
		}
		else // Posição não possui dado válido
		{
			if (iLivre == -1)
				iLivre = i;
		}
	}

	if (iAcerto != -1) // Acerto
	{
		(*cache).conjunto[indice].via[iAcerto].ordemAcesso = ordemMax + 1;
		result = 0;
	}
	else if (iLivre != -1) // Falha SEM substitucao
	{
		(*cache).conjunto[indice].via[iLivre].bitValidade = 1;
		(*cache).conjunto[indice].via[iLivre].tag         = tag;
		(*cache).conjunto[indice].via[iLivre].ordemAcesso = ordemMax + 1;
		result = 1;
	}
	else // Falha COM substitucao
	{
		(*cache).conjunto[indice].via[iLRU].tag         = tag;
		(*cache).conjunto[indice].via[iLRU].ordemAcesso = ordemMax + 1;
		result = 2;
	}

	return result;
}

//-----------------------------------------------------------------------------
void liberaCache(tCache *cache)
{
	for (int i = 0; i < (*cache).nConjuntos; i ++)
		free((*cache).conjunto[i].via);

	free((*cache).conjunto);
}
//-----------------------------------------------------------------------------
