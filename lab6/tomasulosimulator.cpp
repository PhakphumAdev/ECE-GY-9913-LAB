#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <limits>
#include <queue>
using namespace std;
using std::cout;
using std::endl;
using std::string;
using std::vector;

string inputtracename = "trace.txt";
// remove the ".txt" and add ".out.txt" to the end as output name
string outputtracename = inputtracename.substr(0, inputtracename.length() - 4) + ".out.txt";
string hardwareconfigname = "config.txt";

enum Operation
{
	ADD,
	SUB,
	MULT,
	DIV,
	LOAD,
	STORE
};
// The execute cycle of each operation: ADD, SUB, MULT, DIV, LOAD, STORE
const int OperationCycle[6] = {2, 2, 10, 40, 2, 2};

struct HardwareConfig
{
	int LoadRSsize;	 // number of load reservation stations
	int StoreRSsize; // number of store reservation stations
	int AddRSsize;	 // number of add reservation stations
	int MultRSsize;	 // number of multiply reservation stations
	int FRegSize;	 // number of fp registers
};

// We use the following structure to record the time of each instruction
struct InstructionStatus
{
	int cycleIssued;
	int cycleExecuted; // execution completed
	int cycleWriteResult;
};

// Register Result Status structure
struct RegisterResultStatus
{
	string ReservationStationName;
	bool dataReady;
};

/*********************************** ↓↓↓ Todo: Implement by you ↓↓↓ ******************************************/
struct Instruction{
	Operation op;
	string dest,src1,src2;
	int imm;
	InstructionStatus status;
};
struct compareInstruction{
	bool operator() (Instruction const& i1, Instruction const& i2){
		return i1.status.cycleIssued > i2.status.cycleIssued;
	}
};
class RegisterResultStatuses
{
public:
	// ...
    RegisterResultStatuses(int numberOfRegisters) {
        for (int i = 0; i < numberOfRegisters; ++i) {
            _registers.push_back({"", false}); // Initialize with empty reservation station name and data not ready
        }
    }

	void updateStatus(int registerNumber, string reservationStationName, bool dataReady) {
        if (registerNumber >= 0 && registerNumber < _registers.size()) {
            _registers[registerNumber].ReservationStationName = reservationStationName;
            _registers[registerNumber].dataReady = dataReady;
        }
    }

  	RegisterResultStatus getStatus(int registerNumber) const {
        if (registerNumber >= 0 && registerNumber < _registers.size()) {
            return _registers[registerNumber];
        }
        return {"", false}; // Return default status if register number is out of range
    }

	void resetStatuses() {
        for (auto& reg : _registers) {
            reg = {"", false};
        }
    }
/*********************************** ↑↑↑ Todo: Implement by you ↑↑↑ ******************************************/
	/*
	Print all register result status. It is called by PrintRegisterResultStatus4Grade() for grading.
	If you don't want to write such a class, then make sure you call the same function and print the register
	result status in the same format.
	*/
	string _printRegisterResultStatus() const
	{
		std::ostringstream result;
		for (int idx = 0; idx < _registers.size(); idx++)
		{
			result << "F" + std::to_string(idx) << ": ";
			result << _registers[idx].ReservationStationName << ", ";
			result << "dataRdy: " << (_registers[idx].dataReady ? "Y" : "N") << ", ";
			result << "\n";
		}
		return result.str();
	}
/*********************************** ↓↓↓ Todo: Implement by you ↓↓↓ ******************************************/
private:
	vector<RegisterResultStatus> _registers;
};

// Define your Reservation Station structure
struct ReservationStation
{
	// ...
	string name;
	bool busy;
	Operation op;
	int vj,vk;
	int qj,qk;
	int remainCycle;
	Instruction *instruction;

	ReservationStation() : busy(false), remainCycle(0),instruction(nullptr) {}
};
class ReservationStations
{
public:
	// ...
	ReservationStations(int sizeLoad,int sizeStore, int sizeAdd, int sizeMult) {
        for(int i=0;i<sizeLoad;i++){
			string load = "Load";
			load+=to_string(i);	
			ReservationStation newOne = ReservationStation();
			newOne.name = load;
			_stations.push_back(newOne);
		}
		for(int i=0;i<sizeStore;i++){
			string store="Store";
			store+=to_string(i);
			ReservationStation newOne = ReservationStation();
			newOne.name = store;
			_stations.push_back(newOne);
		}
		for(int i=0;i<sizeAdd;i++){
			string add="Add";
			add+=to_string(i);
			ReservationStation newOne = ReservationStation();
			newOne.name = add;
			_stations.push_back(newOne);
		}
		for(int i=0;i<sizeMult;i++){
			string mult="Mult";
			mult+=to_string(i);
			ReservationStation newOne = ReservationStation();
			newOne.name = mult;
			_stations.push_back(newOne);
		}
    }

private:
	vector<ReservationStation> _stations;
};

class CommonDataBus
{
public:
	void addQ(Instruction instruction){
		pq.push(instruction);
	}
	Instruction getTop(){
		return pq.top();
	}
	Instruction pop(){
		Instruction ret = pq.top();
		pq.pop();
		return ret;
	}
private:
	priority_queue<Instruction,vector<Instruction>,compareInstruction> pq;
};

// Function to simulate the Tomasulo algorithm
void simulateTomasulo()
{

	int thiscycle = 1; // start cycle: 1
	// RegisterResultStatuses registerResultStatus(10);

	while (thiscycle < 100000000)
	{

		// Reservation Stations should be updated every cycle, and broadcast to Common Data Bus
		// ...

		// Issue new instruction in each cycle
		// ...

		// At the end of this cycle, we need this function to print all registers status for grading
		// PrintRegisterResultStatus4Grade(outputtracename, registerResultStatus, thiscycle);

		++thiscycle;

		// The simulator should stop when all instructions are finished.
		// ...
	}
};

/*********************************** ↑↑↑ Todo: Implement by you ↑↑↑ ******************************************/

/*
print the instruction status, the reservation stations and the register result status
@param filename: output file name
@param instructionStatus: instruction status
*/
void PrintResult4Grade(const string &filename, const vector<InstructionStatus> &instructionStatus)
{
	std::ofstream outfile(filename, std::ios_base::app); // append result to the end of file
	outfile << "Instruction Status:\n";
	for (int idx = 0; idx < instructionStatus.size(); idx++)
	{
		outfile << "Instr" << idx << ": ";
		outfile << "Issued: " << instructionStatus[idx].cycleIssued << ", ";
		outfile << "Completed: " << instructionStatus[idx].cycleExecuted << ", ";
		outfile << "Write Result: " << instructionStatus[idx].cycleWriteResult << ", ";
		outfile << "\n";
	}
	outfile.close();
}

/*
print the register result status each 5 cycles
@param filename: output file name
@param registerResultStatus: register result status
@param thiscycle: current cycle
*/
void PrintRegisterResultStatus4Grade(const string &filename,
									 const RegisterResultStatuses &registerResultStatus,
									 const int thiscycle)
{
	if (thiscycle % 5 != 0)
		return;
	std::ofstream outfile(filename, std::ios_base::app); // append result to the end of file
	outfile << "Cycle " << thiscycle << ":\n";
	outfile << registerResultStatus._printRegisterResultStatus() << "\n";
	outfile.close();
}

int main(int argc, char **argv)
{
	if (argc > 1)
	{
		hardwareconfigname = argv[1];
		inputtracename = argv[2];
	}

	HardwareConfig hardwareConfig;
	std::ifstream config;
	config.open(hardwareconfigname);
	config >> hardwareConfig.LoadRSsize;  // number of load reservation stations
	config >> hardwareConfig.StoreRSsize; // number of store reservation stations
	config >> hardwareConfig.AddRSsize;	  // number of add reservation stations
	config >> hardwareConfig.MultRSsize;  // number of multiply reservation stations
	config >> hardwareConfig.FRegSize;	  // number of fp registers
	config.close();

/*********************************** ↓↓↓ Todo: Implement by you ↓↓↓ ******************************************/

	// Read instructions from a file (replace 'instructions.txt' with your file name)
	// ...
	std::ifstream trace(inputtracename);
	vector<Instruction>instructions;
	string op,dest,src1,src2;
	while(trace >> op >> dest >> src1 >> src2){
		// cout << op << ' ' << dest << ' ' << src1 << ' ' << src2 << "\n";
			Instruction dummy;
			dummy.imm = -1;
			dummy.dest = dest;
			dummy.status.cycleExecuted = -1;
			dummy.status.cycleIssued = -1;
			dummy.status.cycleWriteResult = -1;
			if(op=="LOAD"){
				dummy.op = LOAD;
				dummy.imm = stoi(src1);
			}
			if(op=="STORE"){
				dummy.op = STORE;
				dummy.imm = stoi(src1);
			}
			if(op=="ADD"){
				dummy.op = ADD;
			}
			if(op=="SUB"){
				dummy.op = SUB;
			}
			if(op=="MULT"){
				dummy.op = MULT;
			}
			if(op=="DIV"){
				dummy.op = DIV;
			}
			if(dummy.imm==-1){
				dummy.src1 = src1;
				dummy.src2 = src2;
			}
			instructions.push_back(dummy);
	}
	// Initialize the register result status
	// RegisterResultStatuses registerResultStatus();
	// ...
	RegisterResultStatuses registerResultStatus(hardwareConfig.FRegSize);

	// Initialize the instruction status table
	vector<InstructionStatus> instructionStatus(instructions.size());
	// ...
	for(int i=0;i<instructions.size();i++){
		instructionStatus[i].cycleIssued = -1;      
    	instructionStatus[i].cycleExecuted = -1;     
    	instructionStatus[i].cycleWriteResult = -1; 
	}
	// Initialize the reservation stations
	ReservationStations reservationStations(hardwareConfig.LoadRSsize,hardwareConfig.StoreRSsize,hardwareConfig.AddRSsize,hardwareConfig.MultRSsize);
	// Simulate Tomasulo:
	// simulateTomasulo(registerResultStatus, instructionStatus, ...);

/*********************************** ↑↑↑ Todo: Implement by you ↑↑↑ ******************************************/

	// At the end of the program, print Instruction Status Table for grading
	PrintResult4Grade(outputtracename, instructionStatus);

	return 0;
}
