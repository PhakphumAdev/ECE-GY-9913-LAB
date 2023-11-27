#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <bitset>

using namespace std;

int main (int argc, char** argv) {
	ifstream config;
	config.open(argv[1]);

	int m, w, h;
	config >> m;
	config >> h;
	config >> w;
 
	config.close();

	ofstream out;
	string out_file_name = string(argv[2]) + ".out";
	out.open(out_file_name.c_str());
	
	ifstream trace;
	trace.open(argv[2]);

	// TODO: Implement a two-level branch predictor 

	/*--------- Create PHT -----------*/
	bitset<2> pht[pow(2,m)];
	for i in pht:
		pht[i] = bitset<2> ("10");			// given that PHT should be initialized at 10 (weak taken)


	/*---------- Create BHT ----------*/
	bitset<w> bht[pow(2,h)];				// BHT is given to be 2^h array with w bit
	for i in bht:
		bht[i] = bitset<w> ("0");			// given that BHT is initiallized to be zero


	/*----------- Read input ------------*/
	while (!trace.eof()) {
		bitset<32> currentPC = trace[0].to_ulong();		//translate pc value on current input line
		bitset<h> bhtIndex = slice(currentPC, 30-h, 30);	// get BHT index (h bits)
		bitset<w> phtIndex = bitset<w> (slice(currentPC, 30-(m-w), 30)).to_string() + bht[bhtIndex].to_string());	//concat of (m-w)bits from PC with w bits from BHT

		//read PHT 
		bitset<2> result = pht[phtIndex];
		if (result.to_string() == "11" or result.to_string() == "10"):	//weak taken and strong taken
			out << 1 << endl; // predict not taken
		elif (result.to_string() == "01" or result.to_string() == "00"):	//weak not taken and strong not taken
			out << 0 << endl; // predict not taken

		//update the PHT from the actual branch action
		if (trace[1] == 1):	// actual brance taken
			if (result.to_string() == "10"):	//weak taken go to strong taken
				pht[phtIndex] = bitset<2> ("11");
			if (result.to_string() == "01"):	//weak not taken go to weak taken
				pht[phtIndex] = bitset<2> ("10");
			if (result.to_string() == "00"):	//strong not taken go to weak not taken
				pht[phtIndex] = bitset<2> ("01");
		if (trace[1] == 0):	// actual brance not taken
			if (result.to_string() == "11"):	//strong taken go to weak taken
				pht[phtIndex] = bitset<2> ("10");
			if (result.to_string() == "10"):	//weak taken go to weak not taken
				pht[phtIndex] = bitset<2> ("01");
			if (result.to_string() == "01"):	//weak not taken go to strong not taken
				pht[phtIndex] = bitset<2> ("00");

		//update the BHT from the actual branch action
		bht[bhtIndex] = bht[bhtIndex] << 1 | trace[1];			//bht left-shifting by 1 and placing the branch action in the LSB		
	}
	 
	trace.close();	
	out.close();
}

// Path: branchsimulator_skeleton_23.cpp
