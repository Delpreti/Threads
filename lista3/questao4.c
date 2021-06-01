#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/* macro para verificar resultado da alocacao de memoria */
#define check_alloc(pointer, message){ \
    if (pointer == NULL) { \
        printf(message); \
        exit(-1); \
    } \
}

/* Implementacao Produtor-Consumidor */

int *content; // Buffer
int size; // Tamanho do buffer
int content_index; // indice para insercao
int **consumido; // a thread marca o proprio indice no item ao consumir

pthread_mutex_t mutex_content;

void produz_item(){
    content[content_index] = 1 + (rand() % 9);
    content_index++;
}

int is_full(int* arr){
    for(int j = 0; j < size; j++){
        if(arr[j] != j){
            return 0;
        }
    }
    return 1;
}

void clean(int* arr){
    for(int j = 0; j < size; j++){
        arr[j] = 0;
    }
}

void check_complete(int index){
    if( is_full(consumido[index]) ){
        clean(consumido[index]);
        content[index] = 0;
    }
}

void consome_item(int index, pthread_t tid){
    consumido[index][tid] = tid;
    check_complete(index);
}

// Funcoes das threads

void *cons(void *args) {
    while(1) { 
        pthread_mutex_lock(&mutex_content);
        consome_item(item);
        pthread_mutex_unlock(&mutex_content); 
        check_complete(index);
    }
    pthread_exit(NULL);
}

void *prod(void *args) {
    while(1) {
        pthread_mutex_lock(&mutex_content);
        produz_item();
        pthread_mutex_unlock(&mutex_content);
    }
    pthread_exit(NULL);
}

/* Funcao principal */
int main(int argc, char *argv[]) {

    // ---- Leitura da entrada ----
    // ----------------------------
    if(argc < 4){
        printf("Digite: %s <tamanho do buffer> <numero de threads produtoras> <numero de threads consumidoras>\n", argv[0]);
        return 1;
    }

    const int N = strtol( argv[1], NULL, 10 ); // Tamanho do Buffer em bytes
    const int P = strtol( argv[2], NULL, 10 ); // Numero de threads produtoras
    const int C = strtol( argv[3], NULL, 10 ); // Numero de threads consumidoras
    if(N < 1 || P < 1 || C < 1){
        printf("Erro: Parametro menor que 1.\n");
        return 1;
    }

    // ------- Inicializacao -------
    // -----------------------------
    int i;
    srand(time(NULL));

    size = N;

    content = (int*) malloc( N * sizeof(int) );
    check_alloc(content, "erro de alocacao do buffer");
    content_index = 0

    consumido = (int**) malloc( N * sizeof(int*) );
    check_alloc(consumido, "erro de alocacao do consumido");
    for(i = 0; i < N; i++){
        consumido[i] = (int*) calloc( C * sizeof(int) );
        check_alloc(consumido[i], "erro de alocacao do consumido*");
    }

    pthread_t produtoras[P];
    pthread_t consumidoras[C];

    pthread_mutex_init(&mutex_content, NULL);

    /* Cria as threads*/
    for(i = 0; i < P; i++){
        pthread_create(&produtoras[i], NULL, prod, NULL);
    }
    for(i = 0; i < C; i++){
        pthread_create(&consumidoras[i], NULL, cons, NULL);
    }

    /* Espera um input do usuario para encerrar */
    char* input;
    scanf()

    for(i = 0; i < P; i++){
        pthread_cancel(produtoras[i], NULL);
    }
    for(i = 0; i < C; i++){
        pthread_cancel(consumidoras[i], NULL);
    }

    free(content);
    for(i = 0; i < N; i++){
        free(consumido[i]);
    }
    free(consumido);
    pthread_mutex_destroy(&mutex_content);
    return 0;
}
