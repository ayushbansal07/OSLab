#include <iostream>
#include <bitset>
#include <sstream>
#include <cstdlib>
#include <stdio.h>
#include <vector>

#define PAGE_TABLE_SIZE 64

using namespace std;

struct PageTableEntry{
	bitset<29> frameNo;
	bitset<1> valid;
	bitset<1> modified;
	bitset<1> referenced;
};

struct PageTable{
	PageTableEntry entry[PAGE_TABLE_SIZE];
};

void init_pagetable(PageTable *pt)
{
	pt= (PageTable *) malloc(PAGE_TABLE_SIZE*sizeof(PageTableEntry));
	for(int i=0;i<PAGE_TABLE_SIZE;i++)
	{
		pt->entry[i].frameNo = 0;
		pt->entry[i].valid = 0;
		pt->entry[i].modified = 0;
		pt->entry[i].referenced = 0;
	}
}

int get_free_frame(vector<bool> &free_frame, int sz)
{
	for(int i=0;i<sz;i++)
	{
		if(free_frame[i])
		{
			free_frame = false;
			return i;
		}
	}
	return -1;
}

int main()
{
	PageTable * pagetable;
	init_pagetable(pagetable);
	string line;
	int lineno = 0;
	int rw = 0;
	int vm_size;
	cin>>vm_size;
	cout<<vm_size<<endl;
	getchar();
	vector<bool> free_frame(vm_size,true);
	while(getline(cin,line))
	{
		if(line[0] == '#') continue;
		lineno++;
		rw = line[0] - '0';
		string sub_ref = line.substr(2); 
		stringstream ss(sub_ref);
		int referenced;
		ss>>referenced;
		cout<<rw<<" "<<referenced<<endl;

		
	}

}