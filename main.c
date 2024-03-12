#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "rtclock.h"
#include "ts_hashmap.h"

#define NUM_OPS_PER_THREAD 10000

// globals
ts_hashmap_t *map;
int maxKey;

/**
 * Work for each thread. Has a 50% chance to put, 25% chance to del, or get
 * @param args a void pointer to a map
 */
void* threadwork(void* args) {
	int r = 0;
	int key = 0;
	for (int i = 0; i < NUM_OPS_PER_THREAD; i++) {
		r = rand() % 10;
		key = rand() % (1+maxKey);
		if (r < 5) put(map, key, key);
		else if (r < 8) get(map, key);
		else del(map, key);
	}
	return NULL;
}

int main(int argc, char *argv[]) {
	if (argc < 4) {
		printf("Usage: %s <num threads> <hashmap capacity> <max key>\n", argv[0]);
		return 1;
	}

	double startTime = rtclock();
	srand(time(NULL));
	int num_threads = (unsigned int) atoi(argv[1]);
	int capacity = (unsigned int) atoi(argv[2]);
	maxKey = (unsigned int) atoi(argv[3]);
	map = initmap(capacity);

	// spawn threads
	pthread_t *threads = (pthread_t*) malloc(sizeof(pthread_t) * num_threads);
	for (int i = 0; i < num_threads; i++) {
		pthread_create(&threads[i], NULL, threadwork, NULL);
	}

	// join threads
	for (int i = 0; i < num_threads; i++) {
		pthread_join(threads[i], NULL);
	}

	double endTime = rtclock();

	// print content and timing results
	printmap(map);
	printf("Number of ops: %d, Time elapsed: %.6f sec\n", map->numOps, endTime-startTime);

	// clean up memory
	free(threads);
	freeMap(map);

	return 0;
}