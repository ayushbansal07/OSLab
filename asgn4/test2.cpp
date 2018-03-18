#include "asgn4.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string.h>
#include <cmath>
#include <vector>

using namespace std;


int main()
{
	int x;
	x = create_myfs(10);
	if(x==-1) cout<<"Error"<<endl;
	else cout<<x<<endl;

	int fd = open_myfs("mytest.txt", 'w');
	srand (1);
	int k=0;
	for (int i=0;i<100;i++)
	{
		x = rand() % 100 + 1;
		write_myfs(fd,sizeof(x),(char *)&x);
	}

	//x = write_myfs(fd, 100, buff);
	x = close_myfs(fd);
	ls_myfs();

	vector<int> v(100);
	fd = open_myfs("mytest.txt", 'r');
	int z;
	for(int i=0;i<100;i++)
	{
		read_myfs(fd, sizeof(x), (char *)&z);
		v[i] = z;
	}
	close_myfs(fd);
	


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

		for(int i=0;i<100;i++)
		{
			z = v[i];
			write_myfs(fdc,sizeof(z),(char *)&z);
		}
		x = close_myfs(fdc);
	}

	ls_myfs();
	
	x = dump_myfs("mydump-33.backup");
	return 0;
	
}