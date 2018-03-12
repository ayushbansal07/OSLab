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
	//Initialise Super Block
	SuperBlock* sb = (SuperBlock *) myfs;
	sb->total_sz = size * MBtoB;
	sb->max_inodes = 	MAX_INODES;
	sb->actual_inodes = 1; //root by default
	sb->max_diskBlocks = MAX_NO_OF_DISKBLOCKS;
	sb->actual_diskBlocks = SUPERBLOCK_BLOCKS + INODE_LIST_BLOCKS + 1;
	for(int i=0;i<MAX_INODES;i++)
		sb->free_inodes[i] = 0;
	sb->free_inodes[0] = 1;
	int i;
	for(i=0;i<SUPERBLOCK_BLOCKS + INODE_LIST_BLOCKS + 1;i++)
		sb->free_diskBlocks[i] = 1;
	//Initialise Inode List
	InodeList* inl = (InodeList *)(myfs + SUPERBLOCK_BYTES);
	inl->node[0].filetype = 1;
	inl->node[0].filesize = 0;
	inl->node[0].last_modified = 	time(NULL);
	inl->node[0].last_read = time(NULL);
	inl->node[0].access_permission[0] = 6;
	inl->node[0].access_permission[1] = 6;
	inl->node[0].access_permission[2] = 6;
	inl->node[0].direct_pointers[0] = sb->actual_diskBlocks -1;
	for(int i=1;i<8;i++)
		inl->node[0].direct_pointers[i] = -1;
	inl->node[0].indirect_pointer = -1;
	inl->node[0].doubly_indirect_pointer = -1;
	//Initialise root directory
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

int* get_inode_ptr(Inode * inode, int idx)
{
	if(idx < NUM_DIRECT_POINTERS)
		return (inode->direct_pointers + idx);
	idx -= NUM_DIRECT_POINTERS;
	if(idx < BLOCKS_INDIRECT_PTR)
	{
		int * ptr = (int *)(myfs + DISKBLOCK_SIZE*inode->indirect_pointer + idx*4);
		return ptr;
	}
	idx -= BLOCKS_INDIRECT_PTR;
	int ptr_idx = idx/BLOCKS_INDIRECT_PTR;
	int* iptr = (int*)(myfs + DISKBLOCK_SIZE*inode->indirect_pointer + ptr_idx*4);
	int* ptr = (int*)(myfs + DISKBLOCK_SIZE*(*iptr) + (idx%BLOCKS_INDIRECT_PTR)*4);
	return ptr;
}

int get_dataBlock()
{
	SuperBlock * sb = (SuperBlock *)myfs;
	for(int i=0;i<MAX_NO_OF_DISKBLOCKS;i++)
	{
		if(sb->free_diskBlocks[i]==0)
		{
			sb->actual_diskBlocks += 1;
			sb->free_diskBlocks[i] = 1;
			return i;
		}
	}
	return -1;
}

int get_dir_block(Directory * dir)
{
	for(int i=0;i<FILES_PER_DIR;i++)
	{
		if(dir->entry[i].inode_no==-1)
			return i;
	}
	return -1;
}

void enter_in_dir(Inode * dir, char * filename, int fileptr)
{
	int idx = dir->filesize / FILES_PER_DIR;
	//TOSEE
	int *ptr = get_inode_ptr(dir, idx);
	if(*ptr == -1)
	{
		*ptr = get_dataBlock();
		Directory * mydir = (Directory *)(myfs + DISKBLOCK_SIZE*(*ptr));
		init_dir(mydir);
	}
	Directory * mydir = (Directory *)(myfs + DISKBLOCK_SIZE*(*ptr));
	int id = get_dir_block(mydir);
	strcpy(mydir->entry[id].filename, filename);
	mydir->entry[id].inode_no = fileptr;
	dir->filesize ++;
	dir->last_modified = time(NULL);
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
	int available_blocks = sb->max_diskBlocks - sb->actual_diskBlocks;
	if(no_of_blocks > available_blocks)
	{
		cout<<"Disk Space not SUfficient"<<endl;
		return -1;
	}
	int inode_idx = get_inode();
	if(inode_idx == -1)
	{
		cout<<"Error creating file: Unable to get new inode"<<endl;
		return -1;
	}
	InodeList * inl = (InodeList *)(myfs + SUPERBLOCK_BLOCKS);
	inl->node[inode_idx].filetype = 0;
	inl->node[inode_idx].filesize = filesize;
	inl->node[inode_idx].last_modified = time(NULL);
	inl->node[inode_idx].last_read = time(NULL);
	inl->node[inode_idx].access_permission[0] = 6;
	inl->node[inode_idx].access_permission[1] = 6;
	inl->node[inode_idx].access_permission[2] = 4;

	int no_of_blocks_req = no_of_blocks;

	no_of_blocks -= NUM_DIRECT_POINTERS;
	if(no_of_blocks > 0)
	{
		inl->node[inode_idx].indirect_pointer = get_dataBlock();
		no_of_blocks -= BLOCKS_INDIRECT_PTR;
	}
	if(no_of_blocks > 0)
	{
		inl->node[inode_idx].doubly_indirect_pointer = get_dataBlock();
		int *iptr = (int*)(myfs+DISKBLOCK_SIZE*inl->node[inode_idx].doubly_indirect_pointer);
		int j = 0;
		while(no_of_blocks > 0 && j<BLOCKS_INDIRECT_PTR)
		{
			*iptr = get_dataBlock();
			iptr++;
			j++;
			no_of_blocks -= BLOCKS_INDIRECT_PTR;
		}
		if(no_of_blocks > 0)
		{
			cout<<"File size too big"<<endl;
			return -1;
		}
	}

	int idx = 0;
	while(idx < no_of_blocks_req && idx < NUM_DIRECT_POINTERS)
	{
		if(file.eof()) break;
		int db = get_dataBlock();
		if(db == -1)
		{
			cout<<"Error getting free data block"<<endl;
			return -1;
		}

		file.read(myfs + DISKBLOCK_SIZE*db, DISKBLOCK_SIZE);
		inl->node[inode_idx].direct_pointers[idx] =  db;
		idx++;
	}
	idx -= NUM_DIRECT_POINTERS;
	no_of_blocks_req -= NUM_DIRECT_POINTERS;

	int *iptr = (int*)(myfs+DISKBLOCK_SIZE*inl->node[inode_idx].indirect_pointer);
	while(idx < no_of_blocks_req && idx < BLOCKS_INDIRECT_PTR)
	{
		if(file.eof()) break;
		int db = get_dataBlock();
		if(db==-1)
		{
			cout<<"Error getting free data block"<<endl;
			return -1;
		}
		file.read(myfs + DISKBLOCK_SIZE*db, DISKBLOCK_SIZE);
		*iptr = db;
		idx++;
		iptr++;
	}
	idx -= BLOCKS_INDIRECT_PTR;
	no_of_blocks_req -= BLOCKS_INDIRECT_PTR;


	int * diptr = (int*)(myfs+DISKBLOCK_SIZE*inl->node[inode_idx].doubly_indirect_pointer);
	while(idx < no_of_blocks_req && idx < BLOCKS_DI_PTR)
	{
		int location_of_iptr = *diptr;
		iptr = (int*)(myfs+DISKBLOCK_SIZE*location_of_iptr);
		while(idx < no_of_blocks_req && idx < BLOCKS_INDIRECT_PTR)
		{
			if(file.eof()) break;
			int db = get_dataBlock();
			if(db == -1)
			{
				cout<<"Error getting free data block"<<endl;
				return -1;
			}
			file.read(myfs+ DISKBLOCK_SIZE*db, DISKBLOCK_SIZE);
			*iptr = db;
			idx++;
			iptr++;
		}
		diptr++;
	} 

	Inode * curr_dir_inode = &((InodeList *)(myfs + SUPERBLOCK_BYTES))->node[cur_dir];
	enter_in_dir(curr_dir_inode, dest, inode_idx);
	file.close();
	return 0;
}

int main()
{
	int x;
	x = create_myfs(10);
	if(x==-1) cout<<"Error"<<endl;
	else cout<<x<<endl;

	x = copy_pc2myfs("ayus.txt","myfile_new");
	cout<<x<<endl;

}