#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "../timer.h"

// ----------- utils ----------
// ----------------------------

// macro para preencher uma matriz N por N de maneira aleatoria
#define fill_matrix_rand(mat, nquad){ \
    for(int i = 0; i < nquad; i++){ \
        mat[i] = rand() % 10; \
    } \
}

// macro para preencher uma matriz N por N com o valor dado
#define fill_matrix_const(mat, nquad, value){ \
    for(int i = 0; i < nquad; i++){ \
        mat[i] = value; \
    } \
}

// macro para exibir a matriz
#define print_matrix(mat, n){ \
    for(int i = 0; i < n; i++){ \
        for(int j = 0; j < n; j++){ \
            printf("%d ", mat[i * n + j]); \
        } \
        printf("\n"); \
    } \
}

// macro com multiplicacao de matrizes tradicional
#define mult_matrix(m1, m2, result, n){ \
    int aux = 0; \
    int i, j, k; \
    for(i = 0; i < n; i++){ \
        for(j = 0; j < n; j++){ \
            for(k = 0; k < n; k++){ \
                aux += m1[n * j + k] * m2[n * k + i]; \
            } \
            result[n * j + i] = aux; \
            aux = 0; \
        } \
    } \
}

// macro para verificar resultado da malloc
#define check_malloc(pointer){ \
    if (pointer == NULL) { \
        printf("malloc() not successfull\n"); \
        exit(-1); \
    } \
}

// --------- Threads ----------
// ----------------------------

typedef struct {
    int* m_a;
    int* m_b;
    int* m_out;
    int thread_index;
    int total_threads;
    int size;
} thread_args;

void* thread_function_1(void* args_pointer){
    thread_args* args = (thread_args*) args_pointer;

    // O indice da thread mapeia nos elementos da saida
    int SQ_SIZE = args->size * args->size;
    for(int k = args->thread_index; k < SQ_SIZE; k += args->total_threads){
        //int a_line = k / args->size;
        int b_col = k % args->size;

        for(int i = 0; i < args->size; i++){
            //args->m_out[a_line * args->size + b_col] += args->m_a[a_line * args->size + i] * args->m_b[i * args->size + b_col];
            args->m_out[k] += args->m_a[k - b_col + i] * args->m_b[i * args->size + b_col];
        }
    }

    free(args_pointer);
    pthread_exit(NULL);
}

void* thread_function_2(void* args_pointer){
    thread_args* args = (thread_args*) args_pointer;

    // O indice da thread mapeia nas linhas
    for(int a_line = args->thread_index; a_line < args->size; a_line += args->total_threads){
        for(int b_col = 0; b_col < args->size; b_col++){
            for(int i = 0; i < args->size; i++){
                args->m_out[a_line * args->size + b_col] += args->m_a[a_line * args->size + i] * args->m_b[i * args->size + b_col];
            }
        }
    }

    free(args_pointer);
    pthread_exit(NULL);
}

#define spawn_thread(thread_id, index, num_threads, ma, mb, mout, msize, f_type){ \
    thread_args* args = (thread_args*) malloc( sizeof(thread_args) ); \
    check_malloc(args); \
    \
    args->m_a = ma; \
    args->m_b = mb; \
    args->m_out = mout; \
    args->thread_index = index; \
    args->total_threads = num_threads; \
    args->size = msize; \
    \
    if(f_type == 1){ \
        if( pthread_create(thread_id + index, NULL, thread_function_1, (void*) args) ){ \
            printf("pthread_create() not successfull\n"); \
            exit(-1); \
        } \
    } else { \
        if( pthread_create(thread_id + index, NULL, thread_function_2, (void*) args) ){ \
            printf("pthread_create() not successfull\n"); \
            exit(-1); \
        } \
    } \
}

// ----------------------------

int main(int argc, char** argv){

    double start, end;
    double t_init, t_mult, t_end;

    // - Parte 1
    GET_TIME(start);
    // ---- Leitura da entrada ----
    // ----------------------------
    if(argc < 4){
        printf("Digite: %s <dimensao da matriz> <numero de threads> <abordagem(1 ou 2)>\n", argv[0]);
        return 1;
    }

    const int M_SIZE = strtol( argv[1], NULL, 10 );
    const int SQUARE_SIZE = M_SIZE * M_SIZE;
    int N_THREADS_T = strtol( argv[2], NULL, 10 );
    const int PART_TYPE = strtol( argv[3], NULL, 10 );
    // Ajuste sugerido no programa feito em aula
    if(PART_TYPE == 1 && N_THREADS_T > SQUARE_SIZE){
        N_THREADS_T = SQUARE_SIZE;
        printf("Foram solicitadas mais threads que o necessario, a quantidade foi ajustada.\n");
    }
    if(PART_TYPE == 2 && N_THREADS_T > M_SIZE){
        N_THREADS_T = M_SIZE;
        printf("Foram solicitadas mais threads que o necessario, a quantidade foi ajustada.\n");
    }
    const int N_THREADS = N_THREADS_T;

    // ------ Inicializacao -------
    // ----------------------------
    srand(time(NULL));

    int* matriz_a = (int*) malloc( SQUARE_SIZE * sizeof(int) );
    check_malloc(matriz_a);
    int* matriz_b = (int*) malloc( SQUARE_SIZE * sizeof(int) );
    check_malloc(matriz_b);
    int* matriz_saida = (int*) malloc( SQUARE_SIZE * sizeof(int) );
    check_malloc(matriz_saida);

    // CASO A:
    //fill_matrix_const(matriz_a, SQUARE_SIZE, 1);
    //fill_matrix_const(matriz_b, SQUARE_SIZE, 1);
    // CASO B:
    fill_matrix_rand(matriz_a, SQUARE_SIZE);
    fill_matrix_rand(matriz_b, SQUARE_SIZE);

    fill_matrix_const(matriz_saida, SQUARE_SIZE, 0);

    int thread;

    GET_TIME(end);
    t_init = end - start;

    // - Parte 2
    GET_TIME(start);
    // --- Criacao das threads ----
    // ----------------------------
    pthread_t tid_sistema[N_THREADS];
    for(thread = 0; thread < N_THREADS; thread++){
        //tid_sistema[thread] = spawn_thread(thread, N_THREADS, matriz_a, matriz_b, matriz_saida, M_SIZE);
        spawn_thread(tid_sistema, thread, N_THREADS, matriz_a, matriz_b, matriz_saida, M_SIZE, PART_TYPE);
    }
    printf("%d thread(s) created.\n", thread);

    // --- Aguardo das threads ----
    // ----------------------------
    for(thread = 0; thread < N_THREADS; thread++){
        if( pthread_join(tid_sistema[thread], NULL) ){
            printf("pthread_join() not successfull\n");
            exit(-1);
        } 
    }

    GET_TIME(end);
    t_mult = end - start;

    // - Parte 3
    GET_TIME(start);
    /* Remover esse comentario para utilizar os casos de teste, caso desejar.
    // --- Verificacao do resultado ---
    // --------------------------------

    int flag = 0;

    // CASO A:
    for(int iter = 0; iter < SQUARE_SIZE; iter++){
        if(matriz_saida[iter] != M_SIZE){
            printf("Error: Matrix multiplication failed\n");
            flag = -1;
            break;
        }
    }

    // CASOS A E B: (teste lento)
    int* matriz_tradicional = (int*) malloc( SQUARE_SIZE * sizeof(int) );
    check_malloc(matriz_tradicional);
    mult_matrix(matriz_a, matriz_b, matriz_tradicional, M_SIZE);
    for(int iter = 0; iter < SQUARE_SIZE; iter++){
        if(matriz_saida[iter] != matriz_tradicional[iter]){
            printf("Error: Matrix multiplication failed\n");
            flag = -1;
            break;
        }
    }

    if(flag == 0){
        printf("Matrix multiplication was successfull\n");
    }
    // print_matrix(matriz_saida, M_SIZE);

    // ---- Limpeza / Finalizacao ----
    // -------------------------------
    free(matriz_tradicional);
    */
    free(matriz_a);
    free(matriz_b);
    free(matriz_saida);
    

    GET_TIME(end);
    t_end = end - start;

    // ---------- Resultados ---------
    // -------------------------------
    printf("Tempo inicializacao: %.5lf segundos\n", t_init);
    printf("Tempo multiplicacao: %.5lf segundos\n", t_mult);
    printf("Tempo finalizacao: %.5lf segundos\n", t_end);
    printf("Tempo total: %.5lf segundos\n", t_init + t_mult + t_end);

    return 0;
}
