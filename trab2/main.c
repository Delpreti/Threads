#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include "rw.h"
#include "util.h"

#define BUFFER_SIZE 60

typedef struct {
    int id;
    int degree;
} heat;

float get_temperature_rand(){
    float x = rand() % 150;
    return (x / 10.0) + 25;
}

heat new_heat(int id){
    heat h;
    h.degree = get_temperature_rand();
    h.id = id;
    return h;
}

int current;
int grow;

void change_state(int s){
    if(s == 0){
        printf("Condicao Normal\n");
    } else if(s == 1){
        printf("Alerta amarelo\n");
    } else {
        printf("Alerta vermelho\n");
    }
}

// 0 para ok, 1 para amarelo, 2 para vermelho
int check_temperature(heat *buff, int t_id){
    int highs = 0;
    float fifteen = 0.0;
    float sum = 0.0;
    int flag = 0;
    if(grow >= 60){
        for(int i = current; i > current - 60; i--){
            if(buff[i % 60].id == t_id){
                sum += buff[i % 60].degree;
                fifteen += 1;
                if(buff[i % 60].degree > 35.0){
                    highs++;
                }
                if(highs == 5){
                    if(i == 5){
                        flag = 2;
                    } else if(fifteen <= 15){
                        flag = 1;
                    }
                }
            }
        }
    } else {
        for(int i = current; i > current - grow; i--){
            if(buff[i % 60].id == t_id){
                fifteen += 1;
                if(buff[i % 60].degree > 35.0){
                    highs++;
                }
                if(highs == 5){
                    if(i == 5){
                        flag = 2;
                    } else if(fifteen <= 15){
                        flag = 1;
                    }
                }
            }
        }
    }
    printf("Sensor %d, Temperatura Média: %.2f\n", t_id, sum / fifteen);
    return flag;
}

void write_buffer(heat *buff, int t_id){
    buff[current] = new_heat(t_id);
    current = (current + 1) % BUFFER_SIZE;
    grow++;
}

// --------- Threads ----------
// ----------------------------

typedef struct {
    heat *buffer;
    Rw *rw_man;
    int *nulo;
    int thread_index;
    int total_threads;
} thread_args;

void* reader_function(void* args_pointer){ // ATUADOR
    thread_args* args = (thread_args*) args_pointer;

    // loop principal
    int result;
    while(1){
        printf("%d quer ler\n", args->thread_index);
        rw_get_read(args->rw_man, args->nulo);
        printf("%d vai ler\n", args->thread_index);
        result = check_temperature(args->buffer, args->thread_index);
        change_state(result);
        rw_release_read(args->rw_man);
        printf("%d ja leu\n", args->thread_index);
        sleep(2);
    }

    free(args->nulo);
    free(args_pointer);
    pthread_exit(NULL);
}

void* writer_function(void* args_pointer){ // SENSOR
    thread_args* args = (thread_args*) args_pointer;

    // loop principal
    while(1){
        printf("%d quer escrever\n", args->thread_index);
        rw_get_write(args->rw_man, args->nulo);
        printf("%d vai escrever\n", args->thread_index);
        write_buffer(args->buffer, args->thread_index);
        rw_release_write(args->rw_man);
        printf("%d ja escreveu\n", args->thread_index);
        sleep(1);
    }

    free(args->nulo);
    free(args_pointer);
    pthread_exit(NULL);
}

#define spawn_thread_read(thread_id, index, num_threads, rwm, b){ \
    thread_args* args = (thread_args*) malloc( sizeof(thread_args) ); \
    check_alloc(args, "Erro de alocacao\n"); \
    \
    int *nul = (int *) malloc(sizeof(int)); \
    check_alloc(nul, "erro de alocacao"); \
    args->nulo = nul; \
    args->buffer = b; \
    args->rw_man = rwm; \
    args->thread_index = index; \
    args->total_threads = num_threads; \
    \
    if( pthread_create(thread_id + index, NULL, reader_function, (void*) args) ){ \
        printf("pthread_create() not successfull\n"); \
        exit(-1); \
    } \
}

#define spawn_thread_write(thread_id, index, num_threads, rwm, b){ \
    thread_args* args = (thread_args*) malloc( sizeof(thread_args) ); \
    check_alloc(args, "Erro de alocacao\n"); \
    \
    int *nul = (int *) malloc(sizeof(int)); \
    check_alloc(nul, "erro de alocacao"); \
    args->nulo = nul; \
    args->buffer = b; \
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

    // ---- Leitura da entrada ----
    // ----------------------------
    if(argc < 2){
        printf("Digite: %s <numero de sensores / escritores>\n", argv[0]);
        return 1;
    }
    char *endptr;
    const int N_THREADS = strtol(argv[1], &endptr, 0); // quantidade de zeros necessarios

    // ---- Inicialização ----
    // -----------------------
    srand(time(NULL));
    Rw rw_manager;
    rw_init(&rw_manager);

    heat buffer[BUFFER_SIZE];
    current = 0;
    grow = 0;

    int thread;
    pthread_t tid_write[N_THREADS];
    pthread_t tid_read[N_THREADS];

    // ---- Criacao das threads ----
    // -----------------------------
    for(thread = 0; thread < N_THREADS; thread++){
        spawn_thread_write(tid_write, thread, N_THREADS, &rw_manager, buffer);
        spawn_thread_read(tid_read, thread, N_THREADS, &rw_manager, buffer);
    }
    printf("%d thread(s) created\n", thread);

    // ---- Aguardo das threads ----
    // -----------------------------
    for(thread = 0; thread < N_THREADS; thread++){
        if( pthread_join(tid_write[thread], NULL) ){
            printf("pthread_join() not successfull\n");
            exit(-1);
        }
        if( pthread_join(tid_read[thread], NULL) ){
            printf("pthread_join() not successfull\n");
            exit(-1);
        }
    }

    //printf("%.1f\n", get_temperature_rand());

    // ----- Encerramento -----
    // ------------------------
    rw_destroy(&rw_manager);
    return 0;
}
