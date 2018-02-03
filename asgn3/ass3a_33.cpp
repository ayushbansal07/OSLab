#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include<algorithm>
#include <queue>

#define RAND_MOD 997
#define EPSILON 0.0001
#define FOR(name,initial,final) for(int name=initial;name<final;name++)
#define F first
#define S second

using namespace std;

typedef pair<int, int> PI;

class Compare
{
public:
	bool operator()(PI a, PI b){
		return a.S > b.S;
	}
};

double uniform_rand_generator()
{
	int n = 1 + rand()%RAND_MOD;
	if(n==0) n = 1;
	return ((double)n)/RAND_MOD;
}

int uniform_rand_1to20(){
	return 1 + rand()%20;
}

vector<int> expnential_rand_0to10(int n){
	vector<double> v(n);
	double maxm = 0;
	for(int i=0;i<n;i++)
	{
		double lg = log(uniform_rand_generator());
		v[i] = -1.0*lg;
		maxm = max(maxm,v[i]);
	}
	vector<int> ans(n);
	for(int i=0;i<n;i++)
	{
		ans[i] = (int)((v[i]/maxm)*10.0);
	}
	return ans;
}

double FCFS(vector<PI> processes, int n)
{
	double taa = 0;
	int curr = 0;
	FOR(i,0,n)
	{
		curr += processes[i].S;
		taa += curr - processes[i].F;
	}

	return taa/n;
}

double round_robin(vector<PI> processes, int n, int delta)
{
	double taa = 0;
	queue<PI> q;
	q.push(processes[0]);
	int j = 1;
	int curr = 0;
	while(!q.empty() || j<n)
	{
		//if q is empty
		if(q.empty())
		{
			q.push(processes[j]);
			curr = processes[j].F;
			j++;
			//if(j>=n) break;
		}


		PI pr = q.front();
		q.pop();
		int next_tq = curr + delta;
		int finishtime = curr + pr.S;
		while(j<n && processes[j].F<=finishtime && processes[j].F<=next_tq)
		{
			q.push(processes[j]);
			j++;
		}
		if(finishtime <= next_tq)
		{
			taa += finishtime - pr.F;
			while(j<n && processes[j].F<=next_tq)
			{
				q.push(processes[j]);
				j++;
			}
		}
		else
		{
			pr.S = finishtime - next_tq;
			q.push(pr);
		}
		curr = next_tq;


	}

	return taa/n;
}

double SJF(vector<PI>processes, int n)
{
	double taa = 0;
	priority_queue<PI, vector<PI>, Compare> q;
	q.push(processes[0]);
	int j = 1;
	int curr = 0;
	while(!q.empty() || j<n)
	{
		//q empty
		if(q.empty())
		{
			q.push(processes[j]);
			curr = processes[j].F;
			j++;
		}


		PI pr = q.top();
		q.pop();
		int finishtime = curr + pr.S;
		if(j<n && processes[j].F <= finishtime)
		{
			q.push(processes[j]);
			pr.S = finishtime - processes[j].F;
			q.push(pr);
			curr = processes[j].F;
			j++;
		}
		else
		{
			curr = finishtime;
			taa += curr - pr.F;
		}
	}

	return taa/n;

}

int main()
{
	int n;
	cout<<"Enter the Number of Processes, N"<<endl;
	cin>>n;
	vector<PI> processes(n);
	vector<int> rands = expnential_rand_0to10(n-1);
	processes[0].F = 0;
	processes[0].S = uniform_rand_1to20();
	FOR(i,1,n)
	{
		processes[i].F = rands[i-1] + processes[i-1].F;
		processes[i].S = uniform_rand_1to20();
	}
	cout<<"Arrival Time\t\tCPU Burst Time"<<endl;
	FOR(i,0,n)
	{
		cout<<processes[i].F<<"\t\t\t"<<processes[i].S<<endl;
	}
	cout<<"-------------------"<<endl;
	double taa_FCS = FCFS(processes,n);
	cout<<"Average Turn around time for FCS is "<<taa_FCS<<endl;

	double taa_SJF = SJF(processes,n);
	cout<<"Average Turn around time for Pre-emptive SJF is "<<taa_SJF<<endl;

	double taa_rr1 = round_robin(processes, n, 1);
	cout<<"Average Turn around time for RR with delta = 1 is "<<taa_rr1<<endl;

	double taa_rr2 = round_robin(processes, n, 2);
	cout<<"Average Turn around time for RR with delta = 2 is "<<taa_rr2<<endl;

	double taa_rr5 = round_robin(processes, n, 5);
	cout<<"Average Turn around time for RR with delta = 5 is "<<taa_rr5<<endl;



}