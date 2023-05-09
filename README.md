# C-Multithreading-Library
This multithreading system uses timers and signals to alternate between thread execution. A timeslice of 10 microseconds is given, though this may not always be the exact amount of time given. A flag is set to make sure that if some particular task is being done that would have consequences if the context is switched, it will not be switched until finished. If the flag is not set and the timer expires, the signal handler will yield to another thread. Any number of threads may be created due to the use of a linked list type of data structure. Thread joining and mutex locks are supported.

The makefile creates a static library which is used by the test drivers. The makefile also has some targeted clean instructions for removing specific files.
