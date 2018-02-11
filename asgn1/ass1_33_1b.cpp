#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <ctime>

#define MAX_ARGS 100
using namespace std;

int main()
{
	while(1)
	{
		string s;
		//cin>>s;
		getline(cin,s);
		cout<<s<<endl;
		if(s=="quit")
		{
			return 0;
		}
		int x = fork();
		if (x==0)
		{
			//execlp("g++","g++","ass1_33_1a.cpp","-o my5","-S",NULL);
			char *args[MAX_ARGS];
			int len = s.length();
			int nargs = 0;
			int idx = 0;
			for(int i=0;i<len;i++)
			{
				if(s[i]==' ')
				{
					//cout<<s.substr(idx,i-idx).c_str()<<endl;
					char * temp = (char*) s.substr(idx,i-idx).c_str();
					args[nargs] = temp;
					//cout<<nargs<<' '<<args[nargs]<<endl;
					for(int j=0;j<=nargs;j++)
					{
						cout<<j<<' '<<args[j]<<endl;
					}
					idx = i+1;
					nargs++;
				}
			}
			cout<<idx<<endl;
			args[nargs] = (char*) s.substr(idx,len-idx).c_str();
			cout<<s.substr(idx,len-idx)<<endl;
			nargs++;
			for(int i=0;i<nargs;i++)
			{
				cout<<i<<' '<<args[i]<<endl;
			}
			cout<<"-----------"<<endl;
			args[nargs] = NULL;
			execvp(args[0],args);
		}

	}
}