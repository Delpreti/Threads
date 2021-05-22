#include <pthread.h>
#include "rw.h"

void rw_init(Rw *rw) {
	rw->readers = 0;
	rw->writers = 0;
	pthread_mutex_init(&rw->mutex, NULL);
	pthread_cond_init(&rw->read_cond, NULL);
	pthread_cond_init(&rw->write_cond, NULL);
}

void rw_destroy(Rw *rw) {
	pthread_mutex_destroy(&rw->mutex);
	pthread_cond_destroy(&rw->read_cond);
	pthread_cond_destroy(&rw->write_cond);
}

void rw_get_read(Rw *rw, int *block_check) {
	pthread_mutex_lock(&rw->mutex);
	*block_check = STRAIGHT;
	while (rw->writers > 0) {
		*block_check = BLOCKED;
		pthread_cond_wait(&rw->read_cond, &rw->mutex);
	}

	if (*block_check == BLOCKED)
		*block_check = UNBLOCKED;

	rw->readers++;
	pthread_mutex_unlock(&rw->mutex);
}

void rw_release_read(Rw *rw) {
	pthread_mutex_lock(&rw->mutex);
	rw->readers--;
	if (rw->readers == 0)
		pthread_cond_signal(&rw->write_cond);
	pthread_mutex_unlock(&rw->mutex);
}

void rw_get_write(Rw *rw, int *block_check) {
	pthread_mutex_lock(&rw->mutex);
	*block_check = STRAIGHT;
	while (rw->writers > 0 || rw->readers > 0) {
		*block_check = BLOCKED;
		pthread_cond_wait(&rw->write_cond, &rw->mutex);
	}

	if (*block_check == BLOCKED)
		*block_check = UNBLOCKED;

	rw->writers++;
	pthread_mutex_unlock(&rw->mutex);
}

void rw_release_write(Rw *rw) {
	pthread_mutex_lock(&rw->mutex);
	rw->writers--;
	pthread_cond_signal(&rw->write_cond);
	pthread_cond_broadcast(&rw->read_cond);
	pthread_mutex_unlock(&rw->mutex);
}
