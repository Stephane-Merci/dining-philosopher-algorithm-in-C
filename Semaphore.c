
// These are the necessary header files for using pthreads, semaphores, and standard input/output functions.
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

#define N 5                     // Definitions for the number of philosophers (N) and
#define THINKING 2
#define HUNGRY 1                // three possible states: thinking, hungry, and eating.
#define EATING 0
#define LEFT (phnum + 4) % N    // The macros LEFT and RIGHT represent the indices of the
#define RIGHT (phnum + 1) % N   // left and right neighbours of a philosopher.

// global variables
int state[N]; // An array representing the state of each philosopher.
int phil[N] = { 0, 1, 2, 3, 4 }; // An array representing the identifiers of each philosopher.

sem_t mutex; // A semaphore for mutual exclusion to protect critical sections.(pickup or put it down)
sem_t S[N]; // An array of semaphores, one for each philosopher, to control their synchronization.(behaviour)

// This function checks if a philosopher can start eating.
// If a philosopher is hungry and its neighbours are not eating,
// it changes its state to eating, prints a message,
// and posts a semaphore to wake up a potentially waiting philosopher.
void test(int phnum)
{
	if (state[phnum] == HUNGRY
		&& state[LEFT] != EATING
		&& state[RIGHT] != EATING) {
		// state that eating
		state[phnum] = EATING;

		sleep(2);

		printf("Philosopher %d takes fork %d and %d\n",
					phnum + 1, LEFT + 1, phnum + 1);

		printf("Philosopher %d is Eating\n", phnum + 1);

		// sem_post(&S[phnum]) has no effect
		// during takefork
		// used to wake up hungry philosophers
		// during putfork
		sem_post(&S[phnum]);
	}
}

// This function represents a philosopher attempting to pick up chopsticks.
// It updates the philosopher's state to hungry and tries to eat.
// If unsuccessful, it waits on a semaphore until it gets signalled.
void take_fork(int phnum)
{

	sem_wait(&mutex);

	// state that hungry
	state[phnum] = HUNGRY;

	printf("Philosopher %d is Hungry\n", phnum + 1);

	// eat if neighbours are not eating
	test(phnum);

	sem_post(&mutex);

	// if unable to eat wait to be signalled
	sem_wait(&S[phnum]);

	sleep(1);
}

// This function represents a philosopher putting down chopsticks.
// It updates the philosopher's state to thinking and signals its neighbours.
void put_fork(int phnum)
{

	sem_wait(&mutex);

	// state that thinking
	state[phnum] = THINKING;

	printf("Philosopher %d putting fork %d and %d down\n",
		phnum + 1, LEFT + 1, phnum + 1);
	printf("Philosopher %d is thinking\n", phnum + 1);

	test(LEFT);
	test(RIGHT);

	sem_post(&mutex);
}

// This function is the main behaviour of each philosopher thread.
// It repeatedly alternates between picking up and putting down chopsticks with delays in between.
void* philosopher(void* num)
{

	while (1) {

		int* i = num;

		sleep(1);

		take_fork(*i);

		sleep(0);

		put_fork(*i);
	}
}

// In the main function, semaphores are initialized, and philosopher threads are created.
// Each philosopher starts in a thinking state.
// The main thread then waits for the philosopher threads to finish.
int main()
{

	int i;
	pthread_t thread_id[N];

	// initialize the semaphores
	sem_init(&mutex, 0, 1);

	for (i = 0; i < N; i++)

		sem_init(&S[i], 0, 0);

	for (i = 0; i < N; i++) {

		// create philosopher processes
		pthread_create(&thread_id[i], NULL,
					philosopher, &phil[i]);

		printf("Philosopher %d is thinking\n", i + 1);
	}

	for (i = 0; i < N; i++)

		pthread_join(thread_id[i], NULL);
}

// One common solution to the Dining Philosopher Problem uses semaphores,
// a synchronization mechanism that can be used to control access to shared resources.
// In this solution, each fork is represented by a semaphore, and a philosopher must acquire
// both the semaphore for the fork to their left and the semaphore for the fork to their right
// before they can begin eating. If a philosopher cannot acquire both semaphores, they must wait until they become available.
