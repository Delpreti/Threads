/*
Atividade de laboratorio #5 de Computacao Concorrente
--- remover comentario nas linhas 148 e 180 caso deseje visualizar o array
    (e usar um tamanho pequeno para nao floodar o terminal)
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

// ----------- utils ----------
// ----------------------------

// macro para verificar resultado da alocacao de memoria
#define check_alloc(pointer, message){ \
    if (pointer == NULL) { \
        printf(message); \
        exit(-1); \
    } \
}

// macro para calcular soma_prefixo de forma sequencial
#define prefix_sum(array, size){ \
    for(int i = 1; i < size; i++){ \
        array[i] = array[i] + array[i - 1]; \
    } \
}

// macro para preencher um array de maneira aleatoria
#define fill_array_rand(array, size){ \
    for(int i = 0; i < size; i++){ \
        array[i] = rand() % 10; \
    } \
}

// macro para copiar conteudo entre arrays de mesmo tamanho
#define copy_array(arrayA, arrayB, size){ \
    for(int i = 0; i < size; i++){ \
        arrayB[i] = arrayA[i]; \
    } \
}

// macro para exibir conteudo do array(de inteiros) na tela
#define print_int_array(array, size){ \
    int i; \
    for(i = 0; i < size - 1; i++){ \
        printf("%d, ", array[i]); \
    } \
    printf("%d\n", array[i]); \
}

// --------- Threads ----------
// ----------------------------

int bloqueadas = 0;
pthread_mutex_t x_mutex;
pthread_cond_t x_cond;

//funcao barreira
void barreira(int nthreads) {
    pthread_mutex_lock(&x_mutex); //inicio da secao critica
    if (bloqueadas == (nthreads - 1)) {
        //ultima thread a chegar na barreira
        pthread_cond_broadcast(&x_cond);
        bloqueadas = 0;
    } else {
        bloqueadas++;
        pthread_cond_wait(&x_cond, &x_mutex);
    }
    pthread_mutex_unlock(&x_mutex); //fim da secao critica
}

typedef struct {
    int thread_index;
    int total_threads;
    int* vector;
    int vector_size;
} thread_args;

void* thread_function(void* args_pointer){
    thread_args* args = (thread_args*) args_pointer;

    int aux;
    for(int i = 1; i < args->vector_size; i *= 2){
        if(args->thread_index >= i){
            aux = args->vector[args->thread_index - i];
        } else {
            aux = 0;
        }
        barreira(args->total_threads);
        args->vector[args->thread_index] += aux;
        barreira(args->total_threads);
    }

    free(args_pointer);
    pthread_exit(NULL);
}

#define spawn_thread(thread_id, index, num_threads, vetor, tam_vetor){ \
    thread_args* args = (thread_args*) malloc( sizeof(thread_args) ); \
    check_alloc(args, "Erro de alocacao\n"); \
    \
    args->thread_index = index; \
    args->total_threads = num_threads; \
    args->vector = vetor; \
    args->vector_size = tam_vetor; \
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
    if(argc < 2){
        printf("Digite: %s <tamanho do array>\n", argv[0]);
        return 1;
    }

    const int ARR_SIZE = strtol( argv[1], NULL, 10 );
    if(ARR_SIZE < 1){
        printf("O <tamanho do array> deve ser maior ou igual a 1.\n");
        return 1;
    }
    const int N_THREADS = ARR_SIZE;

    // ------- Inicializacao -------
    // -----------------------------

    int thread;
    pthread_t tid_sistema[N_THREADS];

    srand(time(NULL));

    int* my_array = (int*) malloc(ARR_SIZE * sizeof(int));
    check_alloc(my_array, "erro na inicializacao");
    int* my_array_copy = (int*) malloc(ARR_SIZE * sizeof(int));
    check_alloc(my_array_copy, "erro na inicializacao");
    fill_array_rand(my_array, ARR_SIZE);
    copy_array(my_array, my_array_copy, ARR_SIZE);

    // print_int_array(my_array, ARR_SIZE);

    // ---- Criacao das threads ----
    // -----------------------------
    for(thread = 0; thread < N_THREADS; thread++){
        spawn_thread(tid_sistema, thread, N_THREADS, my_array, ARR_SIZE);
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

    // ---------- Testes ----------
    // ----------------------------
    prefix_sum(my_array_copy, ARR_SIZE);
    int i;
    for(i = 0; i < ARR_SIZE; i++){
        if(my_array[i] != my_array_copy[i]){
            printf("Erro no algoritmo!\n");
            break;
        }
    }
    if(i == ARR_SIZE){
        printf("Algoritmo executado corretamente.\n");
    }

    // print_int_array(my_array, ARR_SIZE);

    free(my_array);
    free(my_array_copy);
    return 0;
}
