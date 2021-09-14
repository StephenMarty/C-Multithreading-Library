#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include "lib_smthread.h"

#define DIVISOR 2

void *func1() {
	printf("In func1()...\n");
	int i;
	for (i=0; i<12; i++) {
		printf("%d ", i);
	}
	printf("\n");
	printf("Done first iteration in fmain...\n");
	int x = 0;
	for (i=0; i<3; i++) {
		while (x < INT_MAX/DIVISOR) {
			x++;
		}
	}
	printf("FUNC1 TASK COMPLETION\n");
	thread_exit(0);
}

void *func2() {
	printf("In func2()...\n");
	int x = 0;
	while (x < INT_MAX/DIVISOR) {
		x++;
	}
	printf("FUNC2 TASK COMPLETION\n");
	thread_exit(0);
}

void *func3() {
	printf("In func3()...\n");
	int i;
	int x = 0;
	while (x < (INT_MAX/6)/DIVISOR) {
		x++;
	}
	printf("FUNC3 TASK COMPLETION\n");
	thread_exit(0);
}

int main() {
	printf("Starting\n");
	thread_t threads[3];
	thread_create(&threads[0], &func1, 0);
	thread_create(&threads[1], &func2, 0);
	thread_create(&threads[2], &func3, 0);
	thread_join(threads[0], 0);
	thread_join(threads[1], 0);
	thread_join(threads[2], 0);
	printf("After thread completion\n");
	return 0;
}
