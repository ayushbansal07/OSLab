all: vmm ptg

vmm: asgn5_33.cpp
	g++ asgn5_33.cpp -o vmm -std=c++11

ptg: page_trace_gen.cpp
	g++ page_trace_gen.cpp -o ptg -std=c++11 -Wno-write-strings

clean:
	rm vmm ptg