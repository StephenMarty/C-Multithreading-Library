#ifndef H_LIB_SMTHREAD
#define H_LIB_SMTHREAD

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <ucontext.h>
#include <sys/time.h>

enum thread_error {
	EAGAIN = 1,
	EINVAL = 2,
	EPERM = 3
};

enum thread_state {
	PS_ACTIVE = 1,
	PS_BLOCKED = 2,
	PS_DEAD = 3
};

typedef struct {
	int threadID;
	int jointhread;
	ucontext_t* ctx;
	enum thread_state state;
} thread_t;

typedef struct mutex {
	int locked;
	int owner;
} mutex;

typedef struct node {
	struct node *next;
	struct node *prev;
	thread_t *n;
} node;

struct ndat {
	node *head;
	node *tail;
	node *traverse;
} *ndat;

int thread_create(thread_t *thread, void *(*start_routine) (void *), void *arg);

void thread_exit(void *status);

int thread_yield(void);

int thread_join(thread_t thread, void **retval);

int mutex_init(mutex *mtx);

int mutex_lock(mutex *mtx);

int mutex_trylock(mutex *mtx);

int mutex_unlock(mutex *mtx);

int getthreadid(); 

#endif
