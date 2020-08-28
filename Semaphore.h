#ifndef SEMAPHORE_H
#define SEMAPHORE_H

class Semaphore
{
public:
	Semaphore(int count);
	~Semaphore();

	void wait();
	void signal();

private:
	int value;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
};

#endif
