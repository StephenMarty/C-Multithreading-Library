#include <stdio.h>
#include <unistd.h>
#include "lib_smthread.h"

void *func1() {
	printf("Yielding\n");
	thread_yield();
	printf("func1 after yield, exiting\n");
	thread_exit(0);
}

void *func2() {
	printf("Start func2\n");
	int i=0;
	for (i=0; i<256; i++) {
		if (i%2==0) {
			printf("%d ", i);
		}
	}
	printf("\nDone in func2\n");
	thread_exit(0);
}



int main() {
	printf("Starting\n");
	thread_t threads[2];
	thread_create(&threads[0], &func1, 0);
	thread_create(&threads[1], &func2, 0);
	thread_join(threads[0], 0);
	thread_join(threads[1], 0);
	printf("\x1B[32mAfter thread completion\n");
	return 0;
}
