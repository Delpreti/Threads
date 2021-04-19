#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "../timer.h"

// ----------- utils ----------
// ----------------------------

// macro para verificar resultado da malloc
#define check_malloc(pointer, message){ \
    if (pointer == NULL) { \
        printf(message); \
        exit(-1); \
    } \
}

// --------- Threads ----------
// ----------------------------

typedef struct {
    int dificuldade;
    int thread_index;
    int total_threads;
} thread_args;

void* thread_function(void* args_pointer){
    thread_args* args = (thread_args*) args_pointer;

    free(args_pointer);
    pthread_exit(NULL);
}

#define spawn_thread(thread_id, index, num_threads, diff){ \
    thread_args* args = (thread_args*) malloc( sizeof(thread_args) ); \
    check_malloc(args, "Erro de alocacao\n"); \
    \
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

    double start, end;

    // ---- Leitura da entrada ----
    // ----------------------------
    if(argc < 3){
        printf("Digite: %s <dificuldade> <numero de threads>\n", argv[0]);
        return 1;
    }

    const long long difficulty = strtol( argv[1], NULL, 10 ); // quantidade de zeros necessarios
    if(difficulty < 0){
        printf("A <dificuldade> deve ser maior ou igual a 0.\n");
        return 1;
    }
    const int N_THREADS = strtol( argv[2], NULL, 10 );

    // ----------------------------

    GET_TIME(start);

    int thread;
    pthread_t tid_sistema[N_THREADS];

    // ---- Criacao das threads ----
    // -----------------------------
    for(thread = 0; thread < N_THREADS; thread++){
        spawn_thread(tid_sistema, thread, N_THREADS, difficulty);
    }
    // printf("%d thread(s) created.\n", thread);

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