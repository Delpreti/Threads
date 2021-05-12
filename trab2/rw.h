#include <pthread.h>

typedef struct Rw {
	int readers, writers;
	pthread_mutex_t mutex;
	pthread_cond_t read_cond, write_cond;
} Rw;

void rw_init(Rw *rw);
void rw_destroy(Rw *rw);
void rw_get_read(Rw *rw);
void rw_release_read(Rw *rw);
void rw_get_write(Rw *rw);
void rw_release_write(Rw *rw);
