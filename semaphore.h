#ifndef SUDOKU_SEMAPHORE_H
#define SUDOKU_SEMAPHORE_H

#include <pthread.h>

typedef struct Sem *sem;
typedef struct Sem_queue *sem_queue;

struct Sem
{
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    int value;
};

sem  sem__init(int value);
void sem_down(sem s);
void sem_up(sem s);

#endif
