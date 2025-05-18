//-----------------------------------------------------------------------------
// Módulo cache: Implementa uma cache genérica
//-----------------------------------------------------------------------------
// Constantes

#define N_BYTES_PALAVRA	4	// Número de bytes da palavra

//-----------------------------------------------------------------------------
// Tipos que definem estrutura de uma cache genérica

typedef struct	// Posição da cache
{
	int bitValidade,
		 tag,
		 ordemAcesso;	// Para implementar LRU exato
} tPosicao;

typedef struct	// Conjunto da cache
{
	tPosicao *via;
} tConjunto;

typedef struct	// Cache
{
	tConjunto *conjunto;
	int nBlocos,				// Número total de blocos da cache
		 associatividade,		// Número de blocos por conjunto
		 nPalavrasBloco,		// Número de palavras do bloco
		 nConjuntos;			// Número de conjuntos da cache
} tCache;

//-----------------------------------------------------------------------------
// Protótipos de funcoes

void alocaCache(tCache *cache, int nBlocos, int associatividade, int nPalavrasBloco);
int buscaInsereCache(tCache *cache, int endereco);
void liberaCache(tCache *cache);

//-----------------------------------------------------------------------------
