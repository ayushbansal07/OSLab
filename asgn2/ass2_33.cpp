#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_ARGS 100

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
				break;
			default:
				cout<<"Invalid option"<<endl;
		}
		

	}

}