#include <iostream>
#include <bitset>
#include <sstream>
#include <cstdlib>
#include <stdio.h>
#include <vector>
#include <queue>
#define PAGE_TABLE_SIZE 64

using namespace std;

enum class PageReplacementAlgo{
	FIFO = 0,
	RANDOM,
	LRU,
	NRU,
	DFIFO
};

enum class Instruction{
	UNMAP = 0,
	MAP,
	IN,
	OUT
};

struct PageTableEntry{
	bitset<29> frameNo;
	bitset<1> valid;
	bitset<1> modified;
	bitset<1> referenced;
};

class VMM{
private:
	PageTableEntry pagetable[PAGE_TABLE_SIZE];
	int max_frames;
	int num_used_frames;
	PageReplacementAlgo algo;

	queue<int> fifoQ;

public:
	VMM(int max_frames,PageReplacementAlgo algo = PageReplacementAlgo::FIFO)
	{
		for(int i=0;i<PAGE_TABLE_SIZE;i++)
		{
			this->pagetable[i].frameNo = 0;
			this->pagetable[i].valid = 0;
			this->pagetable[i].modified = 0;
			this->pagetable[i].referenced = 0;
		}
		this->max_frames = max_frames;
		this->algo = algo;
		this->num_used_frames = 0;
	}

	void setValid(int index){
		this->pagetable[index].valid[0] = 1;
	}
	void resetValid(int index){ 
		this->pagetable[index].valid[0] = 0;
	}
	bool getValidBit(int index){
		return this->pagetable[index].valid[0];
	}

	void setModified(int index){
		this->pagetable[index].modified[0] = 1;
	}
	void resetModified(int index){
		this->pagetable[index].modified[0] = 0;
	}
	bool getModifiedBit(int index){
		return this->pagetable[index].modified[0];
	}

	void setReferenced(int index){
		this->pagetable[index].referenced[0] = 1;
	}
	void resetReferenced(int index){
		this->pagetable[index].referenced[0] = 0;
	}
	bool getReferencedBit(int index){
		return this->pagetable[index].referenced[0];
	}

	int get_frame_number(int index)
	{
		return (int)this->pagetable[index].frameNo.to_ulong();
	}

	void setFrameNo(int pageNo, int frameNo)
	{
		this->pagetable[pageNo].frameNo = frameNo;
	}

	void accessFrame(int pageNo, int rw)
	{
		if(pageNo > PAGE_TABLE_SIZE){
			cout<<"Error: Invalid Virtual Table Location"<<endl;
			return;
		}
		if(!getValidBit(pageNo))
		{
			//Page Not in Frame
			int newFrame;
			if(this->num_used_frames < this->max_frames){
				//NO REPLACEMENT
				this->num_used_frames++;
				if(this->algo == PageReplacementAlgo::FIFO){
					fifoQ.push(pageNo);
				}
				//TODO: other Page Replacement Algos
			}
			else
			{
				//REPLACEMENT
				int rm_indx = 0;
				if(this->algo == PageReplacementAlgo::FIFO)
				{
					rm_indx = fifoQ.front();
					fifoQ.pop();
					fifoQ.push(pageNo);
				}
				//TODO: other page replacement algos
				newFrame = get_frame_number(rm_indx);
				resetValid(rm_indx);
				//TODO: Print UNMAP Instr
				//TODO: IF MODIFIED SET DO SOMETHING

			}
			setValid(pageNo);
			setModified(pageNo);
			setFrameNo(pageNo,newFrame);
			//TODO: print Instruction, tarnsfer count, fault count, etc

			
		}
		else
		{
			//Page in Frame

		}
		setReferenced(pageNo);
		if(rw) setModified(pageNo);

	}

};

int get_free_frame(vector<bool> &free_frame, int sz)
{
	for(int i=0;i<sz;i++)
	{
		if(free_frame[i])
		{
			free_frame[i] = false;
			return i;
		}
	}
	return -1;
}

int main()
{
	string line;
	int lineno = 0;
	int rw = 0;
	int vm_size;
	cin>>vm_size;
	cout<<vm_size<<endl;
	getchar();
	VMM virtual_mem = VMM(vm_size);
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