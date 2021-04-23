/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 4 */
/* Codigo: Uso de variáveis de condição e suas operações básicas para sincronização por condição */

/***** Condicao logica da aplicacao: a thread B so pode imprimir "Bye" depois que duas threads A imprimirem  "Hello"  ****/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NTHREADS  4

/* Variaveis globais */
int x = 0;
pthread_mutex_t x_mutex;
pthread_cond_t x_cond;
pthread_cond_t y_cond;

/* Thread A */
void *A (void *t) {

  pthread_mutex_lock(&x_mutex);
  printf("Seja bem-vindo!\n");
  x++;
  pthread_cond_broadcast(&x_cond);
  pthread_mutex_unlock(&x_mutex);

  pthread_exit(NULL);
}

/* Thread B */
void *B (void *t) {

  pthread_mutex_lock(&x_mutex);
  if (x < 1) { 
    pthread_cond_wait(&x_cond, &x_mutex);
  }
  printf("Sente-se por favor.\n");
  x++;
  pthread_cond_signal(&y_cond);
  pthread_mutex_unlock(&x_mutex); 
  pthread_exit(NULL);
}

/* Thread C */
void *C (void *t) {

  pthread_mutex_lock(&x_mutex);
  if (x < 1) { 
    pthread_cond_wait(&x_cond, &x_mutex);
  }
  printf("Fique a vontade.\n");
  x++;
  pthread_cond_signal(&y_cond);
  pthread_mutex_unlock(&x_mutex); 
  pthread_exit(NULL);
}

/* Thread D */
void *D (void *t) {

  pthread_mutex_lock(&x_mutex);
  while (x < 3) { 
    pthread_cond_wait(&y_cond, &x_mutex);
  }
  printf("Volte sempre!\n");
  pthread_mutex_unlock(&x_mutex); 
  pthread_exit(NULL);
}

/* Funcao principal */
int main(int argc, char *argv[]) {
  int i; 
  pthread_t threads[NTHREADS];

  /* Inicilaiza o mutex (lock de exclusao mutua) e a variavel de condicao */
  pthread_mutex_init(&x_mutex, NULL);
  pthread_cond_init (&x_cond, NULL);

  /* Cria as threads */
  pthread_create(&threads[0], NULL, A, NULL);
  pthread_create(&threads[1], NULL, B, NULL);
  pthread_create(&threads[2], NULL, C, NULL);
  pthread_create(&threads[3], NULL, D, NULL);

  /* Espera todas as threads completarem */
  for (i = 0; i < NTHREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  /* Desaloca variaveis e termina */
  pthread_mutex_destroy(&x_mutex);
  pthread_cond_destroy(&x_cond);
  pthread_cond_destroy(&y_cond);
}
