#include <pthread.h>

#define STRAIGHT 0  // Passou direto
#define BLOCKED 1   //  Ficou retido no wait

typedef struct Rw {
	int readers, writers;
	pthread_mutex_t mutex;
	pthread_cond_t read_cond, write_cond;
} Rw;

void rw_init(Rw *rw);
void rw_destroy(Rw *rw);
void rw_get_read(Rw *rw, int *block_check);
void rw_release_read(Rw *rw);
void rw_get_write(Rw *rw, int *block_check);
void rw_release_write(Rw *rw);
