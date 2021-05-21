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

// macro para somar elementos de um array
#define sum_array(arr, size, soma_var){ \
    for(int i = 0; i < size; i++){ \
        soma_var += arr[i]; \
    } \
}

// --------- Threads ----------
// ----------------------------

typedef struct {
    int *block_test;
    Rw *rw_man;
    int thread_index;
    int total_threads;
} thread_args;

void* reader_function(void* args_pointer){ // ATUADOR
    thread_args* args = (thread_args*) args_pointer;

    rw_get_read(args->rw_man, args->block_test);
    sleep(3);
    rw_release_read(args->rw_man);

    free(args_pointer);
    pthread_exit(NULL);
}

void* writer_function(void* args_pointer){ // SENSOR
    thread_args* args = (thread_args*) args_pointer;

    rw_get_write(args->rw_man, args->block_test);
    sleep(3);
    rw_release_write(args->rw_man);

    free(args_pointer);
    pthread_exit(NULL);
}

#define spawn_thread_read(thread_id, index, num_threads, rwm, bt){ \
    thread_args* args = (thread_args*) malloc( sizeof(thread_args) ); \
    check_alloc(args, "Erro de alocacao\n"); \
    \
    args->block_test = bt; \
    args->rw_man = rwm; \
    args->thread_index = index; \
    args->total_threads = num_threads; \
    \
    if( pthread_create(thread_id + index, NULL, reader_function, (void*) args) ){ \
        printf("pthread_create() not successfull\n"); \
        exit(-1); \
    } \
}

#define spawn_thread_write(thread_id, index, num_threads, rwm, bt){ \
    thread_args* args = (thread_args*) malloc( sizeof(thread_args) ); \
    check_alloc(args, "Erro de alocacao\n"); \
    \
    args->block_test = bt; \
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

    // Teste 01: Multiplas threads tentando realizar leitura.
    // Requisito: Threads leitoras não devem se bloquear entre si
    printf("Iniciando teste #%d\n", test_num);
    int read_array[N_THREADS];
    for(thread = 0; thread < N_THREADS; thread++){
        spawn_thread_read(tid_sistema, thread, N_THREADS, &rw_manager, &read_array[thread]);
    }
    printf("%d thread(s) created\n", thread);

    for(thread = 0; thread < N_THREADS; thread++){
        if( pthread_join(tid_sistema[thread], NULL) ){
            printf("erro ao encerrar a thread no teste #%d\n", test_num);
        }
    }

    int test = 0;
    sum_array(read_array, N_THREADS, test);
    if(test > 0){
        printf("Teste %d: falhou\n", test_num);
    } else {
        printf("Teste %d: sucesso\n", test_num);
    }
    test_num++;

    // Teste 02: Multiplas threads tentando escrever.
    // Requisito: apenas 1 thread pode escrever por vez.
    printf("Iniciando teste #%d\n", test_num);
    int test_array[N_THREADS];
    for(thread = 0; thread < N_THREADS; thread++){
        spawn_thread_write(tid_sistema, thread, N_THREADS, &rw_manager, &test_array[thread]);
    }
    printf("%d thread(s) created\n", thread);

    for(thread = 0; thread < N_THREADS; thread++){
        if( pthread_join(tid_sistema[thread], NULL) ){
            printf("erro ao encerrar a thread no teste #%d\n", test_num);
        }
    }

    int test_sum = 0;
    sum_array(test_array, N_THREADS, test_sum);
    if(test_sum < 7){
        printf("Teste %d: falhou\n", test_num);
    } else {
        printf("Teste %d: sucesso\n", test_num);
    }
    test_num++;
    
    // Teste 03: Escitora, depois leitora
    // Requisito: A leitora deve bloquear se houver uma thread escrevendo
    printf("Iniciando teste #%d\n", test_num);
    int useful = 42;
    spawn_thread_write(tid_sistema, 0, N_THREADS, &rw_manager, &useful);
    while(useful){}
    spawn_thread_read(tid_sistema, 1, N_THREADS, &rw_manager, &useful);
    if( pthread_join(tid_sistema[0], NULL) ){
        printf("erro ao encerrar a thread no teste #%d\n", test_num);
    }
    if( pthread_join(tid_sistema[1], NULL) ){
        printf("erro ao encerrar a thread no teste #%d\n", test_num);
    }

    if(useful){
        printf("Teste %d: sucesso\n", test_num);
    } else {
        printf("Teste %d: falhou\n", test_num);
    }
    test_num++;

    // Teste 04: Prioridade na escrita
    // Requisito: A escritora deve entrar antes da segunda leitora
    printf("Iniciando teste #%d\n", test_num);
    int use1 = 42;
    spawn_thread_read(tid_sistema, 0, N_THREADS, &rw_manager, &use1);
    while(use1){} // garantir que a thread pegou a permissao para fazer a leitura

    int use2 = 42;
    spawn_thread_write(tid_sistema, 1, N_THREADS, &rw_manager, &use2);
    while(use2 != 1){} // garantir que essa thread bloqueou

    int use3 = 42;
    spawn_thread_read(tid_sistema, 2, N_THREADS, &rw_manager, &use3);

    if( pthread_join(tid_sistema[0], NULL) ){
        printf("erro ao encerrar a thread no teste #%d\n", test_num);
    }
    if( pthread_join(tid_sistema[1], NULL) ){
        printf("erro ao encerrar a thread no teste #%d\n", test_num);
    }
    if( pthread_join(tid_sistema[2], NULL) ){
        printf("erro ao encerrar a thread no teste #%d\n", test_num);
    }

    if(use3){
        printf("Teste %d: sucesso\n", test_num);
    } else {
        printf("Teste %d: falhou\n", test_num);
    }
    test_num++;

    // ----- Encerramento -----
    // ------------------------
    rw_destroy(&rw_manager);
    return 0;
}
