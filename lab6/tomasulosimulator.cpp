#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <limits>
#include <queue>
#include <string>
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
	int dest,src1,src2;
	int imm;
	InstructionStatus status;
};
class RegisterResultStatuses
{
public:
	// ...
    RegisterResultStatuses(int numberOfRegisters) {
        for (int i = 0; i < numberOfRegisters; ++i) {
			RegisterResultStatus dummy;
			dummy.ReservationStationName = "";
			dummy.dataReady = false;
            _registers.push_back(dummy); // Initialize with empty reservation station name and data not ready
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
		RegisterResultStatus dummy;
		dummy.ReservationStationName = "";
		dummy.dataReady = false;
        return dummy; // Return default status if register number is out of range
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
	Operation name;
	string nameString;
	int stationNumber;
	bool busy;
	Operation op;
	int vj,vk;
	string qj,qk;
	bool inQ;
	int remainCycle;
	int issuedCycle;
	int cycleExecuted;
	int numInstruction;
	ReservationStation() : busy(false),inQ(false),qj(""),qk(""), remainCycle(-1) {}
};
struct compareReservationStation{
	bool operator() (ReservationStation const& r1, ReservationStation const& r2){
		return r1.issuedCycle > r2.issuedCycle;
	}
};
class CommonDataBus
{
public:
	void addQ(ReservationStation r){
		pq.push(r);
	}
	ReservationStation getTop(){
		return pq.top();
	}
	ReservationStation pop(){
		ReservationStation ret = pq.top();
		pq.pop();
		return ret;
	}
	bool isEmpty(){
		if(pq.empty()){
			return true;
		}
		return false;
	}
private:
	priority_queue<ReservationStation,vector<ReservationStation>,compareReservationStation> pq;
};class ReservationStations
{
public:
	// ...
	ReservationStations(int sizeLoad,int sizeStore, int sizeAdd, int sizeMult) {
        for(int i=0;i<sizeLoad;i++){
			string load = "Load";
			load+=to_string(i);
			ReservationStation newOne = ReservationStation();
			newOne.name = LOAD;
			newOne.stationNumber = i;
			newOne.nameString = load;
			newOne.inQ = false;
			_stations.push_back(newOne);
		}
		for(int i=0;i<sizeStore;i++){
			string store = "Store";
			store+=to_string(i);
			ReservationStation newOne = ReservationStation();
			newOne.name = STORE;
			newOne.stationNumber = i;
			newOne.inQ = false;
			newOne.nameString = store;
			_stations.push_back(newOne);
		}
		for(int i=0;i<sizeAdd;i++){
			string add = "Add";
			add+=to_string(i);
			ReservationStation newOne = ReservationStation();
			newOne.name = ADD;
			newOne.stationNumber = i;
			newOne.nameString = add;
			newOne.inQ = false;
			_stations.push_back(newOne);
		}
		for(int i=0;i<sizeMult;i++){
			string mult = "Mult";
			mult+=to_string(i);		
			ReservationStation newOne = ReservationStation();
			newOne.name = MULT;
			newOne.stationNumber = i;
			newOne.nameString = mult;
			newOne.inQ = false;
			_stations.push_back(newOne);
		}
    }
	bool isFreeStationAvailable(Operation instruction) {
		if(instruction==SUB){
			instruction = ADD;
		}
		if(instruction==DIV){
			instruction = MULT;
		}
		for(int i=0;i<_stations.size();i++){
			if(_stations[i].name==instruction && !_stations[i].busy){
				return true;
			}
		}
		return false;
	}
	void addInstruction(Instruction instruction,int issuedCycle,RegisterResultStatuses &currentRegisterResultStatuses,int numInstruction){
		for(int i=0;i<_stations.size();i++){
			if( !_stations[i].busy&&(_stations[i].name == instruction.op || (instruction.op == SUB && _stations[i].name == ADD) || (instruction.op == DIV && _stations[i].name == MULT))){
				_stations[i].busy = true;
				_stations[i].op = instruction.op;
				_stations[i].remainCycle = OperationCycle[instruction.op];
				_stations[i].issuedCycle = issuedCycle;
				_stations[i].numInstruction = numInstruction;
				_stations[i].inQ = false;
				if(instruction.op != LOAD && instruction.op != STORE){
					//check if operands data is avaliable 
					RegisterResultStatus src1 = currentRegisterResultStatuses.getStatus(instruction.src1);
					RegisterResultStatus src2 = currentRegisterResultStatuses.getStatus(instruction.src2);
					if(src1.dataReady){
						_stations[i].vj = 1;// don't care value
					}
					else{
						_stations[i].qj = src1.ReservationStationName;
					}
					if(src2.dataReady){
						_stations[i].vk = 1; // don't care value
					}
					else{
						_stations[i].qk = src2.ReservationStationName;
					}
				}
				else{
					// LOAD and STORE always use immi
					_stations[i].vj = instruction.imm;
				}
				currentRegisterResultStatuses.updateStatus(instruction.dest,_stations[i].nameString,false);	
				return;		
			}
		}
	}
	bool getReservationStatus(string stationName){
		for(int i=0;i<_stations.size();i++){
			if(_stations[i].nameString == stationName){
				return _stations[i].busy;
			}
		}
		return false;
	}
	void updateTable(CommonDataBus &cdb,RegisterResultStatuses &currentRegisterResultStatuses,int thiscycle){
		for(int i=0;i<_stations.size();i++){
			string src1 = _stations[i].qj;
			string src2 = _stations[i].qk;
			if((_stations[i].busy && !getReservationStatus(src1) && !getReservationStatus(src2)) || (_stations[i].busy&&_stations[i].name==LOAD) || (_stations[i].busy&&_stations[i].name==STORE)) {
				//check if all sources are ready so we can decrease its remaining cycle
				_stations[i].remainCycle--;
			}
			if(_stations[i].busy&&_stations[i].remainCycle==0){
				_stations[i].cycleExecuted = thiscycle;
				if(!_stations[i].inQ){
					_stations[i].inQ = true;
					cdb.addQ(_stations[i]);
				}
			}
		}
	}
	void freeStation(string stationName){
		for(int i=0;i<_stations.size();i++){
			if(_stations[i].nameString == stationName){
				_stations[i].busy = false;
				_stations[i].cycleExecuted = -1;
				_stations[i].numInstruction = -1;
				_stations[i].remainCycle = -1;
				_stations[i].vk = -1;
				_stations[i].vj = -1;
				_stations[i].qj = "";
				_stations[i].qk = "";
				_stations[i].issuedCycle = -1;
				_stations[i].inQ = false;
				return;
			}
		}
	}

private:
	vector<ReservationStation> _stations;
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

// Function to simulate the Tomasulo algorithm
void simulateTomasulo(RegisterResultStatuses& registerStatuses,ReservationStations& reservationStations,CommonDataBus& cdb,vector<InstructionStatus>& instructionStatus,vector<Instruction>& instructions)
{

	int thiscycle = 1; // start cycle: 1
	int numInstruction = 0;
	int completedInstruction = 0;
	while (thiscycle < 100000000)
	{

		// Reservation Stations should be updated every cycle, and broadcast to Common Data Bus
		// ...
		if(!cdb.isEmpty()){
			ReservationStation broadcast = cdb.getTop();
			cdb.pop();
			//free that station
			reservationStations.freeStation(broadcast.nameString);
			//update register value
			registerStatuses.updateStatus(instructions[broadcast.numInstruction].dest,broadcast.nameString,true);
			instructionStatus[broadcast.numInstruction].cycleWriteResult = thiscycle;
			instructionStatus[broadcast.numInstruction].cycleExecuted = broadcast.cycleExecuted;
			completedInstruction++;
		}
		reservationStations.updateTable(cdb,registerStatuses,thiscycle);
		// Issue new instruction in each cycle
		// ...
		if(reservationStations.isFreeStationAvailable(instructions[numInstruction].op)){
			instructions[numInstruction].status.cycleIssued = thiscycle;
			instructionStatus[numInstruction].cycleIssued = thiscycle;
			reservationStations.addInstruction(instructions[numInstruction],thiscycle,registerStatuses,numInstruction);
			numInstruction++;
		}
		// At the end of this cycle, we need this function to print all registers status for grading
		PrintRegisterResultStatus4Grade(outputtracename, registerStatuses, thiscycle);
		++thiscycle;

		// The simulator should stop when all instructions are finished.
		// ...
		if(completedInstruction == instructions.size()){
			break;
		}
	}
};
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
			dest.erase(0,1);
			dummy.dest = stoi(dest);
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
				src1.erase(0,1);
				src2.erase(0,1);
				dummy.src1 = stoi(src1);
				dummy.src2 = stoi(src2);
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
	CommonDataBus cdb;
	// Simulate Tomasulo:
	simulateTomasulo(registerResultStatus,reservationStations,cdb,instructionStatus, instructions);

/*********************************** ↑↑↑ Todo: Implement by you ↑↑↑ ******************************************/

	// At the end of the program, print Instruction Status Table for grading
	PrintResult4Grade(outputtracename, instructionStatus);

	return 0;
}
