#include "asgn4.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string.h>
#include <cmath>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>


int main()
{
	int x;
	x = create_myfs(10);
	cout<<"HERE"<<endl;
	if(x==-1) cout<<"Error"<<endl;
	else cout<<x<<endl;

	x = mkdir_myfs("mydocs");
	x = mkdir_myfs("mycode");
	cout<<"\n+++++++++++ROOT DIR"<<endl;
	ls_myfs();
	x = chdir_myfs("mydocs");
	x = mkdir_myfs("mytext");
	x = mkdir_myfs("mypapers");
	cout<<"\n+++++++++++MYDOCS DIR"<<endl;
	ls_myfs();
	x = chdir_myfs("..");
	cout<<"\n+++++++++++ROOT DIR"<<endl;
	ls_myfs();

	/*int key = 1234;
	int shmid = shmget(key,sizeof(myfs),IPC_CREAT | 0666);
	char ** myptr = (char **)shmat(shmid,NULL,0);
	if(myptr == (char **)-1)
	{
		cout<<"shmat Error in parent"<<endl;
		exit(-1);
	}
	myptr = &myfs;*/
	int pid1 = fork();
	int status1,status2;
	if(pid1 == 0)
	{
		//P1
		//myfs = *myptr;
		chdir_myfs("mydocs");
		chdir_myfs("mytext");
		int fd = open_myfs("myfile1.txt",'w');
		char c = 'A';
		for(int i=0;i<26;i++)
		{
			write_myfs(fd,sizeof(c),(char *)&c);
			c++;
		}
		close_myfs(fd);
		exit(0);
	}
	else
	{
		int pid2 = fork();
		if(pid2 == 0)
		{
			//P2
			//myfs = *myptr;
			chdir_myfs("mycode");
			copy_pc2myfs("files/file1.txt", "myfile2.txt");
			exit(0);
		}
		else
		{
			//Parent
			waitpid(pid1,&status1,0);
			waitpid(pid2,&status2,0);
			cout<<"\n+++++++++++ROOT DIR"<<endl;
			ls_myfs();
			chdir_myfs("mydocs");
			chdir_myfs("mytext");
			cout<<"\n+++++++++++MYTEXT DIR"<<endl;
			ls_myfs();
			chdir_myfs("..");
			chdir_myfs("..");
			chdir_myfs("mycode");
			cout<<"\n+++++++++++MYCODE DIR"<<endl;
			ls_myfs();
		}
		
	}

	free_myfs();
	return 0;
	
}