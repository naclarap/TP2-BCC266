#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CACHE_SIZE1 16
#define CACHE_SIZE2 32
#define CACHE_SIZE3 64
#define RAM_SIZE 1000
#define WORDS_PER_BLOCK 4
#define NUM_SETS1 4
#define NUM_SETS2 8
#define NUM_SETS3 16

typedef struct {
    int palavras[WORDS_PER_BLOCK];
    int atualizado;
} BlocoMemoria;

typedef struct {
    BlocoMemoria* blocos;
    int tamanho;
    int numSets;
} Cache;

typedef struct {
    int endBloco;
    int endPalavra;
} Endereco;

typedef struct {
    int opcode;
    Endereco add1;
    Endereco add2;
    Endereco add3;
} Instrucao;

typedef struct {
    BlocoMemoria* memoria;
    int tamanho;
} RAM;

int hitC1 = 0, missC1 = 0;
int hitC2 = 0, missC2 = 0;
int hitC3 = 0, missC3 = 0;
int custo = 0;

// Função para criar e inicializar uma cache
Cache* criarCache(int tamanho, int numSets) {
    Cache* cache = (Cache*)malloc(sizeof(Cache));
    cache->tamanho = tamanho;
    cache->numSets = numSets;
    cache->blocos = (BlocoMemoria*)malloc(sizeof(BlocoMemoria) * tamanho);
    for (int i = 0; i < tamanho; i++) {
        cache->blocos[i].atualizado = 0;
    }
    return cache;
}

// Função para inicializar a RAM
void inicializarRAM(RAM* ram, int tamanho) {
    ram->tamanho = tamanho;
    ram->memoria = (BlocoMemoria*)malloc(sizeof(BlocoMemoria) * tamanho);
    for (int i = 0; i < tamanho; i++) {
        for (int j = 0; j < WORDS_PER_BLOCK; j++) {
            ram->memoria[i].palavras[j] = rand() % 100;
        }
        ram->memoria[i].atualizado = 0;
    }
}

// Função de busca nas caches (simplificada)
BlocoMemoria buscarNasCaches(Endereco end, RAM* ram, Cache* cache1, Cache* cache2, Cache* cache3) {
    BlocoMemoria resultado = {0};

    // Calcular conjunto para cada cache
    int conjunto1 = end.endBloco % NUM_SETS1;
    int conjunto2 = end.endBloco % NUM_SETS2;
    int conjunto3 = end.endBloco % NUM_SETS3;

    // Simulação de busca simplificada
    // Substitua isso com a lógica de busca real
    if (cache1->blocos[conjunto1].atualizado) {
        hitC1++;
        resultado = cache1->blocos[conjunto1];
    } else {
        missC1++;
        // Simulação de movimentação de RAM para cache1
        cache1->blocos[conjunto1] = ram->memoria[end.endBloco];
        resultado = cache1->blocos[conjunto1];
    }

    // Repita para cache2 e cache3
    if (cache2->blocos[conjunto2].atualizado) {
        hitC2++;
        resultado = cache2->blocos[conjunto2];
    } else {
        missC2++;
        cache2->blocos[conjunto2] = ram->memoria[end.endBloco];
        resultado = cache2->blocos[conjunto2];
    }

    if (cache3->blocos[conjunto3].atualizado) {
        hitC3++;
        resultado = cache3->blocos[conjunto3];
    } else {
        missC3++;
        cache3->blocos[conjunto3] = ram->memoria[end.endBloco];
        resultado = cache3->blocos[conjunto3];
    }

    return resultado;
}

// Função para executar o programa
void executarPrograma(Instrucao* programa, int tamanhoPrograma, RAM* ram, Cache* cache1, Cache* cache2, Cache* cache3) {
    for (int PC = 0; PC < tamanhoPrograma; PC++) {
        Instrucao inst = programa[PC];
        
        if (inst.opcode == -1) {
            printf("Programa terminado!\n");
            break;
        }

        BlocoMemoria reg1 = buscarNasCaches(inst.add1, ram, cache1, cache2, cache3);
        BlocoMemoria reg2 = buscarNasCaches(inst.add2, ram, cache1, cache2, cache3);
        BlocoMemoria reg3 = buscarNasCaches(inst.add3, ram, cache1, cache2, cache3);

        switch (inst.opcode) {
            case 0: // soma
                reg3.palavras[inst.add3.endPalavra] = reg1.palavras[inst.add1.endPalavra] + reg2.palavras[inst.add2.endPalavra];
                reg3.atualizado = 1;
                custo += reg1.palavras[inst.add1.endPalavra] + reg2.palavras[inst.add2.endPalavra];
                printf("Inst sum -> RAM posicao %d com conteudo na cache 1 %d\n", inst.add3.endBloco, reg3.palavras[inst.add3.endPalavra]);
                break;
            case 1: // subtrai
                reg3.palavras[inst.add3.endPalavra] = reg1.palavras[inst.add1.endPalavra] - reg2.palavras[inst.add2.endPalavra];
                reg3.atualizado = 1;
                custo += reg1.palavras[inst.add1.endPalavra] - reg2.palavras[inst.add2.endPalavra];
                printf("Inst sub -> RAM posicao %d com conteudo na cache 1 %d\n", inst.add3.endBloco, reg3.palavras[inst.add3.endPalavra]);
                break;
        }

        printf("Custo ateh o momento.... %d\n", custo);
        printf("Ateh o momento ... Hit C1: %d Miss C1: %d\n", hitC1, missC1);
        printf("Ateh o momento ... Hit C2: %d Miss C2: %d\n", hitC2, missC2);
        printf("Ateh o momento ... Hit C3: %d Miss C3: %d\n", hitC3, missC3);
    }
}

// Função para gerar um programa aleatório
void gerarProgramaAleatorio(Instrucao* programa, int tamanho) {
    srand(time(NULL));
    for (int i = 0; i < tamanho - 1; i++) {
        programa[i].opcode = rand() % 2;
        programa[i].add1.endBloco = rand() % RAM_SIZE;
        programa[i].add1.endPalavra = rand() % WORDS_PER_BLOCK;
        programa[i].add2.endBloco = rand() % RAM_SIZE;
        programa[i].add2.endPalavra = rand() % WORDS_PER_BLOCK;
        programa[i].add3.endBloco = rand() % RAM_SIZE;
        programa[i].add3.endPalavra = rand() % WORDS_PER_BLOCK;
    }
    programa[tamanho - 1].opcode = -1; // Termina o programa
}

int main() {
    RAM ram;
    Cache* cache1 = criarCache(CACHE_SIZE1, NUM_SETS1);
    Cache* cache2 = criarCache(CACHE_SIZE2, NUM_SETS2);
    Cache* cache3 = criarCache(CACHE_SIZE3, NUM_SETS3);

    inicializarRAM(&ram, RAM_SIZE);

    Instrucao* programa = (Instrucao*)malloc(sizeof(Instrucao) * 10000);
    gerarProgramaAleatorio(programa, 10000);

    executarPrograma(programa, 10000, &ram, cache1, cache2, cache3);

    free(programa);
    free(cache1->blocos);
    free(cache1);
    free(cache2->blocos);
    free(cache2);
    free(cache3->blocos);
    free(cache3);
    free(ram.memoria);

    return 0;
}
