#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include<algorithm>

#define RAND_MOD 997
#define EPSILON 0.0001
#define FOR(name,initial,final) for(int name=initial;name<final;name++)
#define F first
#define S second

using namespace std;

typedef pair<int, int> PI;

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
	sort(processes.begin(),processes.end());
	double taa = 0;
	int curr = 0;
	FOR(i,0,n)
	{
		cout<<processes[i].F<<" "<<processes[i].S<<endl;
		curr += processes[i].S;
		taa += curr - processes[i].F;
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
		processes[i].F = rands[i-1];
		processes[i].S = uniform_rand_1to20();
	}
	FOR(i,0,n)
	{
		cout<<processes[i].F<<" "<<processes[i].S<<endl;
	}
	cout<<"-------------------"<<endl;
	double taa_FCS = FCFS(processes,n);
	cout<<"Average Turn around time for FCS is "<<taa_FCS<<endl;


}