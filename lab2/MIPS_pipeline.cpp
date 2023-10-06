#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;

#define MemSize 1000 // memory size, in reality, the memory size should be 2^32, but for this lab csa23, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.

struct IFStruct {
    bitset<32>  PC;
    bool        nop;  
};

struct IDStruct {
    bitset<32>  Instr;
    bool        nop;  
};

struct EXStruct {
    bitset<32>  Read_data1;
    bitset<32>  Read_data2;
    bitset<16>  Imm;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        is_I_type;
    bool        rd_mem;
    bool        wrt_mem; 
    bool        alu_op;     //1 for addu, lw, sw, 0 for subu 
    bool        wrt_enable;
    bool        nop;  
};

struct MEMStruct {
    bitset<32>  ALUresult;
    bitset<32>  Store_data;
    bitset<5>   Rs;
    bitset<5>   Rt;    
    bitset<5>   Wrt_reg_addr;
    bool        rd_mem;
    bool        wrt_mem; 
    bool        wrt_enable;    
    bool        nop;    
};

struct WBStruct {
    bitset<32>  Wrt_data;
    bitset<5>   Rs;
    bitset<5>   Rt;     
    bitset<5>   Wrt_reg_addr;
    bool        wrt_enable;
    bool        nop;     
};

struct stateStruct {
    IFStruct    IF;
    IDStruct    ID;
    EXStruct    EX;
    MEMStruct   MEM;
    WBStruct    WB;
};

string slice(int start,int end,string input){
  string ret="";
  for(int i=start;i<end;i++){
    ret+=input[i];
  }
  return ret;
}

bitset<32>signExtend(bitset<16>bit){
  string extended = bit.to_string();
  string temp = "";
  char sign_bit = extended[0];
  while(temp.length()!=16){
    temp+=sign_bit;
  }
  string res = temp+extended;
  bitset<32>ret (res);
  return ret;
}

class RF
{
    public: 
        bitset<32> Reg_data;
     	RF()
    	{ 
			Registers.resize(32);  
			Registers[0] = bitset<32> (0);  
        }
	
        bitset<32> readRF(bitset<5> Reg_addr)
        {   
            Reg_data = Registers[Reg_addr.to_ulong()];
            return Reg_data;
        }
    
        void writeRF(bitset<5> Reg_addr, bitset<32> Wrt_reg_data)
        {
            Registers[Reg_addr.to_ulong()] = Wrt_reg_data;
        }
		 
		void outputRF()
		{
			ofstream rfout;
			rfout.open("RFresult.txt",std::ios_base::app);
			if (rfout.is_open())
			{
				rfout<<"State of RF:\t"<<endl;
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

class INSMem
{
	public:
        bitset<32> Instruction;
        INSMem()
        {       
			IMem.resize(MemSize); 
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
                  
		bitset<32> readInstr(bitset<32> ReadAddress) 
		{    
			string insmem;
			insmem.append(IMem[ReadAddress.to_ulong()].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+1].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+2].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+3].to_string());
			Instruction = bitset<32>(insmem);		//read instruction memory
			return Instruction;     
		}     
      
    private:
        vector<bitset<8> > IMem;     
};
      
class DataMem    
{
    public:
        bitset<32> ReadData;  
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
		
        bitset<32> readDataMem(bitset<32> Address)
        {	
			string datamem;
            datamem.append(DMem[Address.to_ulong()].to_string());
            datamem.append(DMem[Address.to_ulong()+1].to_string());
            datamem.append(DMem[Address.to_ulong()+2].to_string());
            datamem.append(DMem[Address.to_ulong()+3].to_string());
            ReadData = bitset<32>(datamem);		//read data memory
            return ReadData;               
		}
            
        void writeDataMem(bitset<32> Address, bitset<32> WriteData)            
        {
            DMem[Address.to_ulong()] = bitset<8>(WriteData.to_string().substr(0,8));
            DMem[Address.to_ulong()+1] = bitset<8>(WriteData.to_string().substr(8,8));
            DMem[Address.to_ulong()+2] = bitset<8>(WriteData.to_string().substr(16,8));
            DMem[Address.to_ulong()+3] = bitset<8>(WriteData.to_string().substr(24,8));  
        }   
                     
        void outputDataMem()
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

void printState(stateStruct state, int cycle)
{
    ofstream printstate;
    printstate.open("stateresult.txt", std::ios_base::app);
    if (printstate.is_open())
    {
        printstate<<"State after executing cycle:\t"<<cycle<<endl; 
        
        printstate<<"IF.PC:\t"<<state.IF.PC.to_ulong()<<endl;        
        printstate<<"IF.nop:\t"<<state.IF.nop<<endl; 
        
        printstate<<"ID.Instr:\t"<<state.ID.Instr<<endl; 
        printstate<<"ID.nop:\t"<<state.ID.nop<<endl;
        
        printstate<<"EX.Read_data1:\t"<<state.EX.Read_data1<<endl;
        printstate<<"EX.Read_data2:\t"<<state.EX.Read_data2<<endl;
        printstate<<"EX.Imm:\t"<<state.EX.Imm<<endl; 
        printstate<<"EX.Rs:\t"<<state.EX.Rs<<endl;
        printstate<<"EX.Rt:\t"<<state.EX.Rt<<endl;
        printstate<<"EX.Wrt_reg_addr:\t"<<state.EX.Wrt_reg_addr<<endl;
        printstate<<"EX.is_I_type:\t"<<state.EX.is_I_type<<endl; 
        printstate<<"EX.rd_mem:\t"<<state.EX.rd_mem<<endl;
        printstate<<"EX.wrt_mem:\t"<<state.EX.wrt_mem<<endl;        
        printstate<<"EX.alu_op:\t"<<state.EX.alu_op<<endl;
        printstate<<"EX.wrt_enable:\t"<<state.EX.wrt_enable<<endl;
        printstate<<"EX.nop:\t"<<state.EX.nop<<endl;        

        printstate<<"MEM.ALUresult:\t"<<state.MEM.ALUresult<<endl;
        printstate<<"MEM.Store_data:\t"<<state.MEM.Store_data<<endl; 
        printstate<<"MEM.Rs:\t"<<state.MEM.Rs<<endl;
        printstate<<"MEM.Rt:\t"<<state.MEM.Rt<<endl;   
        printstate<<"MEM.Wrt_reg_addr:\t"<<state.MEM.Wrt_reg_addr<<endl;              
        printstate<<"MEM.rd_mem:\t"<<state.MEM.rd_mem<<endl;
        printstate<<"MEM.wrt_mem:\t"<<state.MEM.wrt_mem<<endl; 
        printstate<<"MEM.wrt_enable:\t"<<state.MEM.wrt_enable<<endl;         
        printstate<<"MEM.nop:\t"<<state.MEM.nop<<endl;        

        printstate<<"WB.Wrt_data:\t"<<state.WB.Wrt_data<<endl;
        printstate<<"WB.Rs:\t"<<state.WB.Rs<<endl;
        printstate<<"WB.Rt:\t"<<state.WB.Rt<<endl;        
        printstate<<"WB.Wrt_reg_addr:\t"<<state.WB.Wrt_reg_addr<<endl;
        printstate<<"WB.wrt_enable:\t"<<state.WB.wrt_enable<<endl;        
        printstate<<"WB.nop:\t"<<state.WB.nop<<endl; 
    }
    else cout<<"Unable to open file";
    printstate.close();
}
 

int main()
{
    
    RF myRF;
    INSMem myInsMem;
    DataMem myDataMem;
    bitset<6> opcode;  
    bitset<6>funct;
    int cycle = 0;
    //Initialize structs
    stateStruct state={0}; //initialize everything inside to be 0
    stateStruct newState={0};// same^

    //Initialize first state
    state.IF.PC = 0;
    state.IF.nop = 0;
    state.ID.nop = 1;
    state.EX.nop = 1;
    state.MEM.nop = 1;
    state.WB.nop = 1;

             
    while (1) {

        /* --------------------- WB stage --------------------- */
        if (state.WB.nop != 1) {
            if (state.WB.wrt_enable == 1) {
                myRF.writeRF(state.WB.Wrt_reg_addr, state.WB.Wrt_data);
            }                                 
        }
        else {
            newState.WB.nop = 1;
        }

        /* --------------------- MEM stage --------------------- */
        if (state.MEM.nop != 1) {
            newState.WB.nop = 0;
            newState.WB.Wrt_data = state.MEM.ALUresult;
            newState.WB.Rs = state.MEM.Rs;  
            newState.WB.Rt = state.MEM.Rt;
            newState.WB.Wrt_reg_addr = state.MEM.Wrt_reg_addr;
            newState.WB.wrt_enable = state.MEM.wrt_enable;

            if (state.MEM.rd_mem == 1) {        //lw
                newState.WB.Wrt_data = myDataMem.readDataMem(state.MEM.ALUresult);
            }
            if (state.MEM.wrt_mem == 1) {       //sw
                myDataMem.writeDataMem(state.MEM.ALUresult, state.MEM.Store_data);
            }
        }       
        else {
            newState.MEM.nop = 1;
            newState.WB.nop = 1;
        }

        /* --------------------- EX stage --------------------- */
        if (state.EX.nop != 1) {
            newState.MEM.nop = 0;
            newState.MEM.ALUresult = 0;
            newState.MEM.Store_data = 0;
            newState.MEM.Rs = state.EX.Rs;
            newState.MEM.Rt = state.EX.Rt;
            newState.MEM.Wrt_reg_addr = state.EX.Wrt_reg_addr;
            newState.MEM.wrt_enable = 0;  
            newState.MEM.rd_mem = 0;
            newState.MEM.wrt_mem = 0;

            //check RAW hazard and forward values
            if (state.EX.Rs == newState.WB.Wrt_reg_addr) {
                state.EX.Read_data1 = newState.WB.Wrt_data;
            }
            if (state.EX.Rt == newState.WB.Wrt_reg_addr) {
                state.EX.Read_data2 = newState.WB.Wrt_data;
            }


            if (state.EX.is_I_type == 0) {    // R-type
                newState.MEM.wrt_enable = 1;
                if (state.EX.alu_op == 1) {
                    newState.MEM.ALUresult = bitset<32> (state.EX.Read_data1.to_ulong() + state.EX.Read_data2.to_ulong());
                }
                else {
                    newState.MEM.ALUresult = bitset<32> (state.EX.Read_data1.to_ulong() - state.EX.Read_data2.to_ulong());
                }             
            }  
            else {                                  // I-type
                newState.MEM.ALUresult = bitset<32> (state.EX.Read_data1.to_ulong() + signExtend(state.EX.Imm).to_ulong() );
                if (state.EX.rd_mem == 1) {         //lw
                    newState.MEM.wrt_enable = 1;
                    newState.MEM.rd_mem = 1;
                }
                else if (state.EX.wrt_mem == 1) {   //sw
                    newState.MEM.Store_data = state.EX.Read_data2;
                    newState.MEM.wrt_mem = 1;
                }
                else {                              //bnq
                }
            }

        }               
        else {
            newState.EX.nop = 1;
            newState.MEM.nop = 1;
        }

        /* --------------------- ID stage --------------------- */
        if (state.IF.nop != 1) {
            if (state.ID.Instr.to_string() == "11111111111111111111111111111111") { //halt
                state.IF.nop = 1;
                newState.ID.nop = 1;
                newState.EX.nop = 1;
            }
            else{
                newState.EX.nop = 0;
                opcode = bitset<6>(slice(0,6,state.ID.Instr.to_string())); 
                newState.EX.Rs = bitset<5>(slice(6,11,state.ID.Instr.to_string()));
                newState.EX.Rt = bitset<5>(slice(11,16,state.ID.Instr.to_string()));
                newState.EX.Read_data1 =  myRF.readRF(bitset<5>(slice(6,11,state.ID.Instr.to_string()))); //Rs
                newState.EX.Read_data2 =  myRF.readRF(bitset<5>(slice(11,16,state.ID.Instr.to_string()))); //Rt
                newState.EX.Wrt_reg_addr = bitset<5>(slice(16,21,state.ID.Instr.to_string()));  //rd   
                newState.EX.Imm = bitset<16>(slice(16,32,state.ID.Instr.to_string()));          //Immediate
                newState.EX.is_I_type = 0;
                newState.EX.wrt_enable = 0;
                newState.EX.rd_mem = 0;
                newState.EX.wrt_mem = 0;
                newState.EX.alu_op = 1;

                if (opcode.to_ulong() == 0) {  // R-type                                    
                    newState.EX.wrt_enable = 1;
                    funct = bitset<6>(slice(26,32,state.ID.Instr.to_string()));  
                    if (funct.to_ulong() == 35) {   //subs                 
                        newState.EX.alu_op = 0;                        
                    }                  
                }
                else {      // I-type
                    newState.EX.is_I_type = 1;                                                       
                    if (opcode.to_ulong() == 35) {  //lw
                        newState.EX.wrt_enable = 1; 
                        newState.EX.rd_mem = 1;
                        newState.EX.Wrt_reg_addr = bitset<5>(slice(11,16,state.ID.Instr.to_string())); // Rt instead of Rd
                    }
                    else if (opcode.to_ulong() == 43) {     //sw
                        newState.EX.wrt_mem = 1;
                    }
                    else if (opcode.to_ulong() == 5) {      //bne
                        if (myRF.readRF(bitset<5>(slice(6,11,state.ID.Instr.to_string()))) != myRF.readRF(bitset<5>(slice(11,16,state.ID.Instr.to_string())))) {
                            bitset<32>BranchAddr = signExtend(bitset<16>(slice(16,32,state.ID.Instr.to_string())));
                            string temp = BranchAddr.to_string();
                            temp[30] = '0';
                            temp[31] = '0';
                            BranchAddr = bitset<32>(temp);
                            newState.IF.PC = bitset<32>(state.IF.PC.to_ulong() + 4 + BranchAddr.to_ulong());                              
                        }
                    }
                }    
        
            }

        }
        else {
            newState.ID.nop = 1;
            newState.EX.nop = 1;
        }
        
        /* --------------------- IF stage --------------------- */
        if (state.IF.nop != 1) {
            newState.ID.Instr = myInsMem.readInstr(state.IF.PC);
            newState.ID.nop = 0;
            newState.IF.PC =  state.IF.PC.to_ulong() + 4;
        }
        else {
            newState.IF.nop = 1;
            newState.ID.nop = 1;
        }

             
        if (state.IF.nop && state.ID.nop && state.EX.nop && state.MEM.nop && state.WB.nop)
            break;
        
        printState(newState, cycle); //print states after executing cycle 0, cycle 1, cycle 2 ... 
       
        state = newState; /*** The end of the cycle and updates the current state with the values calculated in this cycle. csa23 ***/ 

        cycle++;                	
    }
    
    myRF.outputRF(); // dump RF;	
	myDataMem.outputDataMem(); // dump data mem 
	
	return 0;
}
