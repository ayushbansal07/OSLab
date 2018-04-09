#include <fstream>
#include <iostream>
#include <set>
#include <cstdlib>	

using namespace std;

#define PREC 1000
#define PLOT_FILE "plot_data.txt"

class PageTraceGen{
	int max_pages;
	int count;
	int ws_sz;
	float prob_same, prob_r;

public:
	PageTraceGen(int max_pages, int count, int ws_sz, float prob_same, float prob_r){
		this->max_pages = max_pages;
		this->count = count;
		this->ws_sz = ws_sz;
		this->prob_same = prob_same;
		this->prob_r = prob_r;
	}

	void generate(char* filename){

		ofstream file(filename), plot_file(PLOT_FILE);
		set<int> ws;

		// generate plot metadata
		plot_file << count << " " << max_pages-1 << " " << ws_sz << "\n";

		//generate comments
		file << "# Total number of pages: " << max_pages << "\n";
		file << "# Number of page references: " << count << "\n";
		file << "# Size of working set: " << ws_sz << "\n";
		file << "# Probability of using current working set: " << prob_same << "\n";
		file << "# Probability of using read mode: " << prob_r << "\n";

		for(int i=0;i<count;i++){
			int rw_bit, mem_addr;

			// generate rw bit
			rw_bit = rand()%PREC > prob_r*PREC;

			// generate logical memory address
			if(i < ws_sz){
				mem_addr = rand()%max_pages;
				ws.insert(mem_addr);
			}
			else{
				int ind = rand()%ws_sz;
				set<int>::const_iterator it(ws.begin());
				advance(it, ind);
				ind = *it;
				bool use_ws = rand()%PREC < prob_same*PREC;
				if(use_ws){
					mem_addr = ind;
				}
				else{
					ws.erase(ind);
					mem_addr = rand()%max_pages;
					ws.insert(mem_addr);
				}
			}
			// write to file
			file << rw_bit << " " << mem_addr << "\n";

			// write plot data
			for(auto x : ws)
				plot_file << x << " ";
			plot_file << "\n";
		}

		file.close();
		plot_file.close();
	}
};

int main(int argc, char const *argv[])
{
	int max_pages, count, ws_sz;
	float prob_same, prob_r;
	cout << "Total number of pages: ";
	cin >> max_pages;
	cout << "Number of page references: ";
	cin >> count;
	cout << "Size of working set: ";
	cin >> ws_sz;
	cout << "Probability next mem ref is from current ws: ";
	cin >> prob_same;
	cout << "Probability of using read mode: ";
	cin >> prob_r;
	PageTraceGen pgt(max_pages, count, ws_sz, prob_same, prob_r);
	pgt.generate("instr.txt");
	return 0;
}