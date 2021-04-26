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
        h[u] = rand() % UINT8_MAX;
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
pthread_mutex_t flag_mutex;

void* thread_function(void* args_pointer){
    thread_args* args = (thread_args*) args_pointer;

    uint8_t *copy_header = malloc(HEADER_SIZE * sizeof(uint8_t));
    check_alloc(copy_header, "falha na copia do header\n");
    for(int u = 0; u < 76; u++){
        copy_header[u] = args->block_header[u];
    }

    uint32_t *nonce = (uint32_t *) (copy_header + 76);
    *nonce = args->thread_index;

    char ret_buffer[65];
	int found = 0;
    while(*(args->leave_flag)){
        sha256(copy_header, 80, ret_buffer);
		found = 1;
        for(int i = 0; i < args->dificuldade; i++){
            if(ret_buffer[i] != 'b'){
				found = 0;
				break;
            }
        }
        if (found) {
            pthread_mutex_lock(&flag_mutex);
            *(args->leave_flag) = 0;
            pthread_mutex_unlock(&flag_mutex);

            uint32_t *golden_nonce = malloc(sizeof(uint32_t));
            check_alloc(golden_nonce, "Erro de alocacao\n");
            *golden_nonce = *nonce;

            free(args_pointer);
            free(copy_header);
            pthread_exit(golden_nonce);
        }
        *nonce += args->total_threads;
    }

	free(args_pointer);
	free(copy_header);
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

	char *endptr;
    const int difficulty = strtol(argv[1], &endptr, 0); // quantidade de zeros necessarios
	if (*argv[1] == '\0' || *endptr != '\0' || difficulty <= 0) {
        printf("A <dificuldade> deve ser um número maior que 0.\n");
        return 1;
    }

    const int N_THREADS = strtol(argv[2], &endptr, 0);
	if (*argv[2] == '\0' || *endptr != '\0' || N_THREADS <= 0) {
        printf("O <numero de threads> deve ser um número maior que 0.\n");
        return 1;
    }

    // inicializacao
    srand(time(NULL));
    uint8_t *head = gen_header();
	pthread_mutex_init(&flag_mutex, NULL);
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
		uint32_t *ret;
        if( pthread_join(tid_sistema[thread], (void **) &ret) ){
            printf("pthread_join() not successfull\n");
            exit(-1);
        }

		if (ret)
			printf("Nonce: %u\n", *ret);
		free(ret);
    }
    free(head);
	pthread_mutex_destroy(&flag_mutex);

    GET_TIME(end);
    printf("Tempo: %.5lf segundos\n", end - start);
    return 0;
}
