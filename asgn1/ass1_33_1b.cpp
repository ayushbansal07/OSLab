#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string.h>

#define MAX_ARGS 100
using namespace std;

int main()
{
	while(1)
	{
		string s;
		//cin>>s;
		getline(cin,s);
		//cout<<s<<endl;
		if(s=="quit")
		{
			return 0;
		}
		int x = fork();
		if (x==0)
		{
			//execlp("g++","g++","ass1_33_1a.cpp","-o my5","-S",NULL);
			/*char *args[MAX_ARGS];
			int len = s.length();
			int nargs = 0;
			int idx = 0;
			string each = "";
			for(int i=0;i<len;i++)
			{
				if(s[i]==' ')
				{
					//cout<<s.substr(idx,i-idx).c_str()<<endl;
					//char * temp = (char*) s.substr(idx,i-idx).c_str();
					char * temp = (char *) each.c_str();
					args[nargs] = temp;
					//cout<<nargs<<' '<<args[nargs]<<endl;
					for(int j=0;j<=nargs;j++)
					{
						cout<<j<<' '<<args[j]<<endl;
					}
					idx = i+1;
					each = "";
					nargs++;
				}
				else
				{
					each += s[i];
				}
			}
			cout<<idx<<endl;
			//args[nargs] = (char*) s.substr(idx,len-idx).c_str();
			args[nargs] = (char *) each.c_str();
			//cout<<s.substr(idx,len-idx)<<endl;
			nargs++;
			for(int i=0;i<nargs;i++)
			{
				cout<<i<<' '<<args[i]<<endl;
			}
			cout<<"-----------"<<endl;
			args[nargs] = NULL;
			execvp(args[0],args);*/

			char **args;
			vector<string> args_vec;
			int len = s.length();
			int idx = 0;
			for(int i=0;i<len;i++)
			{
				if(s[i] == ' ')
				{
					args_vec.push_back(s.substr(idx,i-idx));
					idx = i+1;
				}
			}
			args_vec.push_back(s.substr(idx,len-idx));
			len = args_vec.size();
			
			args = new char*[len];
			for(int i=0;i<len;i++)
			{
				string temp = args_vec[i];
				args[i] = new char[temp.length() + 1];
				strcpy(*args[i],temp.c_str());
				*tokens
			}

			/*char ** args;
			args = new char *[s.length() +1];
			s+= "\0";
			strcpy(*args,s.c_str());
			*args = strtok(*args," ");

			execvp(args[0],args);*/

		}

	}
}