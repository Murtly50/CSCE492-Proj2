#include <pthread.h>
#include "Semaphore.h"

#include <stdio.h>

/*************************************************************************************
 * Semaphore (constructor) - this should take count and place it into a local variable.
 *						Here you can do any other initialization you may need.
 *
 *    Params:  count - initialization count for the semaphore
 *
 *************************************************************************************/

Semaphore::Semaphore(int count) {
  value = count;
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond, NULL);
}


/*************************************************************************************
 * ~Semaphore (destructor) - called when the class is destroyed. Clean up any dynamic
 *						memory.
 *
 *************************************************************************************/

Semaphore::~Semaphore() {
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&cond);
}


/*************************************************************************************
 * wait - implement a standard wait Semaphore method here
 *
 *************************************************************************************/

void Semaphore::wait() {
  pthread_mutex_lock(&mutex);
  value--;
  if (value < 0) {
    pthread_cond_wait(&cond, &mutex);
  }
  pthread_mutex_unlock(&mutex);
}


/*************************************************************************************
 * signal - implement a standard signal Semaphore method here
 *
 *************************************************************************************/

void Semaphore::signal() {
  pthread_mutex_lock(&mutex);
  value++;
  if (value <= 0) {
    pthread_cond_signal(&cond);
  }
  pthread_mutex_unlock(&mutex);
}
