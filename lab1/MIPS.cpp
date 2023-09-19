#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>

using namespace std;

#define ADDU (1)
#define SUBU (3)
#define AND (4)
#define OR  (5)
#define NOR (7)

// Memory size.
// In reality, the memory size should be 2^32, but for this lab and space reasons,
// we keep it as this large number, but the memory is still 32-bit addressable.
#define MemSize (65536)

string slice(int start,int end,string input){
  string ret="";
  for(int i=start;i<end;i++){
    ret+=input[i];
  }
  return ret;
}

class RF
{
  public:
    bitset<32> ReadData1, ReadData2; 
    RF()
    { 
      Registers.resize(32);  
      Registers[0] = bitset<32> (0);  // register 0 set to zero
    }

    void ReadWrite(bitset<5> RdReg1, bitset<5> RdReg2, bitset<5> WrtReg, bitset<32> WrtData, bitset<1> WrtEnable)
    {   
      /**
       * @brief Reads or writes data from/to the Register.
       *
       * This function is used to read or write data from/to the register, depending on the value of WrtEnable.
       * Put the read results to the ReadData1 and ReadData2.
       */
      // TODO: implement!     
      if (WrtEnable.to_ulong() == 0) {
        //read       
        ReadData1 = Registers[RdReg1.to_ulong()];
        ReadData2 = Registers[RdReg2.to_ulong()];
      }
      else {
        //write
        Registers[WrtReg.to_ulong()] = WrtData;
      }
    }

    void OutputRF()
    {
      ofstream rfout;
      rfout.open("RFresult.txt",std::ios_base::app);
      if (rfout.is_open())
      {
        rfout<<"A state of RF:"<<endl;
        for (int j = 0; j<32; j++)
        {        
          rfout << Registers[j]<<endl;
        }

      }
      else cout<<"Unable to open file";
      rfout.close();

    }     
  private:
    vector<bitset<32> >Registers;
};


class ALU
{
  public:
    bitset<32> ALUresult;
    bitset<32> ALUOperation (bitset<3> ALUOP, bitset<32> oprand1, bitset<32> oprand2)
    {   
      /**
       * @brief Implement the ALU operation here.
       *
       * ALU operation depends on the ALUOP, which are definded as ADDU, SUBU, etc. 
       */
      // TODO: implement!
      switch(ALUOP.to_ulong()){
        case ADDU: ALUresult = oprand1.to_ulong() + oprand2.to_ulong();
        break;
        case SUBU: ALUresult = oprand1.to_ulong() - oprand2.to_ulong();
        break;
        case AND: ALUresult = oprand1 & oprand2;
        break;
        case OR: 
        break; ALUresult = oprand1 | oprand2;
        case NOR: ALUresult = ~(oprand1|oprand2);
        break;
      }
      return ALUresult;
    }            
};


class INSMem
{
  public:
    bitset<32> Instruction;
    INSMem()
    {       IMem.resize(MemSize); 
      ifstream imem;
      string line;
      int i=0;
      imem.open("imem.txt");
      if (imem.is_open())
      {
        while (getline(imem,line))
        {      
          IMem[i] = bitset<8>(line);
          i++;
        }

      }
      else cout<<"Unable to open file";
      imem.close();

    }

    bitset<32> ReadMemory (bitset<32> ReadAddress) 
    {    
      // TODO: implement!
      /**
       * @brief Read Instruction Memory (IMem).
       *
       * Read the byte at the ReadAddress and the following three byte,
       * and return the read result. 
       */
      int first_byte = ReadAddress.to_ulong();
      string instruction = IMem[first_byte].to_string() + IMem[first_byte+1].to_string() + IMem[first_byte+2].to_string()+ IMem[first_byte+3].to_string();
      Instruction = bitset<32>(instruction);
      return Instruction;     
    }     

  private:
    vector<bitset<8> > IMem;

};

class DataMem    
{
  public:
    bitset<32> readdata;  
    DataMem()
    {
      DMem.resize(MemSize); 
      ifstream dmem;
      string line;
      int i=0;
      dmem.open("dmem.txt");
      if (dmem.is_open())
      {
        while (getline(dmem,line))
        {      
          DMem[i] = bitset<8>(line);
          i++;
        }
      }
      else cout<<"Unable to open file";
      dmem.close();

    }  
    bitset<32> MemoryAccess (bitset<32> Address, bitset<32> WriteData, bitset<1> readmem, bitset<1> writemem) 
    {    
      /**
       * @brief Reads/writes data from/to the Data Memory.
       *
       * This function is used to read/write data from/to the DataMem, depending on the readmem and writemem.
       * First, if writemem enabled, WriteData should be written to DMem, clear or ignore the return value readdata,
       * and note that 32-bit WriteData will occupy 4 continious Bytes in DMem. 
       * If readmem enabled, return the DMem read result as readdata.
       */
      // TODO: implement!
      // TWO FUNCTIONS ON MEMORY ACCESS: WRITE and READ
      // CASE: WRITE
      if (writemem.to_ulong() == 1 && readmem.to_ulong() == 0) {
        int address_byte = Address.to_ulong();
        string write_data_string = WriteData.to_string();
        string temp="";
        DMem[address_byte] = bitset<8>(slice(0,8,write_data_string));
        DMem[address_byte+1] = bitset<8>(slice(8,16,write_data_string));
        DMem[address_byte+2] = bitset<8>(slice(16,24,write_data_string));
        DMem[address_byte+3] = bitset<8>(slice(24,32,write_data_string));
        readdata = 0;        
      } 

      // CASE: READ           
      if (readmem.to_ulong() == 1 && writemem.to_ulong() == 0) {
        int address_byte = Address.to_ulong();
        string read_data = DMem[address_byte].to_string() + DMem[address_byte+1].to_string() + DMem[address_byte+2].to_string()+ DMem[address_byte+3].to_string();
        readdata = bitset<32>(read_data);
      } 

      return readdata;
    }   

    void OutputDataMem()
    {
      ofstream dmemout;
      dmemout.open("dmemresult.txt");
      if (dmemout.is_open())
      {
        for (int j = 0; j< 1000; j++)
        {     
          dmemout << DMem[j]<<endl;
        }

      }
      else cout<<"Unable to open file";
      dmemout.close();

    }             

  private:
    vector<bitset<8> > DMem;

};  



int main()
{
  RF myRF;
  ALU myALU;
  INSMem myInsMem;
  DataMem myDataMem;

  //initialized value (edited)
  int PC = 0;
  bitset<6> opcode;  
  bitset<5> rs;
  bitset<5> rt;
  bitset<5> rd;
  bitset<5> shamt;
  bitset<6> funct;
  bitset<3> ALUop;

  while (1)  // TODO: implement!
  {
    // Fetch: fetch an instruction from myInsMem.
    bitset<32> currentInstruction = myInsMem.ReadMemory(PC);

    // If current instruction is "11111111111111111111111111111111", then break; (exit the while loop)
    if (currentInstruction.to_string() == "11111111111111111111111111111111"){
      break;
    }

    // decode(Read RF): get opcode and other signals from instruction, decode instruction
    opcode = bitset<6>(slice(0,6,currentInstruction.to_string()));     //get opcode
    if (opcode.to_ulong() == 0) {                           // R- type
      rs = bitset<5>(slice(6,11,currentInstruction.to_string()));
      rt = bitset<5>(slice(11,16,currentInstruction.to_string()));
      rd = bitset<5>(slice(16,21,currentInstruction.to_string()));  
      shamt = bitset<5>(slice(21,26,currentInstruction.to_string()));
      funct = bitset<6>(slice(26,32,currentInstruction.to_string()));  
      // check function in R type
      if (funct.to_ulong() == 0) {                                // sll
        //ALUcode =
      }
      else if (funct.to_ulong() == 2) {                           // srl
        //ALUcode =
      }
      else if (funct.to_ulong() == 32) {                          // add
        //ALUcode =
      }
      else if (funct.to_ulong() == 34) {                          // sub
        //ALUcode =
      }
      else {
        //ALUcode =
      }

    }
    
    /*
    if (opcode.to_string() == "100011") {                   // isLoad

    }
    else if (opcode.to_string() == "101011") {              // isStore

    }
    else if (opcode.to_string() == "000000") {              // IsBranch

    }
    else if (opcode.to_string() == "000000") {              // R-type

    }
    else if (opcode.to_string() == "000010") {              // J-type

    }
    */


    // Execute: after decoding, ALU may run and return result

    // Read/Write Mem: access data memory (myDataMem)

    // Write back to RF: some operations may write things to RF


    PC = PC + 4;

    /**** You don't need to modify the following lines. ****/
    myRF.OutputRF(); // dump RF;    
  }
  myDataMem.OutputDataMem(); // dump data mem

  return 0;
}
