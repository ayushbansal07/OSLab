#include "asgn4.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string.h>

#define MBtoB 1048576

using namespace std;



char* myfs;
int cur_dir;

void init_dir(Directory* dir)
{
	for (int i=0;i<FILES_PER_DIR;i++){
		strcpy(dir->entry[i].filename, "");
		dir->entry[i].inode_no = -1;
	}
}

int create_myfs(int size)
{
	if(size > MAX_ALLOCATED_SIZE) return -1;
	if(size*MBtoB < SUPERBLOCK_SIZE + INODE_LIST_BYTES) return -1;
	myfs = new char[size*MBtoB];
	SuperBlock* sb = (SuperBlock *) myfs;

	sb->total_sz = size * MBtoB;
	sb->max_inodes = 	MAX_INODES;
	sb->actual_inodes = 1; //root by default
	sb->max_diskBlocks = MAX_NO_OF_DISKBLOCKS;
	sb->actual_diskBlocks = SUPERBLOCK_BLOCKS + INODE_LIST_BLOCKS + 1;
	for(int i=0;i<INODE_LIST_BYTES;i++)
		sb->free_inodes[i] = 0;
	sb->free_inodes[0] = 1;
	int i = 0;
	for(i=0;i<( SUPERBLOCK_BLOCKS + INODE_LIST_BLOCKS + 1)/8;i++)
		sb->free_diskBlocks[i] = 1;
	
	InodeList* inl = (InodeList *)(myfs + SUPERBLOCK_BYTES);
	inl->node[0].filetype = 1;
	inl->node[0].filesize = 0;
	inl->node[0].last_modified = 	time(NULL);
	inl->node[0].last_read = time(NULL);
	strcpy(inl->node[0].access_permission , "666");
	inl->node[0].direct_pointers[0] = sb->actual_diskBlocks -1;
	for(int i=1;i<8;i++)
		inl->node[0].direct_pointers[i] = -1;
	inl->node[0].indirect_pointer = -1;
	inl->node[0].doubly_indirect_pointer = -1;

	Directory* root_block = (Directory *)(myfs +SUPERBLOCK_BYTES + INODE_LIST_BYTES);
	init_dir(root_block);

	return size;
}


int get_inode()
{
	SuperBlock * sb = (SuperBlock *)myfs;
	for(int i=0;i<MAX_INODES;i++)
	{
		if(sb->free_inodes[i] == 0)
		{
			sb->free_inodes[i] = 1;
			sb->actual_inodes += 1;
			return i;
		}
	}
	return -1;
}

int copy_pc2myfs(char* source, char* dest)
{
	SuperBlock * sb = (SuperBlock *)myfs;
	ifstream file;
	file.open(source);
	if(!file.is_open())
	{	
		cout<<"Error opening File "<<source<<endl;
		return -1;
	}
	file.seekg(0, ios::end);
	int filesize = file.tellg();
	file.seekg(0,ios::beg);
	int no_of_blocks = filesize/DISKBLOCK_SIZE + 1;
	int available_blocks = sb->actual_inodes - sb->actual_diskBlocks;
	if(no_of_blocks > available_blocks)
	{
		cout<<"Disk Space not SUfficient"<<endl;
		return -1;
	}
	int inode_idx = get_inode();
	if(inode_idx == -1)
	{
		cout<<"Error creeating file: Unable to get new inode"<<endl;
		return -1;
	}
	InodeList * inl = (InodeList *)(myfs + SUPERBLOCK_BLOCKS);
	inl->node->filetype = 0;
	inl->node->filesize = filesize;
	inl->node->last_modified = time(NULL);
	inl->node->last_read = time(NULL);
	inl->node->access_permission[0] = 6;
	inl->node->access_permission[1] = 6;
	inl->node->access_permission[2] = 4;





}

int main()
{
	int x;
	x = create_myfs(10);
	if(x==-1) cout<<"Error"<<endl;
	else cout<<x<<endl;

}