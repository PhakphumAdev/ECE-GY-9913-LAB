/*
Cache Simulator
Level one L1 and level two L2 cache parameters are read from file (block size, line per set and set per cache).
The 32 bit address is divided into tag bits (t), set index bits (s) and block offset bits (b)
s = log2(#sets)   b = log2(block size in bytes)  t=32-s-b
32 bit address (MSB -> LSB): TAG || SET || OFFSET

Tag Bits   : the tag field along with the valid bit is used to determine whether the block in the cache is valid or not.
Index Bits : the set index field is used to determine which set in the cache the block is stored in.
Offset Bits: the offset field is used to determine which byte in the block is being accessed.
*/

#include <algorithm>
#include <bitset>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <vector>
using namespace std;
//access state:
#define NA 0 // no action
#define RH 1 // read hit
#define RM 2 // read miss
#define WH 3 // Write hit
#define WM 4 // write miss
#define NOWRITEMEM 5 // no write to memory
#define WRITEMEM 6   // write to memory
#define setAssociative 1
#define fullyAssociative 0

string split_str(int start,int end,string input){
    string ret = "";
    int len = input.length();
    for(int i=start;i<end;i++){
        ret+=input[i];
    }
    return ret;
}

struct config
{
    int L1blocksize;
    int L1setsize;
    int L1size;
    int L2blocksize;
    int L2setsize;
    int L2size;
};

/*********************************** ↓↓↓ Todo: Implement by you ↓↓↓ ******************************************/

struct retMem{
    int accessState;
    int memState;
};
struct cacheBlock
{
    //bitset<32> data  - no need for this lab
    unsigned long tag = 0;
    bool dirty = false;
    bool valid = false;
    bitset<32>addr;
};
struct set
{
    vector<cacheBlock>  myblock;
    int counter = 0;
};
struct cache 
{
    vector<set> myset;
    int num_set;
    int num_block;
    int block_size;
    int associative;
};

struct retDecode{
    unsigned long tag,index,offset;
    int tag_size,index_size,offset_size;
};

class CacheSystem
{
public:
    cache L1, L2;

    CacheSystem(int block_size1, int num_way1, int cache_size1, int block_size2, int num_way2, int cache_size2) {
        // initialize L1
        if (num_way1 == 0){
            //Fully Associative
            L1.associative = fullyAssociative;
            L1.numset = 1;
            L1.num_block = (cache_size1 * 1024) / (block_size1);
            L1.block_size = block_size1;           
        }
        else{
            //Set associative
            L1.associative = setAssociative;
            L1.num_set = (cache_size1 * 1024) / (block_size1 * num_way1);
            L1.num_block = num_way1;
            L1.block_size = block_size1;
        }
        L1.myset.resize(L1.num_set);
        for (int i=0; i<L1.num_set; i++) {
            L1.myset[i].myblock.resize(L1.num_block);
        }
        

        // initialize L2
        if (num_way2 == 0){
            //Fully Associative
            L2.associative = fullyAssociative;
            L2.numset = 1;
            L2.num_block = (cache_size2 * 1024) / (block_size2);
            L2.block_size = block_size2;           
        }
        else{
            //Set associative
            L2.associative = setAssociative;
            L2.num_set = (cache_size2 * 1024) / (block_size2 * num_way2);
            L2.num_block = num_way2;
            L2.block_size = block_size2;
        }
        L2.myset.resize(L2.num_set);
        for (int i=0; i<L2.num_set; i++) {
            L2.myset[i].myblock.resize(L2.num_block);
        }        
    }

    retDecode decodeL1 (bitset<32> addr) {
        retDecode ret;
        int offset_size = log2(L1.block_size);
        int index_size = log2(L1.num_set);
        int tag_size = 32 - offset_size - index_size;
        unsigned long offset = (bitset<32>(split_str(32-offset_size,32,addr.to_string()))).to_ulong();
        unsigned long index = (bitset<32>(split_str(tag_size,32-offset_size,addr.to_string()))).to_ulong();
        unsigned long tag = (bitset<32>(split_str(0,tag_size,addr.to_string()))).to_ulong();
        ret.offset = offset;
        ret.offset_size = offset_size;
        ret.index = index;
        ret.index_size = index_size;
        ret.tag = tag;
        ret.tag_size = tag_size;
        return ret;
    }

    retDecode decodeL2 (bitset<32> addr) {
        retDecode ret;
        int offset_size = log2(L2.block_size);
        int index_size = log2(L2.num_set);
        int tag_size = 32 - offset_size - index_size;
        unsigned long offset = (bitset<32>(split_str(32-offset_size,32,addr.to_string()))).to_ulong();
        unsigned long index = (bitset<32>(split_str(tag_size,32-offset_size,addr.to_string()))).to_ulong();
        unsigned long tag = (bitset<32>(split_str(0,tag_size,addr.to_string()))).to_ulong();
        ret.offset = offset;
        ret.offset_size = offset_size;
        ret.index = index;
        ret.index_size = index_size;
        ret.tag = tag;
        ret.tag_size = tag_size;
        return ret;
    }

    int writeL1(bitset<32> addr){
        if (L1.associative == fullyAssociative) {   
            unsigned long index = 0;            //use only one set0
            unsigned long tag = addr.to_ulong() //tag is the addr in this case            
        }
        if (L1.associative == setAssociative) {
            retDecode ret = decodeL1(addr);
            unsigned long index = ret.index;    //index is the set number
            unsigned long tag = ret.tag;        //tag is used to verify the block with addr
        }        
        for (int i=0; i<L1.num_block; i++) {
            if (L1.myset[index].myblock[i].tag == tag && L1.myset[index].myblock[i].valid) {       //check each block in that set
                L1.myset[index].myblock[i].dirty = true;           
                return WH;     
            }
        }
        return WM;

    }      
    

    int writeL2(bitset<32> addr){  
        if (L2.associative == fullyAssociative) {
            unsigned long index = 0;            //use only one set0
            unsigned long tag = addr.to_ulong() //tag is the addr in this case            
        }
        if (L2.associative == setAssociative) {
            retDecode ret = decodeL2(addr);
            unsigned long index = ret.index;    //index is the set number
            unsigned long tag = ret.tag;        //tag is used to verify the block with addr
        }  
        for (int i=0; i<L2.num_block; i++) {
            if (L2.myset[index].myblock[i].tag == tag && L2.myset[index].myblock[i].valid) {        //check each block in that set
                L2.myset[index].myblock[i].dirty = true; 
                return WH;      
            }
        }
        return WM;
    }

    int readL1(bitset<32> addr){
        if (L1.associative == fullyAssociative) {   
            unsigned long index = 0;            //use only one set0
            unsigned long tag = addr.to_ulong() //tag is the addr in this case            
        }
        if (L1.associative == setAssociative) {
            retDecode ret = decodeL1(addr);
            unsigned long index = ret.index;    //index is the set number
            unsigned long tag = ret.tag;        //tag is used to verify the block with addr
        }   
        for (int i=0; i<L1.num_block; i++) {
            if (L1.myset[index].myblock[i].tag == tag && L1.myset[index].myblock[i].valid) {        
                return RH;  
            }
        }
        return RM;
    }

    retMem readL2(bitset<32> addr){
        if (L2.associative == fullyAssociative) {
            unsigned long index = 0;            //use only one set0
            unsigned long tag = addr.to_ulong() //tag is the addr in this case            
        }
        if (L2.associative == setAssociative) {
            retDecode ret = decodeL2(addr);
            unsigned long index = ret.index;    //index is the set number
            unsigned long tag = ret.tag;        //tag is used to verify the block with addr
        }  
        for (int i=0; i<L2.num_block; i++) {
            if (L2.myset[index].myblock[i].tag == tag && L2.myset[index].myblock[i].valid) {      
                // Read Hit          
                //delete block from L2
                bool dirty_temp = L2.myset[index].myblock[i].dirty;
                bitset<32> addr_temp = L2.myset[index].myblock[i].addr;
                L2.myset[index].myblock[i].valid = false;   
                L2.myset[index].myblock[i].dirty = false;
                L2.myset[index].myblock[i].addr = 0;
                L2.myset[index].myblock[i].tag = 0;
        
                //move the block from L2 to L1  
                ret.memState = addL1(addr_temp, dirty_temp);                
                ret.accessState = RH;   
                
                //return values for both L2 state and memState
                return ret;  
            }
        }
        //read miss -> fectch from memmory to L1
        ret.memState = addL1(addr, false);
        ret.accessState = RM;
        return ret;
    }

    int addL1(bitset<32> addr, bool dirt) {
        if (L1.associative == fullyAssociative) {   
            unsigned long index = 0;            //use only one set0
            unsigned long tag = addr.to_ulong() //tag is the addr in this case            
        }
        if (L1.associative == setAssociative) {
            retDecode ret = decodeL1(addr);
            unsigned long index = ret.index;    //index is the set number
            unsigned long tag = ret.tag;        //tag is used to verify the block with addr
        }   
        for (int i=0; i<L1.num_block; i++) {
            if(!L1.myset[index].myblock[i].valid) { //if there's an empty spot
                L1.myset[index].myblock[i].valid = true;
                L1.myset[index].myblock[i].dirty = dirt;
                L1.myset[index].myblock[i].tag = tag;
                L1.myset[index].myblock[i].addr = addr;
                return NOWRITEMEM;
            }
        }
        //if there's no empty spot -> evict
        //push a block to L2 with ROUND-ROBIN
        int check_WB;
        bool dirty_temp = L1.myset[index].myblock[L1.myset[index].counter].dirty;
        bitset<32> addr_temp = L1.myset[index].myblock[L1.myset[index].counter].addr;
        L1.myset[index].myblock[L1.myset[index].counter].valid = false;
        L1.myset[index].myblock[L1.myset[index].counter].dirty = false;    
        L1.myset[index].myblock[L1.myset[index].counter].addr = 0;  
        L1.myset[index].myblock[L1.myset[index].counter].tag = 0;    
        check_WB = addL2(addr_temp, dirty_temp);
        

        //replace block in L1 to new addr
        L1.myset[index].myblock[L1.myset[index].counter].tag = tag;
        L1.myset[index].myblock[L1.myset[index].counter].addr = addr; 
        L1.myset[index].myblock[L1.myset[index].counter].dirty = dirt;
        L1.myset[index].myblock[L1.myset[index].counter].valid = true;

        //increment counter 
        L1.myset[index].counter++;
        if (L1.myset[index].counter == L1.num_block) {
            L1.myset[index].counter = 0;
        }

        return check_WB;
    }

    int addL2(bitset<32> addr, bool dirt) {
        if (L2.associative == fullyAssociative) {
            unsigned long index = 0;            //use only one set0
            unsigned long tag = addr.to_ulong() //tag is the addr in this case            
        }
        if (L2.associative == setAssociative) {
            retDecode ret = decodeL2(addr);
            unsigned long index = ret.index;    //index is the set number
            unsigned long tag = ret.tag;        //tag is used to verify the block with addr
        }  
        for (int i=0; i<L2.num_block; i++) {
            if(!L2.myset[index].myblock[i].valid) { //if there's an empty spot
                L2.myset[index].myblock[i].tag = tag;
                L2.myset[index].myblock[i].addr = addr;
                L2.myset[index].myblock[i].dirty = dirt;
                L2.myset[index].myblock[i].valid = true;
                return NOWRITEMEM;
            }
        }
        //if there's no empty spot -> evict
        //replace block in L2 to new addr
        int check_WB;
        if (L2.myset[index].myblock[L2.myset[index].counter].dirty) {
            check_WB = WRITEMEM;
        }
        else{
            check_WB = NOWRITEMEM;
        }
        L2.myset[index].myblock[L2.myset[index].counter].tag = tag;
        L2.myset[index].myblock[L2.myset[index].counter].addr = addr;
        L2.myset[index].myblock[L2.myset[index].counter].dirty = dirt;
        L2.myset[index].myblock[L2.myset[index].counter].valid = true;

        //increment counter
        L2.myset[index].counter++;
        if (L2.myset[index].counter == L2.num_block) {
            L2.myset[index].counter = 0;
        }
        return check_WB;     
    }

};
/*********************************** ↑↑↑ Todo: Implement by you ↑↑↑ ******************************************/





int main(int argc, char *argv[])
{

    config cacheconfig;
    ifstream cache_params;
    string dummyLine;
    cache_params.open(argv[1]);
    while (!cache_params.eof())                   // read config file
    {
        cache_params >> dummyLine;                // L1:
        cache_params >> cacheconfig.L1blocksize;  // L1 Block size
        cache_params >> cacheconfig.L1setsize;    // L1 Associativity
        cache_params >> cacheconfig.L1size;       // L1 Cache Size
        cache_params >> dummyLine;                // L2:
        cache_params >> cacheconfig.L2blocksize;  // L2 Block size
        cache_params >> cacheconfig.L2setsize;    // L2 Associativity
        cache_params >> cacheconfig.L2size;       // L2 Cache Size
    }
    ifstream traces;
    ofstream tracesout;
    string outname;
    outname = string(argv[2]) + ".out";
    traces.open(argv[2]);
    tracesout.open(outname.c_str());
    string line;
    string accesstype;     // the Read/Write access type from the memory trace;
    string xaddr;          // the address from the memory trace store in hex;
    unsigned int addr;     // the address from the memory trace store in unsigned int;
    bitset<32> accessaddr; // the address from the memory trace store in the bitset;




/*********************************** ↓↓↓ Todo: Implement by you ↓↓↓ ******************************************/
    // Implement by you:
    // initialize the hirearch cache system with those configs
    // probably you may define a Cache class for L1 and L2, or any data structure you like
    if (cacheconfig.L1blocksize!=cacheconfig.L2blocksize){
        printf("please test with the same block size\n");
        return 1;
    }

    // create caches
    CacheSystem myCacheSystem(cacheconfig.L1blocksize, cacheconfig.L1setsize, cacheconfig.L1size,
                              cacheconfig.L2blocksize, cacheconfig.L2setsize, cacheconfig.L2size);


    int L1AcceState = 0; // L1 access state variable, can be one of NA, RH, RM, WH, WM;
    int L2AcceState = 0; // L2 access state variable, can be one of NA, RH, RM, WH, WM;
    int MemAcceState = 0; // Main Memory access state variable, can be either NA or WH;

    if (traces.is_open() && tracesout.is_open())
    {
        while (getline(traces, line))
        { // read mem access file and access Cache

            istringstream iss(line);
            if (!(iss >> accesstype >> xaddr)){
                break;
            }
            stringstream saddr(xaddr);
            saddr >> std::hex >> addr;
            accessaddr = bitset<32>(addr);

            /* Read and Write Functions*/
            // a Read request
            if (accesstype.compare("R") == 0)  
            {
                //Try read L1
                L1AcceState = myCacheSystem.readL1(accessaddr);
                if(L1AcceState == RH){      //if read L1 success
                    L2AcceState = NA;
                    MemAcceState = NOWRITEMEM;
                }
                else if(L1AcceState == RM){ //if read L1 fail, try read L2
                    retMem ret = myCacheSystem.readL2(accessaddr);
                    L2AcceState = ret.accessState;
                    MemAcceState = ret.memState;
                }
            }
            // a Write request
            else{ 
                //Try write L1
                L1AcceState = myCacheSystem.writeL1(accessaddr);
                if (L1AcceState == WH) {    //if write L1 success
                    L2AcceState = NA;
                    MemAcceState = NOWRITEMEM;
                }
                else if (L1AcceState == WM){    //if write L1 fail, try write L2
                    L2AcceState = myCacheSystem.writeL2(accessaddr);
                }

                //if both writeL1 and writeL2 fail -> write in MEM
                if (L1AcceState == WM && L2AcceState == WM) {   
                    MemAcceState = WRITEMEM;
                }
                else {
                    MemAcceState = NOWRITEMEM;
                }

            }

/*********************************** ↑↑↑ Todo: Implement by you ↑↑↑ ******************************************/




            // Grading: don't change the code below.
            // We will print your access state of each cycle to see if your simulator gives the same result as ours.
            tracesout << L1AcceState << " " << L2AcceState << " " << MemAcceState << endl; // Output hit/miss results for L1 and L2 to the output file;
        }
        traces.close();
        tracesout.close();
    }
    else
        cout << "Unable to open trace or traceout file ";

    return 0;
}
