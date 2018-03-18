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
	return 0;
	
}