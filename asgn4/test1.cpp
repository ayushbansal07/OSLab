#include "asgn4.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string.h>

using namespace std;

int main()
{
	int x;
	x = create_myfs(10);
	if(x==-1) cout<<"Error"<<endl;
	else cout<<x<<endl;



	cout<<"----"<<endl;
	ls_myfs();
	x = copy_pc2myfs("files/file1.txt","myfile1");
	cout<<"----"<<endl;
	ls_myfs();
	x = copy_pc2myfs("files/file2.txt","myfile2");
	cout<<"----"<<endl;
	ls_myfs();
	x = copy_pc2myfs("files/file3.txt","myfile3");
	cout<<"----"<<endl;
	ls_myfs();
	x = copy_pc2myfs("files/file4.txt","myfile4");
	cout<<"----"<<endl;
	ls_myfs();
	x = copy_pc2myfs("files/file5.txt","myfile5");
	cout<<"----"<<endl;
	ls_myfs();
	x = copy_pc2myfs("files/file6.txt","myfile6");
	cout<<"----"<<endl;
	ls_myfs();
	x = copy_pc2myfs("files/file7.txt","myfile7");
	cout<<"----"<<endl;
	ls_myfs();
	x = copy_pc2myfs("files/TCP_1","myTCP_1");
	cout<<"----"<<endl;
	ls_myfs();
	x = copy_pc2myfs("files/TCP_3","myTCP_3");
	cout<<"----"<<endl;
	ls_myfs();
	x = copy_pc2myfs("files/TCP_5","myTCP_5");
	cout<<"----"<<endl;
	ls_myfs();
	x = copy_pc2myfs("files/udp","myudp");
	cout<<"----"<<endl;
	ls_myfs();
	x = copy_pc2myfs("files/paper.pdf","mypaper.pdf");
	cout<<"----"<<endl;
	ls_myfs();

	cout<<"\nEnter a filename to delete:"<<endl;
	string s;
	
	char file[30];
	cin>>file;
	// strcpy(file,s);
	x= rm_myfs(file);
	if (x!=-1)
		cout<<"Delete Sucessful\n"<<endl;
	ls_myfs();

	free_myfs();
	
	return 0;
	
}