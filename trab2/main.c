#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include "rw.h"
#include "util.h"

#define BUFFER_SIZE 60
#define WRITE 30
#define HIGH 35

#define NORMAL 0
#define YELLOW 1
#define RED 2

typedef struct Temperature {
	double temperature;
	int id;
} Temperature;

typedef struct ThreadArgs {
	int id;
	int *block_ret;
} ThreadArgs;

void check_array(int id);
double get_temperature_rand(void);
void add_temp(Temperature temperature);

Temperature array[BUFFER_SIZE];
Rw rw;
int size = 0;
int next = 0;

void *reader_function(void *args_ptr) {
	ThreadArgs *args = (ThreadArgs *) args_ptr;
    while (1) {
        rw_get_read(&rw, args->block_ret);
		check_array(args->id);
        rw_release_read(&rw);
        sleep(2);
    }

	pthread_exit(NULL);
}

void *writer_function(void *args_ptr) {
	ThreadArgs *args = (ThreadArgs *) args_ptr;
    while (1) {
        rw_get_write(&rw, args->block_ret);

		double num = get_temperature_rand();
		if (num > WRITE) {
			Temperature temperature = {.temperature = num, .id = args->id};
			add_temp(temperature);
		}

        rw_release_write(&rw);

        sleep(1);
    }

	pthread_exit(NULL);
}

double get_temperature_rand(void) {
    double x = rand() % 150;
    return (x / 10.0) + 25;
}

void add_temp(Temperature temperature) {
	array[next] = temperature;
	next = (next + 1) % BUFFER_SIZE;
	if (size != BUFFER_SIZE)
		size++;
}

void check_array(int id) {
	int count = 0;
	int high = 0;
	int ret = NORMAL;
	double sum = 0;
	for (int i = 0; i < size; i++) {
		if (array[i].id != id)
			continue;

		sum += array[i].temperature;
		count++;
		if (array[i].temperature > HIGH)
			high++;

		if (count == 5 && high == 5)
			ret = RED;

		if (ret != RED && count <= 15 && high >= 5)
			ret = YELLOW;
	}

	switch (ret) {
		case NORMAL:
			printf("Sensor %d: condição normal.\n", id);
			break;
		case YELLOW:
			printf("Sensor %d: alerta amarelo.\n", id);
			break;
		case RED:
			printf("Sensor %d: alerta vermelho.\n", id);
			break;
		default:
			break;
	}

	if (count != 0)
		printf("Sensor %d: média %.2f\n", id, sum / count);
}

int main(int argc, char **argv) {

    srand(time(NULL));
	rw_init(&rw);

    if(argc < 2) {
        printf("Digite: %s <numero de sensores / escritores>\n", argv[0]);
        return 1;
    }

    char *endptr;
    int N_THREADS = strtol(argv[1], &endptr, 0);

    pthread_t *tid_write;
    pthread_t *tid_read;
	ThreadArgs *args_write;
	ThreadArgs *args_read;
	int *block_ret_write;
	int *block_ret_read;

	tid_write = malloc(N_THREADS * sizeof(pthread_t));
	tid_read = malloc(N_THREADS * sizeof(pthread_t));
	args_write = malloc(N_THREADS * sizeof(ThreadArgs));
	args_read = malloc(N_THREADS * sizeof(ThreadArgs));
	block_ret_write = malloc(N_THREADS * sizeof(int));
	block_ret_read = malloc(N_THREADS * sizeof(int));

	for (int i = 0; i < N_THREADS; i++) {
		args_write[i].id = i;
		args_write[i].block_ret = &block_ret_write[i];
		pthread_create(&tid_write[i], NULL, writer_function, &args_write[i]);

		args_read[i].id = i;
		args_read[i].block_ret = &block_ret_read[i];
		pthread_create(&tid_read[i], NULL, reader_function, &args_read[i]);
	}

	for (int i = 0; i < N_THREADS; i++) {
		pthread_join(tid_write[i], NULL);
		pthread_join(tid_read[i], NULL);
	}

	free(tid_write);
	free(args_write);
	free(block_ret_write);
	free(tid_read);
	free(args_read);
	free(block_ret_read);
	rw_destroy(&rw);
}
