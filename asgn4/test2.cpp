#include "asgn4.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string.h>
#include <cmath>


int main()
{
	int x;
	x = create_myfs(10);
	if(x==-1) cout<<"Error"<<endl;
	else cout<<x<<endl;

	int fd = open_myfs("mytest.txt", 'w');
	char buff[1024*1024];
	srand (1);
	int k=0;
	for (int i=0;i<100;i++)
	{
		x = rand() % 100 + 1;
  		snprintf(buff+k, sizeof(buff), "%d", x);	
  		k = k + (int)log(x)+1;
		*(buff + k) = '\n';
		k++;
		// cout<<buff[i]<<endl;
	}

	x = write_myfs(fd, 100, buff);
	x = close_myfs(fd);
	ls_myfs();

	fd = open_myfs("mytest.txt", 'r');
	x = read_myfs(fd, 100, buff);


	cout<< " Enter N:"<<endl;
	int n;
	cin>>n;
	for (int i=0;i<n;i++)
	{
		//c
		char filename[30];
		char fileno[4];
		snprintf(fileno, sizeof(fileno), "%d", i+1);
		strcpy(filename,"mytest-");
		strcat(filename,fileno);
		strcat(filename,".txt");
		
		int fdc = open_myfs(filename, 'w');
		x = write_myfs(fdc, 100, buff);
		x = close_myfs(fdc);
		// cout<<buff[i];
	}
	x = close_myfs(fd);

	ls_myfs();
	
	x = dump_myfs("mydump-33.backup");
	return 0;
	
}