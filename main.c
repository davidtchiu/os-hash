#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ts_hashmap.h"

#define NUM_OPS_PER_THREAD 10000
#define MAX_KEY_VAL 100

/**
 * Work for each thread. Has a 50% chance to put, 25% chance to del, or get
 * @param args a void pointer to a map
 */
void* threadwork(void* args) {
	ts_hashmap_t *map = (ts_hashmap_t*) args;
	int r = 0;
	int key = 0;
	for (int i = 0; i < NUM_OPS_PER_THREAD; i++) {
		r = rand() % 10;
		key = rand() % (1+MAX_KEY_VAL);
		if (r < 5) put(map, key, key);
		else if (r < 8) get(map, key);
		else del(map, key);
	}
	return NULL;
}

int main(int argc, char *argv[]) {
	if (argc < 3) {
		printf("Usage: %s <num threads> <hashmap capacity>\n", argv[0]);
		return 1;
	}

	srand(time(NULL));	// seed to 0 for repetaability
	int num_threads = atoi(argv[1]);
	int capacity = (unsigned int) atoi(argv[2]);
	ts_hashmap_t *map = initmap(capacity);

	// spawn threads
	pthread_t *threads = (pthread_t*) malloc(sizeof(pthread_t) * num_threads);
	for (int i = 0; i < num_threads; i++) {
		pthread_create(&threads[i], NULL, threadwork, map);
	}

	// join threads
	for (int i = 0; i < num_threads; i++) {
		pthread_join(threads[i], NULL);
	}
	
	// print content
	printmap(map);

	// clean up memory
	free(threads);
	freeMap(map);

	return 0;
}