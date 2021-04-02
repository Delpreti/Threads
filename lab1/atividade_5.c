#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

#define NTHREADS  2
#define V_SIZE    10000

typedef struct {
    int begin;  // inclusive
    int end;    // not inclusive
} interval;

typedef struct {
    int* vec;
    interval range;
} thread_args;

void increment(int* vector, interval intervalo){
    for(int it = intervalo.begin; it < intervalo.end; it++){
        vector[it] += 1;
    }
}

void* thread_function(void* args_pointer){
    thread_args* args = (thread_args*) args_pointer;

    increment(args->vec, args->range);

    free(args_pointer);
    pthread_exit(NULL);
}

pthread_t spawn_thread(int index, int* vector){
    
    // Initialize aguments for the thread function
    thread_args* args = malloc( sizeof(thread_args) );
    if (args == NULL) {
        printf("malloc() not successfull\n");
        exit(-1);
    }
    args->vec = vector; 
    args->range.begin = index * V_SIZE / NTHREADS;
    args->range.end = (index + 1) * V_SIZE / NTHREADS;

    // Create the thread and return it's id
    pthread_t thread_id;
    if( pthread_create(&thread_id, NULL, thread_function, (void*) args) ){
        printf("pthread_create() not successfull\n");
        exit(-1);
    }
    return thread_id;
}

int main(){

    // --- Initialization ---
    int vetor[V_SIZE] = {0};
    int thread;

    // --- Thread creation ---
    pthread_t tid_sistema[NTHREADS];
    for(thread = 0; thread < NTHREADS; thread++){
        tid_sistema[thread] = spawn_thread(thread, vetor);
    }
    printf("%d threads created\n", thread);

    // --- Thread waiting ---
    for(thread = 0; thread < NTHREADS; thread++){
        if( pthread_join(tid_sistema[thread], NULL) ){
            printf("pthread_join() not successfull\n");
            exit(-1); 
        } 
    }

    // --- Test section ---
    int vetor_teste[V_SIZE] = {0};
    int iter;
    int flag = 0;
    for(iter = 0; iter < V_SIZE; iter++){
        if(vetor[iter] != vetor_teste[iter] + 1){
            printf("Error: Vector not incremented successfully\n");
            flag = -1;
            break;
        }
    }
    if(flag == 0){
        printf("Vector incremented correctly\n");
    }

    return 0;
}
