#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string>
#include <string.h>
#include <signal.h>
#include <vector>
#include <unistd.h>

#define FOR(name,initial,final) for(long name=initial;name<final;name++)
#define TIME_QUANTUM 1

using namespace std;

struct STATUS{
	vector<bool> terminated;
	int curr;
};

vector<pthread_t> WORKERS;

int n;

STATUS stat;


void sig_sleep(int sigid)
{
	pause();
}

void sig_wake(int sigid){}

void *worker_func(void * threadid)
{
	long tid = (long) threadid;
	cout<<tid<<endl;
	unsigned int seed = (unsigned int) tid;
	//Generate 1000 and integers
	//sort the thousand integers
	int to_sleep = 1 + rand_r(&seed)%10;
	sleep(to_sleep);
	stat.terminated[tid] = true;
}

int get_next_process(int curr)
{
	FOR(i,1,n+1)
	{
		if(!stat.terminated[(i+curr)%n])
			return (i+curr)%n;
	}
	return -1;
}

void *scheduler_func(void * threadid)
{
	int next_proc = get_next_process(stat.curr);
	while(next_proc != -1)
	{
		pthread_kill(WORKERS[stat.curr],SIGUSR1);
		//pthread_kill(WORKERS[(stat.curr+1)%n]);
		stat.curr = next_proc;
		pthread_kill(WORKERS[stat.curr],SIGUSR2);
		sleep(TIME_QUANTUM);
	}	
	cout<<"Scheduler exiting"<<endl;
}

void *reporter_func(void * threadid)
{

}

int main()
{
	/*pthread_t t1, t2;
	int a1,a2;



	a1 = pthread_create(&t1, NULL, thread_func,(void *)"Thread 1");
	if(a1)	cout<<"Error creating Thread"<<endl;

	a2 = pthread_create(&t2, NULL, thread_func ,(void *)"Thread 2");
	if(a2)	cout<<"Error creating Thread"<<endl;

	pthread_join(t1,NULL);
	pthread_join(t2,NULL);
*/

	signal(SIGUSR1,sig_sleep);
	signal(SIGUSR2,sig_wake);

	int rc;
	cout<<"Enter N"<<endl;
	cin>>n;
	stat.terminated.resize(n);
	FOR(i,0,n) stat.terminated[i] = false;
	stat.curr = 0;

	WORKERS.resize(n);
	pthread_t SCHEDULER, REPORTER;

	FOR(i,0,n)
	{
		rc = pthread_create(&WORKERS[i],NULL,worker_func, (void *) i);
		//Uncomment if not starting with thread 0
		//pthread_kill(WORKERS[i],SIGUSR1);
		pthread_join(WORKERS[i],NULL);

	}

	pthread_create(&SCHEDULER, NULL, scheduler_func,(void *) "Scheduler");
	pthread_create(&REPORTER, NULL, reporter_func,(void *) "Reporter");

	pthread_join(SCHEDULER,NULL);







}