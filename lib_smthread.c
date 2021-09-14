#include "lib_smthread.h"
#define MICROSECOND_TIME_QUANTA 10

static int CurrentThreadID = 1;
static int ThreadCount = 1;
static int queueflag = 0;
static int ndatval = 0;
struct itimerval timer;

/* Initialize ndat, which is used to keep a linked list for nodes containing thread info.
 */
void ndat_init() {
	ndat = malloc(sizeof(ndat));
	//ndat->head = malloc(sizeof(node));
	//ndat->tail = malloc(sizeof(node));
	//ndat->traverse = malloc(sizeof(node));
	ndat->head = NULL;
	ndat->tail = NULL;
	ndat->traverse = NULL;
}

/* User function to check on the current thread id.
 */
int getthreadid() {
	return CurrentThreadID;
}
/* Handle SIGVTALRM signals. Make sure it is safe to yield before doing so.
 */
void sigalrm_handler(int signum) {
	if (queueflag == 1) {
		return;
	}
	thread_yield();
}

/* Set up signal handling and the timer.
 */
void setup() {
	if (signal(SIGVTALRM, (void (*)(int)) sigalrm_handler) == SIG_ERR) {
		printf("\033[31mFatal Error: Unable to catch SIGVTALRM\n\033[0m");
		exit(1);
	}
	
	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = MICROSECOND_TIME_QUANTA;
	timer.it_interval = timer.it_value;
	setitimer(ITIMER_VIRTUAL, &timer, NULL);
}

/* Create a new node in the linked list for thread info.
 */
void makenode(thread_t* data) {
	if (ndatval == 0) {
		ndat_init();
		ndatval++;
	}
	if (ndat->head == NULL) {
		thread_t data;
		ndat->traverse = malloc(sizeof(node));
		ndat->traverse->prev = NULL;
		ndat->traverse->next = NULL;

		ndat->head = ndat->traverse;
		ndat->tail = ndat->traverse;
	}
	else {
		thread_t data;
		ndat->traverse = malloc(sizeof(node));
		ndat->traverse->prev = NULL;
		ndat->traverse->next = NULL;

		ndat->tail->next = ndat->traverse;
		ndat->traverse->prev = ndat->tail;
		ndat->tail = ndat->traverse;
		ndat->tail->next = ndat->head;
		ndat->head->prev = ndat->tail;
	}
	ndat->traverse->n = data;
}

/* Find a specific thread.
 */
int FindThread(const int threadId, thread_t **nthread) {
	ndat->traverse = ndat->head;
	if (threadId == 0) {
		printf("FindThread error; thread was never joined.\n"); //shouldn't ever get a threadid 0 here...
		return 1;
	}
	while (ndat->traverse != NULL) {
		if (ndat->traverse->n->threadID == threadId) {
			*nthread = ndat->traverse->n;
			return 0;
		}
		else {
			ndat->traverse = ndat->traverse->next;
		}
	}
	return 1;
}

/* Get any next thread; needed after exiting or yielding.
 */
int NextThread(const int threadId, thread_t **nthread) {
	//First search from head to find threadId
	ndat->traverse = ndat->head;
	while (ndat->traverse != NULL) {
		if (ndat->traverse->n->threadID == threadId) {
			break;
		}
		else {
			ndat->traverse = ndat->traverse->next;
		}
	}
	if (ndat->traverse->next != NULL) {
		ndat->traverse = ndat->traverse->next;
		while (ndat->traverse != NULL) {
			if (ndat->traverse->n->state == PS_ACTIVE) {
				*nthread = ndat->traverse->n;
				return 0;
			}
			else {
				ndat->traverse = ndat->traverse->next;
			}
		}
	}
	return 1;
}

/* User code to create a new thread. First time running, make some data for the main thread.
 */
int thread_create(thread_t *thread, void *(*start_routine)(void *), void *arg) {
	queueflag = 1;
	if (ThreadCount == 1) {
		thread_t* main_thread;
		if ( (main_thread = (thread_t *) malloc(sizeof(thread_t))) == 0) {
			printf("OUT OF RESOURCES\n");
			return EAGAIN;
		}
		main_thread->threadID = ThreadCount++;
		ucontext_t* context;
		if ( (context = (ucontext_t*) malloc(sizeof(ucontext_t))) == 0) {
			printf("OUT OF RESOURCES\n");
			return EAGAIN;
		}
		main_thread->ctx = context;
		main_thread->ctx->uc_stack.ss_sp = (char*) malloc(sizeof(char) * 4096);
		main_thread->ctx->uc_stack.ss_size = 4096;
		main_thread->state = PS_ACTIVE;
		makenode(main_thread);
	}
	ucontext_t* context;
	if ( (context = (ucontext_t*) malloc(sizeof(ucontext_t))) == 0) {
		printf("OUT OF RESOURCES\n");
		return EAGAIN;
	}
	thread->ctx = context;
	getcontext(thread->ctx);
	if ( (thread->ctx->uc_stack.ss_sp = (char*) malloc(sizeof(char) * 4096)) == 0) {
		printf("OUT OF RESOURCES\n");
		return EAGAIN;
	}
	thread->ctx->uc_stack.ss_size = 4096;
	thread->state = PS_ACTIVE;
	thread->threadID = ThreadCount++;
	thread->jointhread = 0;
	makecontext(thread->ctx, (void (*)()) start_routine, 1, arg);
	makenode(thread);
	setup();
	queueflag = 0;
	return 0;
}

/* User code to exit a thread. Expected for the user to use this at termination of a thread.
 */
void thread_exit(void *status) {
	queueflag = 1;
	thread_t* curr_thread;
	if (FindThread(CurrentThreadID, &curr_thread) != 0) {
		//printf("%d thread_exit() can't find thread, exiting\n", __LINE__);
		exit(0);
	}
	curr_thread->state = PS_DEAD;
	if (curr_thread->jointhread != 0) {
		thread_t* join_thread;
		if (FindThread(curr_thread->jointhread, &join_thread) != 0) {
			//printf("%d thread_exit() can't find thread, exiting\n", __LINE__);
			exit(0);
		}
		join_thread->state = PS_ACTIVE;
	}
	thread_t* next_thread;
	if (NextThread(curr_thread->threadID, &next_thread) != 0) {
		//printf("%d thread_exit() no next thread, exiting\n", __LINE__);
		exit(0);
	}
	if (CurrentThreadID == next_thread->threadID) {
		//printf(" * Same thread\n");
		setup();
		queueflag = 0;
		return;
	}
	CurrentThreadID = next_thread->threadID;
	setup();
	queueflag = 0;
	setcontext(next_thread->ctx);
}

/* User code to yield execution of thread. Also used internally by the signal handler.
 */
int thread_yield(void) {
	queueflag = 1;
	thread_t* curr_thread;
	if (FindThread(CurrentThreadID, &curr_thread) != 0) {
		//printf("%d thread_yield() can't find thread, exiting\n", __LINE__);
		exit(0);
	}
	thread_t* next_thread;
	if (NextThread(curr_thread->threadID, &next_thread) != 0) {
		//printf("%d thread_yield() no next thread, exiting\n", __LINE__);
		exit(0);
	}
	if (CurrentThreadID == next_thread->threadID) {
		setup();
		return 0;
	}

	CurrentThreadID = next_thread->threadID;
	if (curr_thread->ctx == NULL) {
		printf("\n - curr_thread ctx NULL!\n");
		setup();
		queueflag = 0;
		setcontext(next_thread->ctx);
	}
	if (next_thread->ctx == NULL) {
		printf("\n - next_thread ctx NULL!\n");
	}
	//printf("Yield prior swapcontext\n");
	setup();
	queueflag = 0;
	swapcontext(curr_thread->ctx, next_thread->ctx);
	return 0;
}

/* User code to block until a thread terminates.
 */
int thread_join(thread_t thread, void **retval) {
	int find_threadID = thread.threadID;
	thread_t* curr_thread;
	if (FindThread(CurrentThreadID, &curr_thread) != 0) {
		//printf("%d thread_join() can't find thread, exiting\n", __LINE__);
		exit(0);
	}
	thread_t* find_thread;
	if (FindThread(thread.threadID, &find_thread) != 0) {
		//printf("%d thread_join() can't find thread, exiting\n", __LINE__);
		exit(0);
	}
	if (find_thread->state != PS_ACTIVE) {
		setup();
		return 0;
	}
	else {
		curr_thread->state = PS_BLOCKED;
		find_thread->jointhread = CurrentThreadID;
		CurrentThreadID = find_threadID;
		swapcontext(curr_thread->ctx, find_thread->ctx);
	}
	setup();
	return 0;
}

/* User code to initialize a mutex before use. Required before any other mutex operations
 * are performed.
 */
int mutex_init(mutex *mtx) {
	queueflag = 1;
	mtx->owner = 0;
	mtx->locked = 0;
	queueflag = 0;
	return 0;
}

/* User code to lock a mutex. If it is locked already, block until it is available.
 */
int mutex_lock(mutex *mtx) {
	queueflag = 1;
	if (mtx->locked == 0) {
		mtx->locked = 1;
		mtx->owner = CurrentThreadID;
	}
	else {
		while(1) {
			if (mtx->locked == 1) {
				thread_yield();
			}
			else {
				mtx->locked == 1;
				mtx->owner = CurrentThreadID;
				break;
			}
		}
	}
	setup();
	queueflag = 0;
	return 0;
}

/* User code to attraverset to lock a mutex. If it is locked, don't block. Continue execution
 * in thread, but return a value to let the user code know if it is locked or not.
 */
int mutex_trylock(mutex *mtx) {
	queueflag = 1;
	if (mtx->locked == 0) {
		mtx->locked = 1;
		mtx->owner = CurrentThreadID;
		queueflag = 0;
		return 0;
	}
	queueflag = 0;
	return 1;
}

/* User code to unlock a mutex.
 */
int mutex_unlock(mutex *mtx) {
	queueflag = 1;
	if (mtx->owner == CurrentThreadID) {
		if (mtx->locked == 1) {
			mtx->owner = 0;
			mtx->locked = 0;
		}
		else {
			//printf("Already unlocked!\n");
			queueflag = 0;
			return 1;
		}
	}
	else {
		//printf("Wrong thread id in unlock!\n");
		queueflag = 0;
		return 1;
	}
}














