#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;

#define MemSize (65536)

string slice(string input, int start,int end){
  string ret="";
  for(int i=start;i<end;i++){
    ret+=input[i];
  }
  return ret;
}

class PhyMem    
{
  public:
    bitset<32> readdata;  
    PhyMem()
    {
      DMem.resize(MemSize); 
      ifstream dmem;
      string line;
      int i=0;
      dmem.open("pt_initialize.txt");
      if (dmem.is_open())
      {
        while (getline(dmem,line))
        {      
          DMem[i] = bitset<8>(line);
          i++;
        }
      }
      else cout<<"Unable to open page table init file";
      dmem.close(); 

    }  
    bitset<32> outputMemValue (bitset<12> Address) 
    {    
      bitset<32> readdata;
      /**TODO: implement!
       * Returns the value stored in the physical address 
       */
      string first_byte = DMem[Address.to_ulong()].to_string();
      string second_byte = DMem[Address.to_ulong() +1].to_string();
      string third_byte = DMem[Address.to_ulong() +2].to_string();
      string fourth_byte = DMem[Address.to_ulong() +3].to_string();

      readdata = bitset<32> (first_byte + second_byte + third_byte + fourth_byte);
      return readdata;     
    }              

  private:
    vector<bitset<8> > DMem;

};  

int main(int argc, char *argv[])
{
    PhyMem myPhyMem;

    ifstream traces;
    ifstream PTB_file;
    ofstream tracesout;

    string outname;
    outname = "pt_results.txt";

    traces.open(argv[1]);
    PTB_file.open(argv[2]);
    tracesout.open(outname.c_str());

    //Initialize the PTBR
    bitset<12> PTBR;
    PTB_file >> PTBR;

    string line;
    bitset<14> virtualAddr;

    /*********************************** ↓↓↓ Todo: Implement by you ↓↓↓ ******************************************/

    // Read a virtual address form the PageTable and convert it to the physical address - CSA23
    if(traces.is_open() && tracesout.is_open())
    {
        while (getline(traces, line))
        {
        //TODO: Implement!
          //Step 1. Decode the virtual address & default values
          bitset<4> outerPTB(slice(line, 0, 4));
          bitset<4> innerPTB(slice(line, 4, 8));
          bitset<6> offset(slice(line, 9, 14));
          bitset<12> physicalAddress = 0;
          bitset<32> memoryValue = 0;
          bitset<1> validInner = 0;
          bitset<1> validOuter = 0;

          // Access the outer page table 
          bitset<12> accessOuter = (outerPTB.to_ulong() + PTBR.to_ulong()) << 2; //shift value by 2 added by PTBR
          bitset<32> returnOuter = myPhyMem.outputMemValue(accessOuter);

          // If outer page table valid bit is 1, access the inner page table 
          bitset<1> validOuter(slice(returnOuter.to_string(), 31, 32));
          
          if (validOuter.to_ulong()) {
            bitset<6> outerFrame(slice(returnOuter.to_string(), 0, 6));
            bitset<12> accessInner = (innerPTB.to_ulong() + outerFrame.to_ulong()) << 2;  //shift value by 2 
            bitset<32> returnInner = myPhyMem.outputMemValue(accessInner);

            // Check if inner page table valid bit is 1, return the physical address and Mem value
            bitset<1> validInner(slice(returnInner.to_string(), 31, 32));
            if (validInner.to_ulong()) {
              bitset<12> physicalAddress(slice(returnInner.to_string(), 0, 12));
              bitset<32> memoryValue = myPhyMem.outputMemValue(physicalAddress);
            }         
          }

          //Return valid bit in outer and inner page table, physical address, and value stored in the physical memory.
          // Each line in the output file for example should be: 1, 0, 0x000, 0x00000000
          tracesout << validOuter.to_ulong() << ", " 
                    << validInner.to_ulong() << ", "
                    << "0x" << setfill('0') << setw(3) << hex << physicalAddress.to_ulong() << ", "
                    << "0x" <<setfill('0') << setw(8) << hex << memoryValue.to_ulong() <<'\n';

        }
        traces.close();
        tracesout.close();
    }

    else
        cout << "Unable to open trace or traceout file ";

    return 0;
}
