#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<semaphore.h>


typedef void* (*Fun)(void *arg);

struct _thpool_job_t
{
	FUN fucntion;
	void* args;
	struct _thpool_job_t *prev;
	struct _thpool_job_t *next;
}thpool_job_t;

typedef struct _thpool_job_queue
{
	thpool_job_t* head;
	thpool_job_t* tail;
	int jobN;
	sem_t* queueSem;
}thpool_job_queue;


typedef struct _thpool_t
{
	pthread_t* pthreads;
	int threadsN;
	thpool_job_queue* jobqueue;
}thpool_t;

typedef struct _thread_data
{
	pthread_mutex_t* mutex_p;
	thpool_t *tp_p;
	
}thread_data;


thpool_t* thpool_init(int threadN);
void thpool_thread_do(thpool_t* tp_p);
int thpool_add_work(thpool_t* p, void*(*function)(void *), void* arg);
void thpool_destroy(thpool_t* tp_p);
int thpool_jobqueue_init(thpool_t *tp_p);
void thpool_jobqueue_add(thpool_t *tp_p, thool_job_t* newjob_p);
int thpool_jobqueue_removelast(thpool_t* tp_p);
thpool_job_t* thpool_jobqueue_peek(thpool_t* tp_p);
void thpool_jobqueue_empty(thpool_t* tp_p);

static int thpool_keepalive = 1;

ptread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

thpool_t* thpool_init(int threadN)
{
	thpool_t* thpool;
	if(threadN <= 1)
		threadN = 1;
	thpool = (thpool_t *)malloc(sizeof(thpool_t));
	thpool->threadN = threadN;
	thpool->pthreads = (pthread_t *)malloc(threadN * sizeof(pthread_t));
	thpool_jobqueue_init(thpool);
	thpool->jobqueue->queueSem = (sem_t *)malloc(sizeof(sem_t));
	sem_init(thpool->jobqueue->queueSem, 0, 1);
	for(int t = 0; t < threadN; ++t)
	{
		pthread_create(&(thpool->threads[t]), NULL, (void *)thpool_thread_do, (void *)thpool);
	}
	return 0;
}

void thpool_thread_do(thread_t* tp_p)
{
	while(thpool_keepalive == 1)
	{
		sem_wait(tp_p->jobqueue->queueSem);
		if(thpool_keepalive)
		{
			FUNC function;
			void *arg_buff;			
			thpool_job_t* job_p;
			ptread_mutex_lock(&mutex);
			job_p = thpool_jobqueue_peek(tp_p);
			function = job_p->function;
			arg_buff = job_p->arg;
			thpool_jobqueue_romovelast(tp_p);
			pthread_mutex_unlock(&mutex);
			fuction(arg_buff);
			free(job_p);
		}
	}
	return ;
}




int thpool_jobqueue_init(thpool_t* tp_p)
{
	tp_p->jobqueue = (thpool_jobqueue *)malloc(sizeof(thpool_queue));
	tp_p->jobqueue->tail = NULL;
	tp_p->jobqueue->head = NULL;
	tp_p->jobN = 0;
	return 0;
}


thpool_job_t* thpool_jobqueue_peek(thpool_t* tp_p)
{
	return tp_p->jobqueue->tail;
}

int thpool_jobqueue_removelast(thpool_t *tp_p)
{
	if(tp_p == NULL)
		return -1;
	thpool_job_t* thelastjob;
	thelastjob = tp_p->jobqueue->tail;
	switch(tp_p->jobqueue->jobN)
	{
		case 0:
			return -1;
		case 1:
			tp_p->jobqueue->head = NULL;
			tp_p->jobqueue->tail = NULL;
			break;	
		default:	
			thelastjob->prev->next = NULL;
			tp_p->jobqueue->tail = thelastjob->prev;
	}
	(tp_p->jobqueue->jobN)--;
	int reval;
	sem_getvalue(tp_p->jobqueue->queueSem, &reval);
	return ;
}


void thpool_jobqueue_add(thpool_t* tp_p, thpool_job_t *newjob_p)
{
	newjob_p->next = NULL;
	newjob_p->prev = NULL;
	thpool_job_t* oldfirstjob = tp_p->jobqueue->head;
	switch(tp_p->jobqueue->jobN)
	{
		case 0:
			tp_p->jobqueue->head = newjob_p;
			tp_p->jobqueue->tail = newjob_p;
			break;
		default:	
			oldfirstjob->prev = newjob_p;
			newjob_p->next = oldfirstjob;
			tp_p->jobqueue->head = newjob_p;
			
	}
	(tp_p->jobqueue->jobN)++;
	sem_post(tp_p->jobqueue->queueSem);
	int reval;
	
	return;	
}


int thpool_add_work(thpool_t* tp_p, void* (*function)(void *), void* arg)
{
	thpool_job_t* newjob;
	newjob = (thpool_job_t *)malloc(sizeof(thpool_job_t));
	newjob->function = fuction;
	newjob->arg = arg;
	pthread_mutex_lock(&mutex);	
	thpool_jobqueue_add(newjob);
	pthread_mutex_unlock(&mutex);
	return 0;
}







































