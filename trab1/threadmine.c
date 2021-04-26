#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#include <stdint.h>
#include "../timer.h"
#include "sha256.h"

// ----------- utils ----------
// ----------------------------

// macro para verificar resultado da alocacao de memoria
#define check_alloc(pointer, message){ \
    if (pointer == NULL) { \
        printf(message); \
        exit(-1); \
    } \
}

#define HEADER_SIZE 80

uint8_t *gen_header(){
    uint8_t *h = calloc(HEADER_SIZE, sizeof(uint8_t));
    check_alloc(h, "falha na geracao do header\n");
    for(int u = 0; u < 76; u++){
        h[u] = rand();
    }
    return h;
}

// --------- Threads ----------
// ----------------------------

typedef struct {
    uint8_t *block_header;
    int dificuldade;
    int *leave_flag;
    int thread_index;
    int total_threads;
} thread_args;

int flag;

void* thread_function(void* args_pointer){
    thread_args* args = (thread_args*) args_pointer;

    uint32_t nonce = args->thread_index;
    char ret_buffer[256];
    while(*(args->leave_flag)){
        args->block_header[76] = nonce;
        sha256(args->block_header, 80, ret_buffer);
        int j;
        for(j = 0; j < args->dificuldade; j++){
            if(ret_buffer[j] != 'b'){
                break;
            }
        }
        if(j == args->dificuldade){
            *(args->leave_flag) = 0;
        }
        if(nonce == UINT32_MAX){
            printf("nonce nao encontrada\n");
            *(args->leave_flag) = 0;
        }
        nonce += args->total_threads;
    }

    free(args_pointer);
    pthread_exit(NULL); // passar a nonce aqui ?
}

#define spawn_thread(thread_id, index, num_threads, diff, bhead){ \
    thread_args* args = (thread_args*) malloc( sizeof(thread_args) ); \
    check_alloc(args, "Erro de alocacao\n"); \
    \
    args->block_header = bhead; \
    args->leave_flag = &flag; \
    args->dificuldade = diff; \
    args->thread_index = index; \
    args->total_threads = num_threads; \
    \
    if( pthread_create(thread_id + index, NULL, thread_function, (void*) args) ){ \
        printf("pthread_create() not successfull\n"); \
        exit(-1); \
    } \
}

// ----------------------------

int main(int argc, char** argv){

    // ---- Leitura da entrada ----
    // ----------------------------
    if(argc < 3){
        printf("Digite: %s <dificuldade> <numero de threads>\n", argv[0]);
        return 1;
    }

    const int difficulty = strtol( argv[1], NULL, 10 ); // quantidade de zeros necessarios
    if(difficulty < 0){
        printf("A <dificuldade> deve ser maior ou igual a 0.\n");
        return 1;
    }
    const int N_THREADS = strtol( argv[2], NULL, 10 );

    // inicializacao
    srand(time(NULL));
    uint8_t *head = gen_header();
    flag = 1;
    double start, end;

    // ----------------------------

    GET_TIME(start);

    int thread;
    pthread_t tid_sistema[N_THREADS];

    // ---- Criacao das threads ----
    // -----------------------------
    for(thread = 0; thread < N_THREADS; thread++){
        spawn_thread(tid_sistema, thread, N_THREADS, difficulty, head);
    }
    printf("%d thread(s) created.\n", thread);

    // ---- Aguardo das threads ----
    // -----------------------------
    for(thread = 0; thread < N_THREADS; thread++){
        if( pthread_join(tid_sistema[thread], NULL) ){
            printf("pthread_join() not successfull\n");
            exit(-1);
        }
    }

    GET_TIME(end);
    printf("Tempo: %.5lf segundos\n", end - start);
    return 0;
}
