#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <bitset>

using namespace std;

string slice(string input, int start,int end) {
  string ret="";
  for(int i=start;i<end;i++){
    ret+=input[i];
  }
  return ret;
}

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
	int pht_size = int(pow(2,m));
	vector<bitset<2> > pht(pht_size);
	for(int i=0;i<pht_size;i++){
		pht[i] = bitset<2> ("10");			// given that PHT should be initialized at 10 (weak taken)
	}

	/*---------- Create BHT ----------*/
	int bht_size = int(pow(2,h)); // BHT is given to be 2^h array with w bit
	vector<bitset<32> > bht(bht_size);
	for(int i=0;i<bht_size;i++){
		bht[i] = bitset<32>("0");  	// given that BHT is initiallized to be zero
 	}
	
	/*----------- Read input ------------*/
	unsigned long long addr;
	int taken;
	while (trace >> hex >> addr >> taken) {
		bitset<32> currentPC(addr);		//translate pc value on current input line
		bitset<32> bhtIndex(slice(currentPC.to_string(), 30-h, 30));	// get BHT index (h bits)
		bitset<32> phtIndex((slice(currentPC.to_string(), 30-(m-w), 30))+ slice(bht[bhtIndex.to_ulong()].to_string(), 32-w, 32));	//concat of (m-w)bits from PC with w bits from BHT

		//read PHT 
		bitset<2> result = pht[phtIndex.to_ulong()];
		if (result.to_string() == "11" || result.to_string() == "10"){	//weak taken and strong taken
			out << 1 << endl; // predict not taken
		}
		else if(result.to_string() == "01" || result.to_string() == "00"){	//weak not taken and strong not taken
			out << 0 << endl; // predict not taken
		}

		//update the PHT from the actual branch action
		if (taken == 1){	// actual brance taken
			if (result.to_string() == "10"){	//weak taken go to strong taken
				pht[phtIndex.to_ulong()] = bitset<2> ("11");
			}
			if (result.to_string() == "01"){	//weak not taken go to weak taken
				pht[phtIndex.to_ulong()] = bitset<2> ("10");
			}
			if (result.to_string() == "00"){	//strong not taken go to weak not taken
				pht[phtIndex.to_ulong()] = bitset<2> ("01");
			}
		}
		if (taken== 0){	// actual brance not taken
			if (result.to_string() == "11"){	//strong taken go to weak taken
				pht[phtIndex.to_ulong()] = bitset<2> ("10");
			}
			if (result.to_string() == "10"){	//weak taken go to weak not taken
				pht[phtIndex.to_ulong()] = bitset<2> ("01");
			}
			if (result.to_string() == "01"){	//weak not taken go to strong not taken
				pht[phtIndex.to_ulong()] = bitset<2> ("00");
			}
		}
		//update the BHT from the actual branch action
		bitset<32>takenBitset(taken);
		bht[bhtIndex.to_ulong()] = bht[bhtIndex.to_ulong()] << 1 | takenBitset;			//bht left-shifting by 1 and placing the branch action in the LSB		
	}
	 
	trace.close();	
	out.close();
}

// Path: branchsimulator_skeleton_23.cpp
