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
FDTable fd_table;

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
	for(int i=1;i<MAX_INODES;i++)
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
	int* iptr = (int*)(myfs + DISKBLOCK_SIZE*(inode->doubly_indirect_pointer) + ptr_idx*4);
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
	//InodeList * inl = (InodeList *)(myfs + SUPERBLOCK_BLOCKS);
	Inode* file_inode = &((InodeList*)(myfs+SUPERBLOCK_BYTES))->node[inode_idx];
	//waste_inode->filesize = filesize;
	file_inode->filetype = 0;
	file_inode->filesize = filesize;
	file_inode->last_modified = time(NULL);
	file_inode->last_read = time(NULL);
	file_inode->access_permission[0] = 6;
	file_inode->access_permission[1] = 6;
	file_inode->access_permission[2] = 4;

	int no_of_blocks_req = no_of_blocks;

	no_of_blocks -= NUM_DIRECT_POINTERS;
	if(no_of_blocks > 0)
	{
		file_inode->indirect_pointer = get_dataBlock();
		no_of_blocks -= BLOCKS_INDIRECT_PTR;
	}
	if(no_of_blocks > 0)
	{
		file_inode->doubly_indirect_pointer = get_dataBlock();
		int *iptr = (int*)(myfs+DISKBLOCK_SIZE*(file_inode->doubly_indirect_pointer));
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
		file_inode->direct_pointers[idx] =  db;
		idx++;
	}
	idx -= NUM_DIRECT_POINTERS;
	no_of_blocks_req -= NUM_DIRECT_POINTERS;

	int *iptr = (int*)(myfs+DISKBLOCK_SIZE*(file_inode->indirect_pointer));
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


	int * diptr = (int*)(myfs+DISKBLOCK_SIZE*(file_inode->doubly_indirect_pointer));
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
		idx -= BLOCKS_INDIRECT_PTR;
		no_of_blocks_req -= BLOCKS_INDIRECT_PTR;
		diptr++;
	} 

	Inode * curr_dir_inode = &((InodeList *)(myfs + SUPERBLOCK_BYTES))->node[cur_dir];
	enter_in_dir(curr_dir_inode, dest, inode_idx);
	file.close();
	return 0;
}

Inode * get_file_inode(Inode * dir_inode, char * filename)
{
	//Only for direct pointers
	int ct = 0;
	for(int i=0;i<NUM_DIRECT_POINTERS;i++)
	{
		if(ct >= dir_inode->filesize) break;
		int directory_location = dir_inode->direct_pointers[i];
		Directory * block = (Directory *) (myfs + DISKBLOCK_SIZE*directory_location);
		for(int j=0;j<FILES_PER_DIR;j++)
		{
			if(ct >= dir_inode->filesize) break;
			if(block->entry[j].inode_no != -1 && !strcmp(block->entry[j].filename,filename))
			{
				InodeList * list = (InodeList *)(myfs + SUPERBLOCK_BYTES);
				return &(list->node[block->entry[j].inode_no]);
			}
			if(block->entry[j].inode_no != -1)
				ct++;
		}
	}
	return NULL;
}


int copy_myfs2pc(char* source, char* dest)
{
	ofstream file;
	file.open(dest);
	Inode * curr_dir_inode = &((InodeList *)(myfs + SUPERBLOCK_BYTES))->node[cur_dir];
	Inode * file_inode = get_file_inode(curr_dir_inode, source);
	if(file_inode == NULL){
		cout<<"File Not Found"<<endl;
		return -1;
	}
	int fsz = file_inode->filesize;
	int idx = 0;
	while(fsz > 0)
	{
		Block * b = (Block *)(myfs + DISKBLOCK_SIZE*(*get_inode_ptr(file_inode,idx)));
		int sz_to_write = min(fsz, (int)sizeof(Block));
		file.write((char *)b, sz_to_write);
		fsz -= sz_to_write;
		idx++;
	}
	file.close();
	return 0;
}

void rm_diskBlock(int idx)
{
	//cout<<idx<<endl;
	if(idx < SUPERBLOCK_BLOCKS + INODE_LIST_BLOCKS)
	{
		//cout<<"Error Trying to remove Non-data space"<<endl;
		return;
	}
	SuperBlock * sb = (SuperBlock *)(myfs);
	sb->actual_diskBlocks -= 1;
	sb->free_diskBlocks[idx] = 0;
}

int rm_myfs(char * filename)
{
	Inode * curr_dir_inode = &((InodeList *)(myfs + SUPERBLOCK_BYTES))->node[cur_dir];
	Inode * file_inode = get_file_inode(curr_dir_inode, filename);
	if(file_inode == NULL){
		cout<<"File Not Found"<<endl;
		return -1;
	}
	int fsz = file_inode->filesize;
	int idx = 0;
	while(fsz > 0 && idx < NUM_DIRECT_POINTERS + BLOCKS_INDIRECT_PTR)
	{
		int block = *get_inode_ptr(file_inode,idx);			
		rm_diskBlock(block);
		fsz -= DISKBLOCK_SIZE;
		idx++;
	}
	if(file_inode->indirect_pointer != -1)
	{
		rm_diskBlock(file_inode->indirect_pointer);
	}
	while(fsz > 0 && idx < NUM_DIRECT_POINTERS + BLOCKS_INDIRECT_PTR + BLOCKS_DI_PTR)
	{
		int block = *get_inode_ptr(file_inode,idx);
		rm_diskBlock(block);
		fsz -= DISKBLOCK_SIZE;
		idx++;
	}
	if(file_inode->doubly_indirect_pointer != -1)
	{
		int * diptr = (int *)(myfs + DISKBLOCK_SIZE*(file_inode->doubly_indirect_pointer));
		int num_ptrs = 0;
		while(num_ptrs < BLOCKS_INDIRECT_PTR)
		{
			if(*diptr > 0)
			{
				rm_diskBlock(*diptr);
			}
			else
			{
				break;
			}
			diptr++;
		}		
		rm_diskBlock(file_inode->doubly_indirect_pointer);
	}

	SuperBlock * sb = (SuperBlock *)myfs;
	int inode_idx = file_inode - (Inode *)(myfs + SUPERBLOCK_BYTES);
	sb->free_inodes[inode_idx] = 0;
	sb->actual_inodes -= 1;

	//TODO:Remove directory from entry
	int ct = 0;
	bool f = false;
	for(int i=0;i<NUM_DIRECT_POINTERS && !f;i++)
	{
		if(ct >= curr_dir_inode->filesize) break;
		int directory_location = curr_dir_inode->direct_pointers[i];
		Directory * block = (Directory *) (myfs + DISKBLOCK_SIZE*directory_location);
		for(int j=0;j<FILES_PER_DIR;j++)
		{
			if(ct >= curr_dir_inode->filesize) break;
			if(block->entry[j].inode_no != -1 && !strcmp(block->entry[j].filename,filename))
			{
				block->entry[j].inode_no = -1;
				f = true;
				break;
			}
			if(block->entry[j].inode_no != -1)
				ct++;
		}
	}
	curr_dir_inode->filesize -= 1;
	return 0;
}

int showfile_myfs(char *filename)
{
	Inode * curr_dir_inode = &((InodeList *)(myfs + SUPERBLOCK_BYTES))->node[cur_dir];
	Inode * file_inode = get_file_inode(curr_dir_inode, filename);
	if(file_inode == NULL){
		cout<<"File Not Found"<<endl;
		return -1;
	}
	int fsz = file_inode->filesize;
	int idx = 0;
	while(fsz > 0)
	{
		int block = *get_inode_ptr(file_inode,idx);	
		Block * b = (Block *) (myfs + DISKBLOCK_SIZE*block);
		cout<<(char *)b;
		fsz -= DISKBLOCK_SIZE;
		idx++;
	}
	return 0;
}

int ls_myfs()
{
	Inode * dir_inode = &((InodeList *)(myfs + SUPERBLOCK_BYTES))->node[cur_dir];
	int ct = 0;
	for(int i=0;i<NUM_DIRECT_POINTERS;i++)
	{
		if(ct >= dir_inode->filesize) break;
		int directory_location = dir_inode->direct_pointers[i];
		Directory * block = (Directory *) (myfs + DISKBLOCK_SIZE*directory_location);
		for(int j=0;j<FILES_PER_DIR;j++)
		{
			if(ct >= dir_inode->filesize) break;
			if(block->entry[j].inode_no != -1)
			{
				cout<<block->entry[j].filename<<endl;
				ct++;
			}
		}
	}
	return 0;
}

int mkdir_myfs(char * dirname)
{
	int inode_idx = get_inode();
	if(inode_idx == -1)
	{
		cout<<"Error creating file: Unable to get new inode"<<endl;
		return -1;
	}
	//InodeList * inl = (InodeList *)(myfs + SUPERBLOCK_BLOCKS);
	Inode* file_inode = &((InodeList*)(myfs+SUPERBLOCK_BYTES))->node[inode_idx];
	//waste_inode->filesize = filesize;
	file_inode->filetype = 1;
	file_inode->filesize = 0;
	file_inode->last_modified = time(NULL);
	file_inode->last_read = time(NULL);
	file_inode->access_permission[0] = 6;
	file_inode->access_permission[1] = 6;
	file_inode->access_permission[2] = 6;
	for(int i=0;i<8;i++)
		file_inode->direct_pointers[i] = -1;
	file_inode->indirect_pointer = -1;
	file_inode->doubly_indirect_pointer = -1;

	Inode * curr_dir_inode = &((InodeList *)(myfs + SUPERBLOCK_BYTES))->node[cur_dir];
	enter_in_dir(curr_dir_inode, dirname, inode_idx);

	enter_in_dir(file_inode,"..",cur_dir);

	return 0;
}

int chdir_myfs(char* dirname)
{
	Inode * curr_dir_inode = &((InodeList *)(myfs + SUPERBLOCK_BYTES))->node[cur_dir];
	Inode * new_dir_inode = get_file_inode(curr_dir_inode, dirname);
	if(new_dir_inode == NULL){
		cout<<"Direcetory Not Found"<<endl;
		return -1;
	}	
	if(new_dir_inode->filetype != 1)
	{
		cout<<"Not a directory name"<<endl;
		return -1;
	}
	int inode_idx = new_dir_inode - (Inode *)(myfs + SUPERBLOCK_BYTES);
	cur_dir = inode_idx;
	return 0;
}

int rmdir_myfs(char *dirname)
{
	Inode * curr_dir_inode = &((InodeList *)(myfs + SUPERBLOCK_BYTES))->node[cur_dir];
	Inode * my_dir_inode = get_file_inode(curr_dir_inode, dirname);
	if(my_dir_inode == NULL)
	{
		cout<<"Direcetory Not Found"<<endl;
		return -1;
	}
	if(my_dir_inode->filetype != 1){
		cout<<"Not a directory name"<<endl;
		return -1;
	}
	//change to mydir
	int prev_dir = cur_dir;
	int inode_idx = my_dir_inode - (Inode *)(myfs + SUPERBLOCK_BYTES);
	cur_dir = inode_idx;
	for(int i=0;i<NUM_DIRECT_POINTERS;i++)
	{
		if(my_dir_inode->filesize == 1) break;
		int directory_location = my_dir_inode->direct_pointers[i];
		Directory * block = (Directory *) (myfs + DISKBLOCK_SIZE*directory_location);
		for(int j=0;j<FILES_PER_DIR;j++)
		{
			if(cur_dir == 0){
				if(my_dir_inode->filesize ==0) break;
			}
			else{
				if(my_dir_inode->filesize == 1) break;
			}
			
			if(block->entry[j].inode_no != -1)
			{
				//getinode, name
				char * filename = block->entry[j].filename;
				if(strcmp("..",filename) == 0) continue;
				Inode * file_inode = get_file_inode(my_dir_inode, filename);
				if(file_inode->filetype == 1)
				{
					cout<<"Removing Directory "<<filename<<endl;
					rmdir_myfs(filename);
				}
				else
				{
					rm_myfs(filename);
				}
				
			}
		}
	}
	//change back to olddir
	cur_dir = prev_dir;
	for(int i=0;i<NUM_DIRECT_POINTERS;i++)
	{
		if(my_dir_inode->direct_pointers[i] != -1)
		{
			rm_diskBlock(my_dir_inode->direct_pointers[i]);
		}
	}

	SuperBlock *sb = (SuperBlock *)(myfs);
	sb->free_inodes[inode_idx] = 0;
	sb->actual_inodes -= 1;

	//Remove Entry from directory
	int ct = 0;
	bool f = false;
	for(int i=0;i<NUM_DIRECT_POINTERS && !f;i++)
	{
		if(ct >= curr_dir_inode->filesize) break;
		int directory_location = curr_dir_inode->direct_pointers[i];
		Directory * block = (Directory *) (myfs + DISKBLOCK_SIZE*directory_location);
		for(int j=0;j<FILES_PER_DIR;j++)
		{
			if(ct >= curr_dir_inode->filesize) break;
			if(block->entry[j].inode_no != -1 && !strcmp(block->entry[j].filename,dirname))
			{
				block->entry[j].inode_no = -1;
				f = true;
				break;
			}
			if(block->entry[j].inode_no != -1)
				ct++;
		}
	}
	curr_dir_inode->filesize -= 1;

	return 0;

}

int open_myfs(char *filename, char mode)
{
	int idx = 0;
	for(;idx < MAX_FDTABLE_SIZE;idx++)
	{
		if(fd_table.entry[idx].inode == NULL) break;
	}
	if(idx == MAX_FDTABLE_SIZE){
		cout<<"Error: FDTable full"<<endl;
		return -1;
	}
	Inode * curr_dir_inode = &((InodeList *)(myfs + SUPERBLOCK_BYTES))->node[cur_dir];
	Inode * file_inode = get_file_inode(curr_dir_inode, filename);
	if(file_inode == NULL)
	{
		if(mode == 'r'){
			cout<<"FIle does not Exist, unable to open"<<endl;
			return -1;
		}	
		else if(mode == 'w')
		{
			int inode_idx = get_inode();
			file_inode = &(((InodeList *)(myfs + SUPERBLOCK_BYTES))->node[inode_idx]);
			file_inode->filetype = 0;
			file_inode->filesize = 0;
			file_inode->last_modified = time(NULL);
			file_inode->last_read = time(NULL);
			file_inode->access_permission[0] = 6;
			file_inode->access_permission[1] = 6;
			file_inode->access_permission[2] = 6;

			enter_in_dir(curr_dir_inode,filename,inode_idx);
		}

		else
		{
			cout<<"Invalid mode"<<endl;
			return -1;
		}
	}

	fd_table.entry[idx].offset = 0;
	fd_table.entry[idx].inode = file_inode;
	fd_table.entry[idx].mode = mode;

	return idx;
}

int close_myfs(int fd)
{
	if(fd_table.entry[fd].inode == NULL) return -1;
	fd_table.entry[fd].inode = NULL;
	return 0;
}


//TODO: read and write
int read_myfs(int fd, int nbytes, char *buff)
{
	if(fd_table.entry[fd].mode == 'w')
	{
		cout<<"Error file opened in Read Mode"<<endl;
		return -1;
	}
	int * loc = &fd_table.entry[fd].offset;
	int idx = *loc/DISKBLOCK_SIZE;
	int init_offset = *loc%DISKBLOCK_SIZE;
	int read = 0;
	int fsz = fd_table.entry[fd].inode->filesize - *loc;
	if(init_offset)
	{
		int to_copy = min(min(nbytes,fsz),DISKBLOCK_SIZE - init_offset);
		memcpy(buff,myfs + DISKBLOCK_SIZE*(*get_inode_ptr(fd_table.entry[fd].inode,idx)) + init_offset, to_copy);
		read += to_copy;
		nbytes -= to_copy;
		fsz -= to_copy;
		idx++;
	}
	while(nbytes && fsz)
	{
		int to_copy = min(min(nbytes,fsz),DISKBLOCK_SIZE);
		memcpy(buff + read, myfs + DISKBLOCK_SIZE * (*get_inode_ptr(fd_table.entry[fd].inode, idx)), to_copy);
		read += to_copy;
		nbytes -= to_copy;
		fsz -= to_copy;
		idx++;
	}
	*loc += read;
	fd_table.entry[fd].inode->last_read = time(NULL);
	return read;
}

int write_myfs(int fd, int nbytes, char *buff)
{
	if(fd_table.entry[fd].mode == 'r')
	{
		cout<<"Error file opened in Write Mode"<<endl;
		return -1;
	}
	int * loc = &fd_table.entry[fd].offset;
	int idx = *loc/DISKBLOCK_SIZE;
	int init_offset = *loc%DISKBLOCK_SIZE;
	int written = 0;
	if(init_offset)
	{
		int to_write = min(nbytes, DISKBLOCK_SIZE - init_offset);
		memcpy(myfs + DISKBLOCK_SIZE * (*get_inode_ptr(fd_table.entry[fd].inode,idx)) + init_offset, buff, to_write);
		written += to_write;
		nbytes -= to_write;
		idx++;
	}
	while(nbytes)
	{
		int to_write = min(nbytes, DISKBLOCK_SIZE);
		if(idx == NUM_DIRECT_POINTERS)
			fd_table.entry[fd].inode->indirect_pointer = get_dataBlock();
		if(idx == NUM_DIRECT_POINTERS + BLOCKS_INDIRECT_PTR)
			fd_table.entry[fd].inode->doubly_indirect_pointer = get_dataBlock();
		if(idx >= NUM_DIRECT_POINTERS + BLOCKS_INDIRECT_PTR)
		{
			if(idx - (NUM_DIRECT_POINTERS + BLOCKS_INDIRECT_PTR)%BLOCKS_INDIRECT_PTR==0)
			{
				*(int*)(myfs + DISKBLOCK_SIZE*fd_table.entry[fd].inode->doubly_indirect_pointer + 4*(idx - (NUM_DIRECT_POINTERS + BLOCKS_INDIRECT_PTR))/BLOCKS_INDIRECT_PTR) = get_dataBlock();
			}
		}
		int *ind = get_inode_ptr(fd_table.entry[fd].inode,idx);
		*ind = get_dataBlock();
		memcpy(myfs + DISKBLOCK_SIZE*(*ind), buff + written,to_write);
		written += to_write;
		nbytes -= to_write;
		idx++;
	}
	*loc += written;
	fd_table.entry[fd].inode->filesize += written;
	fd_table.entry[fd].inode->last_read = time(NULL);
	fd_table.entry[fd].inode->last_modified = time(NULL);
	return written;
}

int eof_myfs(int fd)
{
	if(fd_table.entry[fd].offset == fd_table.entry[fd].inode->filesize) return true;
	return false;
}

int dump_myfs(char *dumpfile)
{
	ofstream file;	
	file.open(dumpfile);
	int sz = ((SuperBlock *)myfs)->total_sz;
	file.write(myfs,sz);
	file.close();
	return 0;
}

int restore_myfs(char *dumpfile)
{
	ifstream file;
	file.open(dumpfile);
	file.seekg(0, ios::end);
	int sz = file.tellg();
	file.seekg(0,ios::beg);
	file.read(myfs,sz);
	file.close();
	return sz;
}

int status_myfs()
{
	SuperBlock * sb = (SuperBlock *)myfs;
	cout<<"Size of FS = "<<sb->total_sz<<endl;
	int free_space = (sb->max_diskBlocks - sb->actual_diskBlocks)*DISKBLOCK_SIZE;
	cout<<"Free Space = "<<free_space<<endl;
	cout<<"Occupied Space = "<<(sb->total_sz - free_space)<<endl;
	cout<<"Number of files =  "<<sb->actual_inodes<<endl;
}

int chmod_myfs(char* name, int mode){
	Inode * curr_dir_inode = &((InodeList *)(myfs + SUPERBLOCK_BYTES))->node[cur_dir];
	Inode * file_indoe = get_file_inode(curr_dir_inode, name);
	if(file_indoe == NULL)
	{
		cout<<"File DOES NOT EXIST"<<endl;
		return -1;
	}
	file_inode->access_permission[0] = (mode>>6)%8;
	file_inode->access_permission[1] = (mode>>3)%8;
	file_inode->access_permission[2] = (mode)%8;

	return 0;
}



/*int main()
{
	int x;
	x = create_myfs(10);
	if(x==-1) cout<<"Error"<<endl;
	else cout<<x<<endl;



	cout<<"----"<<endl;
	ls_myfs();
	x = copy_pc2myfs("ayus.txt","myfile_new");
	cout<<"----"<<endl;
	ls_myfs();
	x = copy_pc2myfs("water.jpg","myfile_new2");
	cout<<"----"<<endl;
	ls_myfs();
	x = copy_pc2myfs("ayus.txt","myfile_new3");
	cout<<"----"<<endl;
	ls_myfs();
	x = copy_pc2myfs("ayus.txt","myfile_new4");
	cout<<"----"<<endl;
	ls_myfs();
	x = copy_pc2myfs("ayus.txt","myfile_new5");
	cout<<"----"<<endl;
	ls_myfs();
	mkdir_myfs("mydir_ayush");
	cout<<"-------"<<endl;
	ls_myfs();

	cout<<"---"<<endl;
	chdir_myfs("mydir_ayush");
	cout<<"------"<<endl;
	ls_myfs();
	copy_pc2myfs("ayus.txt","myfile_new7");
	cout<<"------"<<endl;
	ls_myfs();
	chdir_myfs("..");
	cout<<"------"<<endl;
	ls_myfs();
	rmdir_myfs("mydir_ayush");
	cout<<"-------"<<endl;
	ls_myfs();
	cout<<"__________________"<<endl;
	int fd = open_myfs("myfile_new3",'r');
	cout<<fd<<endl;
	
	int fd3 = open_myfs("myfile_new3",'r');
	cout<<fd3<<endl;
	int fd2 = open_myfs("nkfej",'w');
	cout<<fd2<<endl;
	cout<<"-------"<<endl;
	ls_myfs();
	int fd4 = open_myfs("myfile_new2",'r');
	cout<<fd4<<endl;
	cout<<close_myfs(fd4)<<endl;
	cout<<dump_myfs("mydump")<<endl;
	cout<<restore_myfs("mydump")<<endl;

	
}*/