#ifndef ASGN4_H
#define ASGN4_H

#include <bitset>
#include <ctime>

#define MAX_ALLOCATED_SIZE 128 //MB
#define DISKBLOCK_SIZE 256
#define MAX_INODES 256

using namespace std;

#define MAX_NO_OF_DISKBLOCKS 65536 //64K, max allocated 128MB memory

struct SuperBlock{
	int total_sz;
	int max_inodes;
	int actual_inodes;
	int max_diskBlocks;
	int actual_diskBlocks;
	bitset<MAX_NO_OF_DISKBLOCKS> free_diskBlocks;
	bitset<MAX_INODES> free_inodes;
};

const int NUM_DIRECT_POINTERS = 8;
const int BLOCKS_INDIRECT_PTR = 64;
const int BLOCKS_DI_PTR = 4096;

struct Inode{
	bool filetype;
	int filesize;
	time_t last_modified;
	time_t last_read;
	char access_permission[3];
	int direct_pointers[8];
	int indirect_pointer;
	int doubly_indirect_pointer;
};

struct InodeList{
	Inode node[MAX_INODES];
};

struct Block{
	unsigned char data[DISKBLOCK_SIZE];
};

const int SUPERBLOCK_SIZE = sizeof(SuperBlock);
const int SUPERBLOCK_BLOCKS = SUPERBLOCK_SIZE/DISKBLOCK_SIZE + 1;
const int SUPERBLOCK_BYTES = SUPERBLOCK_BLOCKS*DISKBLOCK_SIZE;

const int INODE_SIZE = sizeof(Inode);
const int INODE_LIST_SIZE = INODE_SIZE*MAX_INODES;
const int INODE_LIST_BLOCKS = INODE_LIST_SIZE/DISKBLOCK_SIZE + 1;
const int INODE_LIST_BYTES = INODE_LIST_BLOCKS*DISKBLOCK_SIZE;

#define FILENAME_SIZE 30

struct DirectoryEntry{
	char filename[FILENAME_SIZE];
	short int inode_no;
};

#define FILES_PER_DIR 8
struct Directory{
	DirectoryEntry entry[FILES_PER_DIR];
};

struct FDEntry{
	int offset;
	Inode * inode;
	char mode;
};

#define MAX_FDTABLE_SIZE 64
struct FDTable{
	FDEntry entry[MAX_FDTABLE_SIZE];
};

extern char* myfs;
extern int cur_dir;
extern FDTable fd_table;
extern int shmid;

int create_myfs(int size);

int copy_pc2myfs(char *source, char* dest);

int copy_myfs2pc(char *source, char *dest);

int rm_myfs(char *filename);

int showfile_myfs(char *filename);

int ls_myfs();

int mkdir_myfs(char *dirname);

int chdir_myfs(char* dirname);

int rmdir_myfs(char *dirname);

int open_myfs(char *filename, char mode);

int close_myfs(int fd);

int read_myfs(int fd, int nbytes, char *buff);

int write_myfs(int fd, int nbytes, char *buff);

int eof_myfs(int fd);

int dump_myfs(char *dumpfile);

int restore_myfs(char *dumpfile);

int status_myfs();

int chmod_myfs(char *name, int mode);

int free_myfs();

#endif
