#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fcntl.h>
#include <string.h>
#include <vector>
#include <sys/wait.h>
#include <sys/shm.h>

using namespace std;

#define SIZE_OF_FIFO 5
#define NO_OF_PRIMES 100

struct fifo{
	int front;
	int end;
	int data[SIZE_OF_FIFO];
};

vector<int> getPrimes(int n)
{
	vector<bool> v(n*n,true);
	vector<int> ans;
	int n2 = n*n;
	int ct = 0;
	
	for(int i=2;i<n2;i++)
	{
		if(v[i])
		{
			ans.push_back(i);
			ct++;
			if(ct==NO_OF_PRIMES)
				break;
			for(int j = 2*i;j<n2;j+=i)
			{
				v[j] = false;
			}
		}
	}
	return ans;	
}

bool isFull(fifo *f)
{
	if(f->front == (f->end+1)%SIZE_OF_FIFO)
		return true;
	return false;
}

bool isEmpty(fifo * f)
{
	if(f->front == f->end)
		return true;
	return false;
}

bool insert(fifo * f, int v)
{
	if(isFull(f))
		return false;
	f->data[f->end] = v;
	f->end = (f->end + 1)%SIZE_OF_FIFO;
	return true;
}

int dequeue(fifo *f)
{
	if(isEmpty(f))
		return -1;
	int out = f->data[f->front];
	f->front = (f->front + 1)%SIZE_OF_FIFO;
	return out;
}

int main()
{

	vector<int> primes = getPrimes(NO_OF_PRIMES);

	int np, nc;
	cin>>np>>nc;

	vector<int> producers(np);
	vector<int> consumers(nc);

	int key = 1234;



	int shm_id = shmget(key, sizeof(fifo),IPC_CREAT | 0666);
	if(shm_id < 0)
	{
		cout<<"Error shmget()"<<endl;
		exit(-1);
	}

	fifo * list = (fifo*) shmat(shm_id, NULL, 0);
	if(list == (fifo *)-1)
	{
		cout<<"shmat Error in parent"<<endl;
		exit(-1);
	}

	list->front = 0;
	list->end = 0;



	for(int i=0;i<np;i++)
	{
		int x = fork();
		producers[i] = x;
		if(x==0)
		{
			unsigned int seed = i;
			int idx = rand_r(&seed)%100;
			int prime = primes[idx];

			double time = ((double)rand_r(&seed))/RAND_MAX;
			time *= 5.0;
			sleep(time);

			fifo * ptr =  (fifo *)shmat(shm_id, NULL, 0);
			if(ptr == (fifo *)-1)
			{
				cout<<"shmat Error in Producer "<<i<<endl;
				exit(-1);
			}
			while(isFull(ptr)){}
			insert(ptr, prime);
			cout<<"Producer "<<i<<": "<<prime<<", Time: "<<time<<endl;
			shmdt(ptr);
			break;
		}

	}

	for(int i=0;i<nc;i++)
	{
		int x = fork();
		consumers[i] = x;
		if(x==0)
		{
			unsigned int seed = i;
			double time = ((double)rand_r(&seed))/RAND_MAX;
			time *= 5.0;
			sleep(time);

			fifo * ptr =  (fifo *)shmat(shm_id, NULL, 0);
			if(ptr == (fifo *)-1)
			{
				cout<<"shmat Error in Consumer "<<i<<endl;
				exit(-1);
			}
			while(isEmpty(ptr)){}
			int result = dequeue(ptr);
			cout<<"Consumer "<<i<<": "<<result<<", Time: "<<time<<endl;
			shmdt(ptr);
			break;
		}
	}

	sleep(30);

	shmdt(list);

	for(int i=0;i<np;i++)
	{
		kill(producers[i],SIGKILL);
	}
	for(int i=0;i<nc;i++)
	{
		kill(consumers[i],SIGKILL);
	}




}