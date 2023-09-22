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

// MIPS is big endian MSB is stored at the lowest address
string slice(int start,int end,string input){
  string ret="";
  for(int i=start;i<end;i++){
    ret+=input[i];
  }
  return ret;
}
bitset<32>signExtend(bitset<16>bit){
  // string extended = bit.to_string();
  // string temp = bit.to_string();
  // while(temp.length()!=32){
  //   temp+=extended[15];
  // }
  // bitset<32>result(temp);
  // return result;
  string extended = bit.to_string();
  string temp = "";
  while(temp.length()!=16){
    temp+=extended[0];
  }
  string res = temp+extended;
  bitset<32>ret (res);
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
        Registers[WrtReg.to_ulong()] = bitset<32>(0);        
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
        ALUresult = oprand1| oprand2;break;
        case NOR: ALUresult = ~(oprand1|oprand2);
        break;
      }
      cout << "oprand1:" << oprand1 <<"\n";
      cout << "oprand2:" << oprand2 <<"\n";
      cout << "inside ALU ALUresult:" << ALUresult << "\n";
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
  bitset<32>Instruction;
  bitset<32> PC = 0;
  bitset<6> opcode;  
  bitset<5> rs;
  bitset<5> rt;
  bitset<5> rd;
  bitset<5> shamt;
  bitset<6> funct;
  bitset<16> immediate;
  bitset<3> ALUop;       //ALUop: ADDU (1), SUBU (3), AND (4), OR  (5), NOR (7), doNothing (6)  
  bitset<26> address;
  bitset<32> BranchAddr;
  bitset<32> JumpAddr;
  int instructionType;    //Type: R-type(0), I-type(1), J-type(2)
  bitset<32>tempPC;
  while (1)  // TODO: implement!
  {
    cout << "\nStart each iteration";
    // Fetch: fetch an instruction from myInsMem.
    Instruction = myInsMem.ReadMemory(PC);
    cout << "\nInstruction:" << Instruction;

    // If current instruction is "11111111111111111111111111111111", then break; (exit the while loop)
    if (Instruction.to_string() == "11111111111111111111111111111111"){
      break;
    }
    tempPC = PC;
    // decode(Read RF): get opcode and other signals from instruction, decode instruction
    opcode = bitset<6>(slice(0,6,Instruction.to_string()));        
    if (opcode.to_ulong() == 0) {                                         // R-type
      rs = bitset<5>(slice(6,11,Instruction.to_string()));
      rt = bitset<5>(slice(11,16,Instruction.to_string()));
      rd = bitset<5>(slice(16,21,Instruction.to_string()));  
      shamt = bitset<5>(slice(21,26,Instruction.to_string()));
      funct = bitset<6>(slice(26,32,Instruction.to_string()));          
      instructionType = 0;
      cout << "R-type" << rs,rt,rd,shamt,funct;
    }
    else if ((opcode.to_ulong() == 2) || (opcode.to_ulong() == 3)){         // j & jal, J-type
      address = bitset<26>(slice(6,32,Instruction.to_string()));
      instructionType = 2;
      cout << "J-type" << address;
    }  
    else{                                                                // I-type
      rs = bitset<5>(slice(6,11,Instruction.to_string()));
      rt = bitset<5>(slice(11,16,Instruction.to_string()));
      immediate = bitset<16>(slice(16,32,Instruction.to_string()));
      instructionType = 1;
      cout << "I-type" << rs,rt,immediate;
    }

    // Execute: after decoding, ALU may run and return result
    /*
      // check function in R type
      if (funct.to_ulong() == 0) {                 // sll
      else if (funct.to_ulong() == 2) {            // srl
      else if (funct.to_ulong() == 8) {            // jr
      else if (funct.to_ulong() == 18) {           // mult
      else if (funct.to_ulong() == 19) {           // multu
      else if (funct.to_ulong() == 26) {           // div
      else if (funct.to_ulong() == 27) {           // divu
      else if (funct.to_ulong() == 32) {           // add
      else if (funct.to_ulong() == 33) {           // addu
      else if (funct.to_ulong() == 34) {           // sub
      else if (funct.to_ulong() == 35) {           // subu
      else if (funct.to_ulong() == 36) {           // and
      else if (funct.to_ulong() == 37) {            // or
      else if (funct.to_ulong() == 39) {            // nor
      // check opcode in I type
      if (opcode.to_ulong() == 4) {                // beq
      else if (opcode.to_ulong() == 5) {            // bneq
      else if (opcode.to_ulong() == 8) {            // addi
      else if (opcode.to_ulong() == 9) {            // addiu
      else if (opcode.to_ulong() == 12) {            // andi
      else if (opcode.to_ulong() == 35) {            // lw
      else if (opcode.to_ulong() == 43) {            // sw
    */
    if (instructionType == 0) {                                             // R-type
      // if ((funct.to_ulong() == 0) || (funct.to_ulong() == 2)) { // sll & srl
      //   myRF.ReadWrite(rt, 0, 0, 0, 0);
      // }
      // else if (funct.to_ulong() == 8) {             //jr
      //   myRF.ReadWrite(31, 0, 0, 0, 0); //read $rs
      //   PC = bitset<32> (myRF.ReadData1); 
      // }
      // else {                                         // other R-type
        ALUop = bitset<3>(slice(3,6,funct.to_string()));
        myRF.ReadWrite(rs, rt, 0, 0, 0);
        myALU.ALUOperation (ALUop, myRF.ReadData1, myRF.ReadData2);
      // }      
    }
    else if (instructionType == 1) {                                        // I-type
      if (opcode.to_ulong() == 4) {                // beq
        myRF.ReadWrite(rs, rt, 0, 0, 0);
        // myRF.ReadWrite(0, rt, 0, 0, 0);
        if (myRF.ReadData1.to_ulong() == myRF.ReadData2.to_ulong()) {     //if equal branch to PC + 4 + BranchAddress
          // string temp = "00"+immediate.to_string();
          // string dummy = immediate.to_string();
          // while(temp.length()!=32){
          //   temp+=dummy[15];
          // }
          BranchAddr = signExtend(immediate);
          string temp = BranchAddr.to_string();
          temp[30] = '0';
          temp[31] = '0';
          BranchAddr = bitset<32>(temp);
          PC = bitset<32>(PC.to_ulong() + 4 + BranchAddr.to_ulong());   //แก้แล้ว ไม่แน่ใจ
        }
        else {                            // if not equal go to PC + 4
          PC = bitset<32> (PC.to_ulong()+4);
        }
      }
      // else if (opcode.to_ulong() == 5) {            // bneq
      //   myRF.ReadWrite(rs, rt, 0, 0, 0);
      //   if (myRF.ReadData1.to_ulong() != myRF.ReadData2.to_ulong()) {     //if not equal branch to PC + 4 + BranchAddress
      //     BranchAddr = signExtend(immediate);
      //     string temp = BranchAddr.to_string();
      //     temp[30] = '0';
      //     temp[31] = '0';
      //     BranchAddr = bitset<32>(temp);
      //     PC = bitset<32>(PC.to_ulong() + 4 + BranchAddr.to_ulong());   //แก้แล้ว ไม่แน่ใจ
      //   }
      //   else {                            // if equal go to PC + 4
      //     PC = bitset<32> (PC.to_ulong()+4);
      //   }
      // }
      else if (opcode.to_ulong() == 8) {            // addi
        myRF.ReadWrite(rs, 0, 0, 0, 0);
        ALUop = bitset<3>(1);
        myALU.ALUOperation (ALUop, myRF.ReadData1, signExtend(immediate)); //บรรทัดนี้ผิด ต้องเป็น SignExtImm
      }
      else if (opcode.to_ulong() == 9) {            // addiu
        myRF.ReadWrite(rs, 0, 0, 0, 0);
        ALUop = bitset<3>(1);
        myALU.ALUOperation (ALUop, myRF.ReadData1, signExtend(immediate)); //บรรทัดนี้ผิด ต้องเป็น SignExtImm
      }
      else if (opcode.to_ulong() == 35) {            // lw
        myRF.ReadWrite(rs, 0, 0, 0, 0);
        ALUop = bitset<3> (1);    //add
        myALU.ALUOperation (ALUop, myRF.ReadData1, signExtend(immediate)); 
      }
      else if (opcode.to_ulong() == 43) {            // sw
        myRF.ReadWrite(rs, rt, 0, 0, 0);
        ALUop = bitset<3> (1);    //add
        myALU.ALUOperation (ALUop, myRF.ReadData1, signExtend(immediate)); //บรรทัดนี้ผิด ต้องเป็น SignExtImm
      }

    }
    else if (instructionType == 2) {                                        // J-type
      // if (opcode.to_ulong() == 2) {                  // j
        address = bitset<26>(slice(6,32,Instruction.to_string()));
        PC = bitset<32>((slice(0,4,bitset<32>((PC.to_ulong()+4)).to_string()))+address.to_string()+ "00");         // set next PC address to [0:4]PC + [5:30]address + 00    //ฝากแก้แหน่ กุงง
      // }
      // else if (opcode.to_ulong() == 2) {             // jal
      //   myRF.ReadWrite(0, 0, 31, PC.to_ulong() + 4, 1);                  // save the next PC instruction to register 31  ($ra)
      //   address = bitset<26>(slice(6,32,Instruction.to_string()));
      //   PC = bitset<32>((slice(0,4,bitset<32>((PC.to_ulong()+4)).to_string()))+address.to_string()+ "00");         // set next PC address to [0:4]PC + [5:30]address + 00    //ฝากแก้แหน่ กุงง
      // }
    }

    // Read/Write Mem: access data memory (myDataMem)
    if (instructionType == 0) {                                             // R-type  
      //do nothing
    }
    else if (instructionType == 1) {                                        // I-type
      if (opcode.to_ulong() == 35) {            // lw
        myDataMem.MemoryAccess(myALU.ALUresult, 0, 1, 0); 
        cout << "\nread DMem at" << myRF.ReadData1;
        cout << "\nDMem = " << myDataMem.readdata;
      }
      else if (opcode.to_ulong() == 43) {            // sw
        // myDataMem.MemoryAccess (myALU.ALUresult, rt, 0, 1);
        myDataMem.MemoryAccess(myALU.ALUresult,myRF.ReadData2,0,1); // <- double
      }
    }
    else if (instructionType == 2) {                                        // J-type
      //do nothing
    }

    // Write back to RF: some operations may write things to RF
    if (instructionType == 0) {                                             // R-type    
      // if (funct.to_ulong() == 0) {      //sll
      //   myRF.ReadWrite(0, 0, rd, myRF.ReadData1<<shamt.to_ulong(), 1);
      // }
      // else if (funct.to_ulong() == 2) { // srl
      //   myRF.ReadWrite(0, 0, rd, myRF.ReadData1>>shamt.to_ulong(), 1);
      // }
        myRF.ReadWrite(0, 0, rd, myALU.ALUresult, 1);
      // }      
    }
    else if (instructionType == 1) {                                        // I-type
      if (opcode.to_ulong() == 4) {                // beq
        // do nothing
      }
      else if (opcode.to_ulong() == 5) {            // bneq
        // do nothing
      }
      else if (opcode.to_ulong() == 8) {            // addi
        myRF.ReadWrite(0, 0, rt, myALU.ALUresult, 1);
      }
      else if (opcode.to_ulong() == 9) {            // addiu
        myRF.ReadWrite(0, 0, rt, myALU.ALUresult, 1);
      }
      else if (opcode.to_ulong() == 35) {            // lw
        myRF.ReadWrite(0, 0, rt, myDataMem.readdata, 1);
        cout << "write back to RF<" << "\n";
        cout << "ALUresult :" << myALU.ALUresult << "\n";
      }
      else if (opcode.to_ulong() == 43) {            // sw
        // do nothing
      }
    }
    else if (instructionType == 2) {                                        // J-type
      //do nothing
    }
    
    
    if(tempPC == PC){
     // if PC has not changed at all during the while loop or not Jump/Jal/Enq, we move to the next instruction
      PC = bitset<32>(PC.to_ulong() + 4);
    }
    /**** You don't need to modify the following lines. ****/
    myRF.OutputRF(); // dump RF;    
  }
  myDataMem.OutputDataMem(); // dump data mem

  return 0;
}
