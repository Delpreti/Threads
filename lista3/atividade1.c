#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#define NTHREADS  4

/* macro para verificar resultado da alocacao de memoria */
#define check_alloc(pointer, message){ \
    if (pointer == NULL) { \
        printf(message); \
        exit(-1); \
    } \
}

/* Variaveis globais */
sem_t *semaforo;

/* Thread A */
void *A (void *t) {

  printf("Seja bem-vindo!\n");
  sem_post(&semaforo[0]);
  sem_post(&semaforo[0]);

  pthread_exit(NULL);
}

/* Thread B */
void *B (void *t) {

  sem_wait(&semaforo[0]);
  printf("Sente-se por favor.\n");
  sem_post(&semaforo[1]);

  pthread_exit(NULL);
}

/* Thread C */
void *C (void *t) {

  sem_wait(&semaforo[0]);
  printf("Fique a vontade.\n");
  sem_post(&semaforo[2]);

  pthread_exit(NULL);
}

/* Thread D */
void *D (void *t) {

  sem_wait(&semaforo[1]);
  sem_wait(&semaforo[2]);
  printf("Volte sempre!\n");

  pthread_exit(NULL);
}

/* Funcao principal */
int main(int argc, char *argv[]) {
  int i; 
  pthread_t threads[NTHREADS];

  /* Inicilaiza o semaforo*/
  semaforo = (sem_t*) malloc(NTHREADS * sizeof(sem_t));
  check_alloc(semaforo, "erro de alocacao do semaforo");
  for (i = 0; i < NTHREADS; i++) {
    sem_init(&semaforo[i], 0, 0);
  }

  /* Cria as threads */
  pthread_create(&threads[0], NULL, A, NULL);
  pthread_create(&threads[1], NULL, B, NULL);
  pthread_create(&threads[2], NULL, C, NULL);
  pthread_create(&threads[3], NULL, D, NULL);

  /* Espera todas as threads completarem */
  for (i = 0; i < NTHREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  free(semaforo);
  return 0;
}
