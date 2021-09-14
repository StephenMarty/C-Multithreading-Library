#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include "lib_smthread.h"

#define DIVISOR 2

mutex lock;
void *func1() {
	printf("func1 Thread id: %d\n", getthreadid());
	printf("func1 starting, locking mutex.\n");
	mutex_lock(&lock);
	printf("the value is %u\n", lock.locked);
	printf("func1 got mutex...\n");
	int x = 0;
	while (x < INT_MAX/DIVISOR) {
		x++;
	}
	printf("func1 done task, unlocking\n");
	mutex_unlock(&lock);
	printf("func1 unlocked\n");
	thread_exit(0);
}

void *func2() {
	printf("func2 Thread id: %d\n", getthreadid());
	printf("func2 starting, locking mutex...\n");
	mutex_lock(&lock);
	printf("func2 got mutex.\n");
	mutex_unlock(&lock);
	printf("func2 done with mutex.\n");
	thread_exit(0);
}

int main() {
	printf("Starting\n");
	thread_t threads[2];
	mutex_init(&lock);
	thread_create(&threads[0], &func1, 0);
	thread_create(&threads[1], &func2, 0);
	thread_join(threads[0], 0);
	thread_join(threads[1], 0);
	printf("Threads done\n");
	return 0;
}
