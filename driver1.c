#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include "lib_smthread.h"

#define DIVISOR 5

int globintx = 0;

void *fmain() {
	printf("In fmain, performing task...\n");
	globintx++;
	int i;
	for (i=0; i<12; i++) {
		printf("%d ", i);
	}
	globintx++;
	printf("\n");
	printf("Done first iteration in fmain...\n");
	globintx++;
	int x = 0;
	for (i=0; i<3; i++) {
		while (x < INT_MAX/DIVISOR) {
			x++;
		}
	}
	printf("FMAIN TASK COMPLETION\n");
	thread_exit(0);
}

void *func2() {
	printf("In func2, performing task...\n");
	int x = 0;
	while (x < INT_MAX/DIVISOR) {
		x++;
	}
	printf("FUNC2 TASK COMPLETION\n");
	thread_exit(0);
}

int main() {
	printf("Starting\n");
	thread_t mthread;
	thread_t thread2;
	thread_create(&mthread, &fmain, 0);
	thread_create(&thread2, &func2, 0);
	thread_join(mthread, 0);
	thread_join(thread2, 0);
	printf("globintx: %d\n", globintx);
	printf("After thread completion\n");
	return 0;
}
