## Parallel vs Concurrent

Concurrent: Two things logically happen at the same time (Either we don't know their order, we don't care, or they actually happen simultenously).

- This is a program design question.
- This makes programs potentially harder to reason about (or easier if that's what makes sense for the program).

Parallel: Two things happen physically at the same time.

- This is a hardware question: There need to be 2 seperate pieces of execution hardware to get parallel executions
- This makes programs run fater

## Threads vs. Processes

We now have two ways to do things concurrently:
	
- fork() gets us a process
- pthread_create() gets us a threads

The big difference is shared memory

- Threads share by default
- Processes don't

# History of Concurrency

Back in the day, computers ran a single program at once.

- Business / academic systems (1975)