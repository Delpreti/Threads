#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "rw.h"
#include "util.h"

#define NTHREADS 8
#define NTESTS 5

#define spawn_thread(thread, attr, start_routine, arg) { \
	if (pthread_create(thread, attr, start_routine, arg)) { \
        printf("pthread_create() not successfull\n"); \
		exit(1); \
	} \
}

Rw rw;
pthread_t tid[NTHREADS];

void *reader_thread(void *ret) {
    rw_get_read(&rw, (int *) ret);
    sleep(3);
    rw_release_read(&rw);
    pthread_exit(NULL);
}

void *writer_thread(void *ret) {
    rw_get_write(&rw, (int *) ret);
    sleep(3);
    rw_release_write(&rw);
    pthread_exit(NULL);
}

// Teste 01: Múltiplas threads tentando realizar leitura.
// Requisito: Threads leitoras não devem se bloquear entre si
int test1(void) {
	int block_ret[NTHREADS];

	for (int i = 0; i < NTHREADS; i++)
		spawn_thread(&tid[i], NULL, reader_thread, &block_ret[i]);
	for (int i = 0; i < NTHREADS; i++)
		pthread_join(tid[i], NULL);

	int test = 0;
    sum_array(block_ret, NTHREADS, test);

	/* Nenhuma das threads deve ter sido bloqueada  */
	return test == NTHREADS * STRAIGHT;
}

// Teste 02: Múltiplas threads tentando escrever.
// Requisito: Apenas 1 thread pode escrever por vez.
int test2(void) {
	int block_ret[NTHREADS];

	for (int i = 0; i < NTHREADS; i++)
		spawn_thread(&tid[i], NULL, writer_thread, &block_ret[i]);
	for (int i = 0; i < NTHREADS; i++)
		pthread_join(tid[i], NULL);

	int test = 0;
    sum_array(block_ret, NTHREADS, test);

	/* Todas as escritoras exceto uma devem ter sido bloqueadas */
	return test == (NTHREADS - 1) * UNBLOCKED + STRAIGHT;
}

// Teste 03: Escritora, depois leitora.
// Requisito: A leitora deve bloquear se houver uma thread escrevendo.
int test3(void) {
	int block_ret = BLOCKED;

	spawn_thread(&tid[0], NULL, writer_thread, &block_ret);
	/* Garante que a thread pegou permissão para escrita */
	while (block_ret != STRAIGHT)
		;

	spawn_thread(&tid[1], NULL, reader_thread, &block_ret);

	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);

	/* A leitora deve ter sido bloqueada */
	return block_ret == UNBLOCKED;
}

// Teste 04: Prioridade na escrita I
// Requisito: Quando uma escritora está escrevendo e
// há escritoras leitoras bloqueadas, as escritoras
// devem ter prioridade.
int test4(void) {
	int block_ret[] = {BLOCKED, STRAIGHT, STRAIGHT};

	spawn_thread(&tid[0], NULL, writer_thread, &block_ret[0]);
	while (block_ret[0] != STRAIGHT)
		;

	spawn_thread(&tid[1], NULL, reader_thread, &block_ret[1]);
	spawn_thread(&tid[2], NULL, writer_thread, &block_ret[2]);
	
	/* Espera alguma das duas outras threads conseguir permissão */
	pthread_join(tid[0], NULL);
	while (block_ret[1] == STRAIGHT && block_ret[2] == STRAIGHT)
		;

	/* Salva status após alguma das duas conseguir permissão */
	int reader_ret = block_ret[1];
	int writer_ret = block_ret[2];

	pthread_join(tid[1], NULL);
	pthread_join(tid[2], NULL);

	/*
	 * No momento em que o status foi salvo, a escritora tem que
	 * ter tido prioridade. Após aguardar o final das threads,
	 * todas precisam ter sido desbloqueadas.
	 */
	return reader_ret == BLOCKED &&
		   writer_ret == UNBLOCKED &&
		   block_ret[1] == UNBLOCKED &&
		   block_ret[2] == UNBLOCKED;
}

// Teste 05: Prioridade na escrita II
// Requisito: Quando há leitoras lendo e uma escritora
// é bloqueada, futuras leitoras também precisam ser bloqueadas.
int test5(void) {
	int block_ret[] = {BLOCKED, STRAIGHT, STRAIGHT};

	spawn_thread(&tid[0], NULL, reader_thread, &block_ret[0]);
	while (block_ret[0] != STRAIGHT)
		;

	spawn_thread(&tid[1], NULL, writer_thread, &block_ret[1]);
	while (block_ret[1] != BLOCKED)
		;

	spawn_thread(&tid[2], NULL, reader_thread, &block_ret[2]);

	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);
	pthread_join(tid[2], NULL);

	/*
	 * A primeira leitora não deve ser bloqueada, a escritora
	 * deve ser bloqueada por causa da leitora e a segunda leitora
	 * deve ser bloqueada pois há uma escritora bloqueada.
	 */
	return block_ret[0] == STRAIGHT &&
		   block_ret[1] == UNBLOCKED &&
		   block_ret[2] == UNBLOCKED;
}

void run_test(int n, int (*test)(void)) {
	printf("Iniciando teste #%d\n", n);
	printf("Teste #%d: %s\n", n, (test()) ? "sucesso" : "falhou");
}

int main(void) {
	int (*tests[NTESTS])(void) = {test1, test2, test3, test4, test5};
	rw_init(&rw);

	for (int i = 0; i < NTESTS; i++)
		run_test(i + 1, tests[i]);

	rw_destroy(&rw);
}
