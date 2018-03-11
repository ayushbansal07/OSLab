#include "asgn4.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#define MBtoB 1048576

using namespace std;



char* myfs_mem;
int cur_dir;

int create_myfs(int size)
{
	if(size > MAX_ALLOCATED_SIZE) return -1;
	if(size*1024*1024 < SUPER_BLOCK_BYTES + INODES_LIST_BYTES) return -1;
	myfs = new char[size*MBtoB];
	SuperBlock* sb = (SuperBlock *) myfs;


	if(memory == NULL)
		return -1;
	return sizeof(MRFS);
}

int main()
{
	int x;
	x = create_myfs(10);
	if(x==-1) cout<<"Error"<<endl;
	else cout<<x<<endl;

}