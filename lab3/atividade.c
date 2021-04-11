#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "timer.h"

// ----------- utils ----------
// ----------------------------

// macro para verificar resultado da malloc
#define check_malloc(pointer, message){ \
    if (pointer == NULL) { \
        printf(message); \
        exit(-1); \
    } \
}

double approx_pi_0(long long n){
    double result = 1.0;
    int counter = 1;
    for(double i = 3.0; counter < n; i += 2.0){
        if(counter % 2 == 0){
            result += 1.0 / i;
        } else {
            result -= 1.0 / i;
        }
        counter++;
    }
    return 4.0 * result;
}

// Esse foi o algoritmo adaptado para a versão concorrente,
// ele insere dois termos da serie a cada passagem do loop,
// e no final verifica se deveria remover o ultimo.
double approx_pi_1(long long n){
    double result = 1.0;
    int counter = 1;
    double i;
    for(i = 3.0; counter < n; i += 4.0){
        result -= 2.0 / (i * (i + 2));
        counter += 2;
    }
    if( n % 2 == 0 ){
        result -= 1 / (i - 2);
    }
    return 4.0 * result;
}

// --------- Threads ----------
// ----------------------------

typedef struct {
    int n_terms;
    int thread_index;
    int total_threads;
} thread_args;

void* thread_function(void* args_pointer){
    thread_args* args = (thread_args*) args_pointer;

    double* result = (double*) malloc( sizeof(double) );
    check_malloc(result, "Erro de alocacao\n");
    *result = 0.0;

    int counter = 1;
    double i;
    double increment = 4.0 * args->total_threads;
    int count_incr = 2 * args->total_threads;
    for(i = 3.0 + (4.0 * args->thread_index); counter < args->n_terms; i += increment){
        *result += 2.0 / (i * (i + 2));
        counter += count_incr;
    }

    free(args_pointer);
    pthread_exit((void*) result);
}

#define spawn_thread(thread_id, index, num_threads, terms){ \
    thread_args* args = (thread_args*) malloc( sizeof(thread_args) ); \
    check_malloc(args, "Erro de alocacao\n"); \
    \
    args->n_terms = terms; \
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
        printf("Digite: %s <numero de termos> <numero de threads>\n", argv[0]);
        return 1;
    }

    const long long TERMOS = strtol( argv[1], NULL, 10 );
    if(TERMOS < 1){
        printf("O <numero de termos> deve ser maior ou igual a 1.\n");
        return 1;
    }
    int N_THREADS_T = strtol( argv[2], NULL, 10 );
    if(N_THREADS_T > TERMOS){
        N_THREADS_T = TERMOS;
        printf("Foram solicitadas mais threads que o necessario, a quantidade foi ajustada.\n");
    }
    const int N_THREADS = N_THREADS_T;    

    // algoritmo sequencial 0
    GET_TIME(start);
    double not_pi_0 = approx_pi_0(TERMOS);
    GET_TIME(end);
    double t_seq_0 = end - start;

    // algoritmo sequencial 1
    GET_TIME(start);
    double not_pi_1 = approx_pi_1(TERMOS);
    GET_TIME(end);
    double t_seq_1 = end - start;

    // algoritmo concorrente
    GET_TIME(start);

    int thread;
    pthread_t tid_sistema[N_THREADS];

    // ---- Criacao das threads ----
    // -----------------------------
    for(thread = 0; thread < N_THREADS; thread++){
        spawn_thread(tid_sistema, thread, N_THREADS, TERMOS);
    }
    // printf("%d thread(s) created.\n", thread);

    // ---- Aguardo das threads ----
    // -----------------------------
    double* retorno;
    double not_pi_2 = 1;
    for(thread = 0; thread < N_THREADS; thread++){
        if( pthread_join(tid_sistema[thread], (void**) &retorno) ){
            printf("pthread_join() not successfull\n");
            exit(-1);
        }
        not_pi_2 -= *retorno;
    }
    if(TERMOS % 2 == 0 ){
        not_pi_2 -= 1 / (2 * TERMOS + 1);
    }
    not_pi_2 *= 4;

    GET_TIME(end);
    double t_conc = end - start;

    // -- Exibicao dos resultados --
    // -----------------------------
    printf("Aproximacao sequencial 0: %.15lf\n", not_pi_0);
    printf("Erro: %.15lf%%\n", 100.0 * fabs(not_pi_0 - M_PI) / M_PI);
    printf("Tempo (sequencial 0): %.5lf segundos\n\n", t_seq_0);

    printf("Aproximacao sequencial 1: %.15lf\n", not_pi_1);
    printf("Erro: %.15lf%%\n", 100.0 * fabs(not_pi_1 - M_PI) / M_PI);
    printf("Tempo (sequencial 1): %.5lf segundos\n\n", t_seq_1);

    printf("Aproximacao concorrente: %.15lf\n", not_pi_2);
    printf("Erro: %.15lf%%\n", 100.0 * fabs(not_pi_2 - M_PI) / M_PI);
    printf("Tempo (concorrente): %.5lf segundos\n", t_conc);

    return 0;
}

// printf("Valor de PI: %.15lf\n", M_PI);