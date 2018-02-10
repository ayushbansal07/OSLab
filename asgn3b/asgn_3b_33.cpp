#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string>
#include <string.h>
#include <signal.h>
#include <.h>

#define FOR(name,initial,final) for(long name=initial;name<final;name++)

using namespace std;

void sig_sleep(int sigid)
{
	pause();
	signal(SIGUSR1,sig_sleep);
}

void sig_wake(int sigid)
{
	signal(SIGUSR2,sig_wake);
}

void *worker_func(void * threadid)
{
	unsigned int seed = (unsigned int) threadid;
	//Generate 1000 and integers
	//sort the thousand integers
	int to_sleep = 1 + rand_r(&seed)%10;
	sleep(to_sleep);
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

	int n;
	int rc;
	cout<<"Enter N"<<endl;
	cin>>n;

	pthread_t WORKERS[n];

	FOR(i,0,n)
	{
		rc = pthread_create(&WORKERS[i],NULL,worker_func, (void *) i);
	}






}