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
	/*
	int cdind = 0;
	bool cdcheck = false;
	for (int i=0;i<nargs;i++)
	{
		if (strcmp(args, "cd") == 0)
		{
			cdind = i;
			cdcheck = true;
		} 
	}
	if (cdcheck == true)
	{
		chdir(args[cdind+1]);
	}
	*/
	string s;
	getline(cin,s);
	//cout<<s<<endl;

	char *args[MAX_ARGS];
	int nargs = 0;
	char *s_char = const_cast<char *>(s.c_str());
	nargs = get_args(s_char,args);
	args[nargs] = NULL;

	if (strcmp(args[0], "cd") == 0 || strcmp(args[0], "chdir") == 0)
	{
		chdir(args[1]);
	} 

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

string trim(string s)
{
	int len = s.length();
	int i = 0;
	int first = 0;
	int last = len-1;
	while(i<len && s[i]==' ')
	{
		i++;
	}
	first = i;
	i = len-1;
	while(i>=0 && s[i] ==' ')
	{
		i--;
	}
	last = i;
	return s.substr(first, last-first+1);

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
			ans.push_back(trim(s.substr(first,i-first)));
			first = i+1;
		}
	}
	ans.push_back(trim(s.substr(first,len-first)));
	return ans;
}

void pipe_run()
{
	
	string s;
	getline(cin,s);
	vector<string> args = break_pipe_args(s);
	int n = args.size();
	int pipes[2*(n-1)];
	cout<<n<<endl;
	for(int i=0;i<n-1;i++)
	{
		pipe(pipes+2*i);
	}
	for(int i=0;i<n;i++)
	{
		char *arg[MAX_ARGS];
		char *s_char = const_cast<char *>(args[i].c_str());
		int nargs = get_args_redirect(s_char,arg);
		arg[nargs] = NULL;
		/*cout<<nargs<<endl;
		for(int j=0;j<nargs;j++)
		{
			cout<<arg[j]<<' ';
		}*/
		int status;
		int x = fork();
		if(x==0)
		{
			//i/o
			if(i!=0)
			{
				dup2(pipes[2*(i-1)],0);
				close(pipes[2*i-1]);
			}
			if(i!=n-1)
			{
				dup2(pipes[2*i+1],1);
				if(i!=0)
					close(pipes[2*(i-1)]);
			}


			/*for(int j=0;j<n-1;j++)
			{
				close(pipes[j]);
			}*/
			execvp(arg[0],arg);
		}
		


	}
	
}

int main()
{
	while(1)
	{
		cout<< "----------------\nA.Run an internal command \nB.Run an external command  \nC.Run an external command by redirecting standard input from a file \nD.Run an external command by redirecting standard output to a file \nE.Run an external command in the background F.Run several external commands in the pipe mode \nG.Quit the shell "<< endl;
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