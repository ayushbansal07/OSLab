// use g++ main.cpp -std=c++11 for compiling

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <queue>
#include <set>
#include <vector>
#include <climits>

#define MAX_INSTR_LENGTH 256
#define VIRTUAL_TABLE_SZ 64
#define NRU_REF_RESET_COUNT 10

using namespace std;

int cur_line = 0;

class VMMParser{
	string fname;
	ifstream fhandler;
	int curline;
public:
	VMMParser(string fname){
		this->fname = fname;
		fhandler.open(fname);
		if(!fhandler.is_open()){
			cout << "Could not open file " << fname << "!\n";
			exit(1);
		}
		this->curline = 0;
	}
	pair<int,int> read_next(){
		if(fhandler.eof()){
			return make_pair(-1, 0);
		}
		char buff[MAX_INSTR_LENGTH];
		while(!fhandler.eof()){
			fhandler.getline(buff, MAX_INSTR_LENGTH);
			curline ++;
			cur_line ++;
			if(buff[0] != '#')
				break;
		}
		if(buff[0] == '#'){
			return make_pair(-1, 0);
		}
		int sep = -1, i;
		for(i=0;buff[i]!='\0';i++)
			if(buff[i] == ' '){
				sep = i;
				break;
			}
		if(buff[i] == '\0'){
			return make_pair(-1,0);
		}
		if(sep == -1){
			cout << "Invalid instruction format at line: " << curline << endl;
			return make_pair(-1, 0);
		}
		buff[sep] = '\0';
		int rw = atoi(buff);
		if(rw != 0 && rw != 1){
			cout << "Invalid rw bit at line: " << curline << endl;
			return make_pair(-1, 0);
		}
		int pno = atoi(buff + sep + 1);
		if(pno >= VIRTUAL_TABLE_SZ || pno < 0){
			cout << "Invalid virtual table location access at line: " << curline << endl;
			return make_pair(-1, 0);
		}
		return make_pair(rw, pno);
	}
};

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

void print_instr(Instruction instr, int pno, int fno, long long int& cycles){
	cout << cur_line << ": ";
	switch(instr){
		case Instruction::UNMAP:{
			cout << "UNMAP "; cycles += 250; break;
		}
		case Instruction::MAP:{
			cout << "MAP "; cycles += 250; break;
		}
		case Instruction::IN:{
			cout << "IN "; cycles += 3000; break;
		}
		case Instruction::OUT:{
			cout << "OUT "; cycles += 3000; break;
		}
	}
	cout << pno << " " << fno << "\n";
}

class VMM{
	int page_table[VIRTUAL_TABLE_SZ];
	int cur_empty_frame;
	int max_frames;
	int fault_count;
	int tranfer_count;
	long long int cycles;
	PageReplacementAlgo algo;

	// for FIFO pra or SC pra
	queue<int> fifoq;

	// for RANDOM pra
	set<int> is_used;

	// for LRU pra
	int lru_count = 0;
	vector<int> lru_timestamp;

	// for NRU pra
	set<int> nru_class[4];

	inline void setbit_VALID(int& x, bool val){
		if((x >> 2)%2 != val)
			x ^= (1 << 2);
	}
	inline void setbit_MODIFIED(int& x, bool val){
		if((x >> 1)%2 != val)
			x ^= (1 << 1);
	}
	inline void setbit_REFERENCED(int& x, bool val){
		if(x%2 != val)
			x ^= 1;
	}
	inline bool getbit_VALID(int x){
		return (x >> 2)%2;
	}
	inline bool getbit_MODIFIED(int x){
		return (x >> 1)%2;
	}
	inline bool getbit_REFERENCED(int x){
		return x%2;
	}
	inline int get_frame(int x){
		return (x >> 3);
	}
	inline void set_frame(int& x, int f){
		x = (f << 3) | (x % (1 << 3));
	}
public:
	VMM(int max_frames, PageReplacementAlgo algo = PageReplacementAlgo::FIFO){
		this->max_frames = max_frames;
		cur_empty_frame = 0;
		fault_count = 0;
		tranfer_count = 0;
		cycles = 0;
		for(int i=0;i<VIRTUAL_TABLE_SZ;i++){
			page_table[i] = 0;
		}
		this->algo = algo;
		switch(algo){
			case PageReplacementAlgo::LRU:{
				lru_timestamp.resize(VIRTUAL_TABLE_SZ, -1);
				break;
			}
		}
	}

	void print_table(){
		cout << "---- Page Table ----\n";
		for(int i=0;i<VIRTUAL_TABLE_SZ;i++){
			if(getbit_VALID(page_table[i])){
				cout << "page idx: " << i << " -> " << get_frame(page_table[i]) << ", dirty: " << getbit_MODIFIED(page_table[i]) << ", referenced: " << getbit_REFERENCED(page_table[i]) << "\n";
			}
		}
	}

	void print_summary(){
		cout << "---- Summary Statistics ----\n";
		cout << "Page fault count: " << fault_count << ", page transfer operation count: " << tranfer_count << ", cycles required: " << cycles << endl;
	}

	void access(int loc, bool wmode, bool detail_print = false){
		if(loc < 0 || loc > VIRTUAL_TABLE_SZ){
			cout << "Invalid virtual table location access" << endl;
			return;
		}
		if(!getbit_VALID(page_table[loc])){
			// page not in frame
			int new_frame;
			if(cur_empty_frame < max_frames){
				// does not require replacement
				new_frame = cur_empty_frame;
				cur_empty_frame ++;

				switch(algo){
					case PageReplacementAlgo::FIFO:{
						fifoq.push(loc);
						break;
					}
					case PageReplacementAlgo::RANDOM:{
						is_used.insert(loc);
						break;
					}
					case PageReplacementAlgo::LRU:{
						lru_timestamp[loc] = lru_count ++;
						break;
					}
					case PageReplacementAlgo::NRU:{
						if(wmode)
							nru_class[3].insert(loc);
						else
							nru_class[2].insert(loc);
						break;
					}
					case PageReplacementAlgo::DFIFO:{
						fifoq.push(loc);
						break;
					}
				}
			}
			else{
				// use replacement algo
				int rmindx = 0;
				switch(algo){
					case PageReplacementAlgo::FIFO:{
						rmindx = fifoq.front();
						fifoq.pop();
						fifoq.push(loc);
						break;
					}
					case PageReplacementAlgo::RANDOM:{
						rmindx = rand()%max_frames;
						set<int>::const_iterator it(is_used.begin());
						advance(it, rmindx);
						rmindx = *it;
						is_used.erase(it);
						is_used.insert(loc);
						break;
					}
					case PageReplacementAlgo::LRU:{
						int min_timestamp = INT_MAX;
						for(int i=0;i<VIRTUAL_TABLE_SZ;i++){
							if(lru_timestamp[i] != -1){
								if(lru_timestamp[i] < min_timestamp){
									rmindx = i;
									min_timestamp = lru_timestamp[i];
								}
							}
						}
						lru_timestamp[rmindx] = -1;
						lru_timestamp[loc] = lru_count ++;
						break;
					}
					case PageReplacementAlgo::NRU:{
						for(int i=0;i<4;i++){
							if(!nru_class[i].empty()){
								rmindx = *nru_class[i].begin();
								nru_class[i].erase(rmindx);
								break;
							}
						}
						if(wmode)
							nru_class[3].insert(loc);
						else
							nru_class[2].insert(loc);
						break;
					}
					case PageReplacementAlgo::DFIFO:{
						rmindx = fifoq.front();
						while(getbit_REFERENCED(page_table[rmindx])){
							setbit_REFERENCED(page_table[rmindx], 0);
							fifoq.pop();
							fifoq.push(rmindx);
							rmindx = fifoq.front();
						}
						fifoq.pop();
						fifoq.push(loc);
						break;
					}
				}
				new_frame = get_frame(page_table[rmindx]);
				setbit_VALID(page_table[rmindx], 0);
				print_instr(Instruction::UNMAP, rmindx, new_frame, cycles);
				if(getbit_MODIFIED(page_table[rmindx])){
					print_instr(Instruction::OUT, rmindx, new_frame, cycles);
					tranfer_count ++;
				}

			}
			setbit_VALID(page_table[loc], 1);
			setbit_MODIFIED(page_table[loc], 0);
			set_frame(page_table[loc], new_frame);
			print_instr(Instruction::IN, loc, new_frame, cycles);
			tranfer_count ++;
			print_instr(Instruction::MAP, loc, new_frame, cycles);
			fault_count ++;

			if(algo == PageReplacementAlgo::NRU){
				if(fault_count%NRU_REF_RESET_COUNT == 0){
					vector<int> temp;
					for(auto v : nru_class[3])
						temp.push_back(v);
					for(auto v : temp)
						nru_class[1].insert(v);
					temp.clear();
					nru_class[3].clear();
					for(auto v : nru_class[2])
						temp.push_back(v);
					for(auto v : temp)
						nru_class[0].insert(v);
					temp.clear();
					nru_class[2].clear();
				}
			}
		}
		else{
			// page already loaded in frame
			switch(algo){
				case PageReplacementAlgo::LRU:{
					lru_timestamp[loc] = lru_count ++;
					break;
				}
				case PageReplacementAlgo::NRU:{
					for(int i=0;i<4;i++){
						if(nru_class[i].find(loc) != nru_class[i].end()){
							if(wmode){
								nru_class[i].erase(loc);
								nru_class[3].insert(loc);
							}
							else{
								if(i < 2){
									nru_class[i].erase(loc);
									nru_class[2].insert(loc);
								}
							}
							break;
						}
					}
					break;
				}
			}
		}
		cycles ++;
		setbit_REFERENCED(page_table[loc], 1);
		if(wmode)
			setbit_MODIFIED(page_table[loc], 1);

		if(detail_print){
			print_table();
			print_summary();
		}
	}
};

int main(int argc, char const *argv[]){
	// variable declarations
	bool trace_mode = false;
	int file_ind = 1;

	if(argc < 2){
		cout << "Usage: " << argv[0] << " [-t] instructions.txt" << endl;
		exit(1);
	}
	for(int i=1;i<argc;i++){
		if(!strcmp(argv[i], "-t"))
			trace_mode = true;
		else
			file_ind = i;
	}

	VMMParser file(argv[file_ind]);

	int max_frames;
	cout << "Enter max frames: ";
	cin >> max_frames;

	VMM vmm(max_frames, PageReplacementAlgo::DFIFO);

	while(1){
		pair<int,int> instr = file.read_next();
		if(instr.first == -1)
			break;
		vmm.access(instr.second, instr.first, trace_mode);
	}

	if(!trace_mode){
		vmm.print_table();
		vmm.print_summary();
	}

	return 0;
}
