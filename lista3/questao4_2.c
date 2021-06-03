#include <semaphore.h>
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

// --------------------------------------------------------------------------------------------------------

#define N 50
#define C 1
#define P 1

int buf[N];
int next_index_p = 0;
int next_index_c[C] = {0};

/* Cada semáforo do array começa com 0 */
sem_t sem[C];
/* Começa com 1 */
sem_t mutex;
/* Começa com N */
sem_t empty_slot;

void produz(int num) {
	sem_wait(&empty_slot);

	sem_wait(&mutex);
	buf[next_index_p] = num;
	next_index_p = (next_index_p + 1) % N;
	sem_post(&mutex);

	for (int i = 0; i < C; i++)
		sem_post(&sem[i]);
}

int consome(int thread_id) {
	sem_wait(&sem[thread_id]);

	sem_wait(&mutex);
	int ret = buf[next_index_c[thread_id]];
	next_index_c[thread_id] = (next_index_c[thread_id] + 1) % N;

	int buf_length;
	sem_getvalue(&empty_slot, &buf_length);
	buf_length = N - buf_length;

	int all_consumed = 1;
	for (int i = 0; i < C; i++) {
		int to_consume;
		sem_getvalue(&sem[i], &to_consume);

		if (to_consume == buf_length) {
			all_consumed = 0;
			break;
		}
	}

	if (all_consumed)
		sem_post(&empty_slot);

	sem_post(&mutex);

	return ret;
}

// --------------------------------------------------------------------------------------------------------

pthread_mutex_t mutex_content;

void *cons(void *args) {
	int *tid = (int*) args;
    while(1) { 
        //pthread_mutex_lock(&mutex_content);
        printf("vou consumir\n");
        consome(*tid);
        //pthread_mutex_unlock(&mutex_content); 
    }
    pthread_exit(NULL);
}

void *prod(void *args) {
    while(1) {
        //pthread_mutex_lock(&mutex_content);
        printf("vou produzir\n");
        produz(1 + (rand() % 9));
        //pthread_mutex_unlock(&mutex_content);
    }
    pthread_exit(NULL);
}

/* Funcao principal */
int main(int argc, char *argv[]) {

	/*
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
    */

    // ------- Inicializacao -------
    // -----------------------------
    int i;
    srand(time(NULL));

    pthread_t produtoras[P];
    pthread_t consumidoras[C];

    pthread_mutex_init(&mutex_content, NULL);

    for(i = 0; i < C; i++){
	    sem_init(&sem[i], 0, 0);
	}
    sem_init(&mutex, 0, 1);
    sem_init(&empty_slot, 0, N);

    /* Cria as threads*/
    for(i = 0; i < P; i++){
        pthread_create(&produtoras[i], NULL, prod, NULL);
    }
    for(i = 0; i < C; i++){
        pthread_create(&consumidoras[i], NULL, cons, (void*) &i);
    }

    /* Espera um input (unico caractere) do usuario para encerrar */
    char input;
    scanf("%c", &input);

    for(i = 0; i < P; i++){
        pthread_cancel(produtoras[i]);
    }
    for(i = 0; i < C; i++){
        pthread_cancel(consumidoras[i]);
    }

    pthread_mutex_destroy(&mutex_content);
    return 0;
}
