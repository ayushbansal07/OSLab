#include "asgn4.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string.h>
#include <vector>

using namespace std;

int main()
{
	int x;
	x = create_myfs(10);
	if(x==-1) cout<<"Error"<<endl;
	else cout<<x<<endl;

	x = restore_myfs("mydump-33.backup");
	ls_myfs();

	char buff[1024*1024];
	int fd = open_myfs("mytest.txt", 'r');
	vector<int> v(100);
	int z;
	for(int i=0;i<100;i++)
	{
		read_myfs(fd,sizeof(int),(char*)&z);
		cout<<z<<"\t";
	}
	cout<<endl;

	free_myfs();
	return 0;


}