#include <stdio.h>
#include <unistd.h>
#include "lib_smthread.h"

void *func() {
	printf("Yielding\n");
	thread_yield();
	thread_exit(0);
}

int main() {
	printf("Starting\n");
	thread_t threads[6];
	int i;
	for (i=0; i<6; i++) {
		thread_create(&threads[i], &func, 0);
	}
	thread_join(threads[0], 0);
	thread_join(threads[1], 0);
	thread_join(threads[2], 0);
	thread_join(threads[3], 0);
	thread_join(threads[4], 0);
	thread_join(threads[5], 0);
	printf("\x1B[32mAfter thread completion\n");
	return 0;
}
