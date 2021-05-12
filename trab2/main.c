#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include "rw.h"

#define BUFFER_SIZE 60

// ----------- utils ----------
// ----------------------------

// macro para verificar resultado da alocacao de memoria
#define check_alloc(pointer, message){ \
    if (pointer == NULL) { \
        printf(message); \
        exit(-1); \
    } \
}

float get_temperature_rand(){
    float x = rand() % 150;
    return (x / 10.0) + 25;
}

int current;
int state;

void change_state(int s){
    state = s;
    if(s == 0){
        printf("Condicao Normal\n");
    } else if(s == 1){
        printf("Alerta amarelo\n");
    } else {
        printf("Alerta vermelho\n");
    }
}

// 0 para ok, 1 para amarelo, 2 para vermelho
int check_temperature(float *buff){
    int highs = 0;
    for(int i = current; i > current - 15; i--){
        if(buff[i % 60] > 35.0){
            highs++;
        }
        if(highs == 5){
            if(i == 5){
                return 2;
            } else {
                return 1;
            }
        }
    }
    return 0;
}

void write_buffer(float *buff, float temperature){
    buff[current] = temperature;
    current = current + 1 % BUFFER_SIZE;
}

// --------- Threads ----------
// ----------------------------

typedef struct {
    float *buffer;
    Rw *rw_man;
    int thread_index;
    int total_threads;
} thread_args;

void reader_function(void* args_pointer){ // ATUADOR
    thread_args* args = (thread_args*) args_pointer;

    // loop principal
    int result;
    while(1){
        rw_get_read(args->rw_man);
        result = check_temperature(args->buffer);
        rw_release_read(args->rw_man);
        if(result != state){
            change_state(result);
        }
        sleep(2);
    }

    free(args_pointer);
    pthread_exit(NULL);

}

void writer_function(void* args_pointer){ // SENSOR
    thread_args* args = (thread_args*) args_pointer;

    // loop principal
    while(1){
        rw_get_write(args->rw_man);
        write_buffer(args->buffer, get_temperature_rand());
        rw_release_write(args->rw_man);
        sleep(1);
    }

    free(args_pointer);
    pthread_exit(NULL);
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
    const int NTHREADS = strtol(argv[1], &endptr, 0); // quantidade de zeros necessarios

    // ---- Inicialização ----
    // -----------------------
    srand(time(NULL));
    Rw rw_manager;
    rw_init(&rw_manager);

    float buffer[BUFFER_SIZE];
    current = 0;
    state = 0;

    printf("%.1f\n", get_temperature_rand());

    // ----- Encerramento -----
    // ------------------------
    rw_destroy(&rw_manager);
    return 0;
}
