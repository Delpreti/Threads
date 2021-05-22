#include <pthread.h>

/* A chamada não bloqueou */
#define STRAIGHT 0

/* A chamada bloqueou e ainda está bloqueada */
#define BLOCKED 1

/* A chamada bloqueou mas já foi desbloqueada */
#define UNBLOCKED 2

typedef struct Rw {
	int readers, writers;
	pthread_mutex_t mutex;
	pthread_cond_t read_cond, write_cond;
} Rw;

/* Inicializa um struct Rw */
void rw_init(Rw *rw);

/* Libera os recursos de um struct Rw inicializados por rw_init */
void rw_destroy(Rw *rw);

/*
 * Requisita permissão para leitura. O status do bloqueio será escrito
 * em block_check usando as macros acima definidas.
 */
void rw_get_read(Rw *rw, int *block_check);

/* Devolve permissão para leitura */
void rw_release_read(Rw *rw);

/*
 * Requisita permissão para escrita. O status do bloqueio será escrito
 * em block_check usando as macros acima definidas.
 */
void rw_get_write(Rw *rw, int *block_check);

/* Devolve permissão para leitura */
void rw_release_write(Rw *rw);
