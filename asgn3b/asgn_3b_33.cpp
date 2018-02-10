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
#define REPORT_FREQ 500000

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
	//cout<<tid<<endl;
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
		next_proc = get_next_process(stat.curr);
	}	
	cout<<"Scheduler exiting"<<endl;
}

void *reporter_func(void * threadid)
{
	int last = stat.curr;
	vector<bool> already_terminated(n,false);
	while(1)
	{
		if(last != stat.curr)
		{
			cout<<"Worker "<<last<<" put to sleep, Worker "<<stat.curr<<" woken up"<<endl;
			last = stat.curr;
		}
		FOR(i,0,n)
		{
			if(already_terminated[i] != stat.terminated[i]){
				cout<<"Worker "<<i<<" Terminated..."<<endl;
				already_terminated[i] = true;
			}
		}

		usleep(REPORT_FREQ);
	}
}

int main()
{
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
		pthread_kill(WORKERS[i],SIGUSR1);
	}
	pthread_create(&SCHEDULER, NULL, scheduler_func,(void *) "Scheduler");
	pthread_create(&REPORTER, NULL, reporter_func,(void *) "Reporter");

	pthread_kill(WORKERS[0],SIGUSR2);
	pthread_join(SCHEDULER,NULL);


}