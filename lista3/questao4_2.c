#include <semaphore.h>

#define N 10
#define C 10

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

int main(void) {};
