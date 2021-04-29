/*
Atividade de laboratorio #5 de Computacao Concorrente
--  Essa implementacao permite que o usuario especifique uma quantidade de threads menor que #elementos / 2.
    desde que a quantidade de threads seja um divisor desse valor.
--  O programa calcula todos os indices onde a soma sera realizada antes de criar as threads.
    Esses indices ficam salvos em duas matrizes, indices_get e indices_set;
    cada linha da matriz deve esperar que toda a linha anterior de somas tenha executado;
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
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

// ---- Matriz de indices ----
// ---------------------------

// Funcao de doer o cerebro
// Calcula os indices onde sera feita a soma e salva em duas matrizes
void init_matrix(int* ma, int* mb, int lines, int cols){
    int current_line = 0;
    int u, v, w, aux1, aux2;
    for(u = 0; u < cols; u++){
        ma[u] = 2 * u;
        mb[u] = ma[u] + 1;
    }
    current_line = 1;
    for(w = 0; w < (pow(2, lines - 1) - 1); w++){
        aux1 = pow(2, current_line - 1);
        aux2 = pow(2, current_line);
        for(v = 0; v < aux2; v++){
            ma[u + v] = mb[u - cols + aux1 - 1];
            mb[u + v] = aux2 + v + (2 * (u % cols));
        }
        u += v;
        current_line = u / cols;
    }
}

void print_mat(int* ma, int* mb, int l, int c){
    int elements = l * c;
    for(int i = 0; i < elements; i++){
        printf("%d%d ", ma[i], mb[i]);
        if(i % c == c - 1){
            printf("\n");
        }
    }
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
    int* index_get;
    int* index_set;
} thread_args;

void* thread_function(void* args_pointer){
    thread_args* args = (thread_args*) args_pointer;

    int limit = log2(args->vector_size) * args->vector_size / 2;
    int i;
    for(i = args->thread_index; i < limit; i += args->total_threads){
        args->vector[args->index_set[i]] += args->vector[args->index_get[i]];
        barreira(args->total_threads);
    }

    free(args_pointer);
    pthread_exit(NULL);
}

#define spawn_thread(thread_id, index, num_threads, vetor, tam_vetor, index_g, index_s){ \
    thread_args* args = (thread_args*) malloc( sizeof(thread_args) ); \
    check_alloc(args, "Erro de alocacao\n"); \
    \
    args->thread_index = index; \
    args->total_threads = num_threads; \
    args->vector = vetor; \
    args->vector_size = tam_vetor; \
    args->index_get = index_g; \
    args->index_set = index_s; \
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
        printf("Digite: %s <tamanho do array> <numero de threads (opcional)>\n", argv[0]);
        return 1;
    }

    const int ARR_SIZE = strtol( argv[1], NULL, 10 );
    if(ARR_SIZE < 1){
        printf("O <tamanho do array> deve ser maior ou igual a 1.\n");
        return 1;
    }
    int N_THREADS_T;
    if(argc > 2){
        N_THREADS_T = strtol( argv[2], NULL, 10 );
        if(ARR_SIZE % N_THREADS_T != 0){
            N_THREADS_T = ARR_SIZE / 2;
            printf("O numero de threads fornecido deve ser divisor do tamanho do array, a quantidade foi ajustada.\n");
        }
    } else {
        N_THREADS_T = ARR_SIZE / 2;
    }
    if(N_THREADS_T > ARR_SIZE / 2){
        N_THREADS_T = ARR_SIZE / 2;
        printf("Foram solicitadas mais threads que o necessario, a quantidade foi ajustada.\n");
    }
    const int N_THREADS = N_THREADS_T;

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

    const int LINES = log2(ARR_SIZE);
    const int COLS = ARR_SIZE / 2;

    int* indices_get = (int*) malloc(LINES * COLS * sizeof(int));
    check_alloc(indices_get, "erro na alocacao de matriz\n");
    int* indices_set = (int*) malloc(LINES * COLS * sizeof(int));
    check_alloc(indices_set, "erro na alocacao de matriz\n");

    init_matrix(indices_get, indices_set, LINES, COLS);

    //print_mat(indices_get, indices_set, ARR_SIZE, colunas);
    print_int_array(my_array, ARR_SIZE);

    // ---- Criacao das threads ----
    // -----------------------------
    for(thread = 0; thread < N_THREADS; thread++){
        spawn_thread(tid_sistema, thread, N_THREADS, my_array, ARR_SIZE, indices_get, indices_set);
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

    print_int_array(my_array, ARR_SIZE);

    free(my_array);
    free(my_array_copy);
    free(indices_get);
    free(indices_set);
    return 0;
}
