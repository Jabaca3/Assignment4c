#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common_threads.h"

// If done correctly, each child should print their "before" message
// before either prints their "after" message. Test by adding sleep(1)
// calls in various locations.

// You likely need two semaphores to do this correctly, and some
// other integers to track things.

typedef struct __barrier_t {
    // add semaphores and other information here
    int n;
    int count;
    sem_t s1;
    sem_t s2;
} barrier_t;


// the single barrier we are using for this program
barrier_t b;

void barrier_init(barrier_t *b, int num_threads) {
    // initialization code goes here
    b->n = num_threads;
    b->count =0;
    sem_init(&b->s1, 0, 1);
    sem_init(&b->s2, 0, 1);
}

void barrier(barrier_t *b) {
    // barrier code goes here

    // probably lock and unlock here
    sem_wait(&b->s1);
    b->count++;
    //printf("child %s:", "waiting 1 sec");
    //sleep(1);
    sem_post(&b->s1);

    // Checking to see if all threads are enetered.
    // Beginning to unlock only one.
    if(b->count == b->n){
        sem_post(&b->s2);
    }

    //after we unlock we need to unlock the next thread...
    sem_wait(&b->s2);
    sem_post(&b->s2);
}

//
// XXX: don't change below here (just run it!)
//
typedef struct __tinfo_t {
    int thread_id;
} tinfo_t;

void *child(void *arg) {
    tinfo_t *t = (tinfo_t *) arg;
    printf("child %d: before\n", t->thread_id);
    barrier(&b);
    printf("child %d: after\n", t->thread_id);
    return NULL;
}


// run with a single argument indicating the number of 
// threads you wish to create (1 or more)
int main(int argc, char *argv[]) {
    assert(argc == 2);
    int num_threads = atoi(argv[1]);
    assert(num_threads > 0);

    pthread_t p[num_threads];
    tinfo_t t[num_threads];

    printf("parent: begin\n");
    barrier_init(&b, num_threads);
    
    int i;
    for (i = 0; i < num_threads; i++) {
	t[i].thread_id = i;
	Pthread_create(&p[i], NULL, child, &t[i]);
    }

    for (i = 0; i < num_threads; i++) 
	Pthread_join(p[i], NULL);

    printf("parent: end\n");
    return 0;
}

