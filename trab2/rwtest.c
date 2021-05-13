#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "rw.h"

// ----------- utils ----------
// ----------------------------

// macro para verificar resultado da alocacao de memoria
#define check_alloc(pointer, message){ \
    if (pointer == NULL) { \
        printf(message); \
        exit(-1); \
    } \
}

// --------- Threads ----------
// ----------------------------

typedef struct {
    Rw *rw_man;
    int thread_index;
    int total_threads;
} thread_args;

void* reader_function(void* args_pointer){ // ATUADOR
    thread_args* args = (thread_args*) args_pointer;

    rw_get_read(args->rw_man);
    sleep(10);
    rw_release_read(args->rw_man);

    free(args_pointer);
    pthread_exit(NULL);
}

void* writer_function(void* args_pointer){ // SENSOR
    thread_args* args = (thread_args*) args_pointer;

    rw_get_write(args->rw_man);
    sleep(10);
    rw_release_write(args->rw_man);

    free(args_pointer);
    pthread_exit(NULL);
}

#define spawn_thread_read(thread_id, index, num_threads, rwm){ \
    thread_args* args = (thread_args*) malloc( sizeof(thread_args) ); \
    check_alloc(args, "Erro de alocacao\n"); \
    \
    args->rw_man = rwm; \
    args->thread_index = index; \
    args->total_threads = num_threads; \
    \
    if( pthread_create(thread_id + index, NULL, reader_function, (void*) args) ){ \
        printf("pthread_create() not successfull\n"); \
        exit(-1); \
    } \
}

#define spawn_thread_write(thread_id, index, num_threads, rwm){ \
    thread_args* args = (thread_args*) malloc( sizeof(thread_args) ); \
    check_alloc(args, "Erro de alocacao\n"); \
    \
    args->rw_man = rwm; \
    args->thread_index = index; \
    args->total_threads = num_threads; \
    \
    if( pthread_create(thread_id + index, NULL, writer_function, (void*) args) ){ \
        printf("pthread_create() not successfull\n"); \
        exit(-1); \
    } \
}

// ----------------------------

int main(int argc, char** argv){

    // ---- Inicialização ----
    // -----------------------
    Rw rw_manager;
    rw_init(&rw_manager);

    const int N_THREADS = 8;

    int thread;
    pthread_t tid_sistema[N_THREADS];

    int test_num = 1;

    // Teste 01: Multiplas threads tentando escrever.
    // Requisito: apenas 1 thread pode escrever por vez.
    printf("Iniciando teste #%d\n", test_num);
    for(thread = 0; thread < N_THREADS; thread++){
        spawn_thread_write(tid_sistema, thread, N_THREADS, &rw_manager);
    }
    printf("%d thread(s) created\n", thread);

    for(thread = 0; thread < N_THREADS; thread++){
        if( pthread_kill(tid_sistema[thread], 0) != 0 ){
            printf("Thread %d nao recebeu sinal.\n", thread);
        }
    }

    for(thread = 0; thread < N_THREADS; thread++){
        if( pthread_cancel(tid_sistema[thread]) != 0 ){
            printf("erro ao derrubar a thread no teste #%d\n", test_num);
        }
    }

    test_num++;/*
    // Teste 02: Escitora, depois leitora
    // Requisito: A leitora deve bloquear se houver uma thread escrevendo
    printf("Iniciando teste #%d\n", test_num);
    spawn_thread_write(tid_sistema, 0, N_THREADS, &rw_manager);
    if( pthread_kill(0, 0) != 0 ){
        printf("Thread %d nao recebeu sinal, ok.\n", thread);
        spawn_thread_read(tid_sistema, 1, N_THREADS, &rw_manager);
        if( pthread_kill(0, 0) != 0 ){
    }

    if( pthread_cancel(tid_sistema[0]) != 0 ){
        printf("erro ao derrubar a thread no teste #%d\n", test_num);
    }*/

    // ----- Encerramento -----
    // ------------------------
    rw_destroy(&rw_manager);
    return 0;
}
