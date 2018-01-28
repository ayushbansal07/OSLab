#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fcntl.h>
#include <string.h>
#include <vector>
#include <sys/wait.h>

#define MAX_ARGS 100
#define MAX_PIPES 20

using namespace std;

int get_args(char *line,char **args)
{
	int argno=1;
	*args = line;
	while(*line != '\0')
	{
		if (*line == ' ')
		{
			*line++ = '\0';
			*args++;
			*args = line;
			argno++;
		}
		*line++;

	}
	// *args = '\0';
	return argno;
}

int get_args_redirect(char *line,char **args)
{
	int argno=1;
	*args = line;
	while(*line != '\0')
	{
		if (*line == ' ')
		{
			*line++ = '\0';
			*args++;
			*args = line;
			argno++;
		}
		if(*line == '<' || *line =='>')
		{
			*line++ = '\0';
			*line++;
			*args = line;
			// argno++;
			break;
		}
		*line++;

	}
	while(*line != '\0')
	{
		if(*line == ' ')
		{
			*line++ = '\0';
			break;
		}
		*line++;
	}
	// *args = '\0';
	return argno;
}

int get_args_pipe(char *line, vector<char*> &args)
{

	int i = 0;
	int argno=1;
	
	args[i] = line;
	cout<<"HERE2"<<endl;
	while(*line != '\0')
	{
		while(*line != '\0' || *line != '|')
		{
			if (*line == ' ')
			{
				*line++ = '\0';
				args[i]++;
				args[i] = line;
				argno++;
			}
			if(*line == '<' || *line =='>')
			{
				*line++ = '\0';
				*line++;
				args[i] = line;
				// argno++;
				break;
			}
			*line++;

		}
		while(*line != '\0' || *line != '|')
		{
			if(*line == ' ')
			{
				*line++ = '\0';
				break;
			}
			*line++;
		}
		if(*line =='\0') break;
		*line++;
		i++;
	}
	
	// *args = '\0';
	return i+1;
}

void basic_run()
{
	string s;
	getline(cin,s);
	//cout<<s<<endl;

	char *args[MAX_ARGS];
	int nargs = 0;
	char *s_char = const_cast<char *>(s.c_str());
	nargs = get_args(s_char,args);
	args[nargs] = NULL;
	int status;
	bool isBackground = (strcmp(args[nargs-1],"&")==0);
	if(isBackground)
	{
		args[nargs-1] = NULL;
	}
	int x = fork();
	if(x==0)
	{
		execvp(args[0],args);
	}
	else
	{
		if(isBackground)
		{

		}
		else
		{
			waitpid(x,&status,0);
		}
	}

	return;
}

void redirect_run(bool isInput)
{
	string s;
	getline(cin,s);

	/*int x = 0;//fork();
	if(x==0)
	{*/
	char *args[MAX_ARGS];
	int nargs = 0;
	char *s_char = const_cast<char *>(s.c_str());
	nargs = get_args_redirect(s_char,args);
	char *filename = args[nargs-1];
	args[nargs-1] = NULL;
	nargs--;

	int x = fork();
	if(x==0)
	{
		if(isInput)
		{
			//close(fileno(stdin));
			//FILE * f = fopen(filename, "r");
			
			int f = open(filename, O_RDONLY);
			
			dup2(f,0);
			close(f);
		}
		else
		{
			//close(fileno(stdout));

			int f = open(filename,O_WRONLY | O_CREAT,0777);
			
			dup2(f,1);
			close(f);

		}

		execvp(args[0],args);
		cout<<"------------"<<endl;
	}
	//}
	return;
}

vector<string> break_pipe_args(string s)
{
	int len = s.length();
	vector<string> ans;
	int first = 0;
	for(int i=0;i<len;i++)
	{
		if(s[i] == '|')
		{
			ans.push_back(s.substr(first,i-first));
			first = i+1;
		}
	}
	ans.push_back(s.substr(first,len-first));
	return ans;
}

void pipe_run()
{
	
	string s;
	getline(cin,s);
	vector<string> args = break_pipe_args(s);
	int n = args.size();
	for(int i=0;i<n;i++)
	{
		char *arg[MAX_ARGS];
		char *s_char = const_cast<char *>(args[i].c_str());
		int nargs = get_args_redirect(s_char,arg);
		arg[nargs] = NULL;
		int x = fork();
		if(x==0)
		{
			//i/o
			//excevp()
		}


	}
	/*char *s_char = const_cast<char *>(s.c_str());
	char *dummy[MAX_ARGS];
	vector<char*> args(MAX_PIPES, new );

	int n = get_args_pipe(s_char, args);
	cout<<n<<endl;*/
	/*for(int i=0;i<n;i++)
	{
		int j = 0;
		char * temp = args[i];
		while(temp[j] !=NULL)
		{
			cout<<temp[j]<<' ';
		}
		cout<<endl;
	}*/
}

int main()
{
	while(1)
	{
		char option;
		cin>>option;
		getchar();
		switch(option)
		{
			case 'G':
				return 0;
			case 'A':
				basic_run();
				break;
			case 'B':
				basic_run();
				break;
			case 'C':
				redirect_run(true);
				break;
			case 'D':
				redirect_run(false);
				break;
			case 'E':
				basic_run();
				break;
			case 'F':
				pipe_run();
				break;
			default:
				cout<<"Invalid option"<<endl;
		}
		

	}

}