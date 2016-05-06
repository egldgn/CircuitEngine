/*
This file is part of Circuit Engine.
 
Circuit Engine is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
 
Circuit Engine is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
 
You should have received a copy of the GNU General Public License
along with Circuit Engine.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------
	Gate.h : The primitive base class for CEngine,
			 handles individual gate calculations
			 and connections.
*/

#pragma once
#include "Binary.h" // inherits <stdio.h><stdlib.h><malloc.h><math.h>and<string.h>

#define GATE_MAX_GATE_INPUT_ARRAY 10
#define GATE_NO_LEG 0
#define GATE_NOF_TYPES 12

enum GateType {gtEmpty,gtInput,gtAND,gtNAND,gtOR,gtNOR,gtXOR,gtNOT,gtXNOR,gtVoltage,gtGround,gtCable,gtSource};
char *GateNames[] = {"Empty","Input","AND","NAND","OR","NOR","XOR","NOT","XNOR","Voltage","Ground","Cable","Source"};

class Gate
{
private:

	bool on;					//the gate gives always logical 0, if it is off.
	GateType type;				//the code of the gate which indicates the type.
	bool ready;					//gate doesn't respond calculate command unless it is ready.

	void GiveError(char *func_name,char *e);
	void GiveMsg(char *func_name,char *msg);
	bool SetInputArray(unsigned int nof_inputs);

public:
	void SetType(GateType type);

	Gate **Input;				//variable input array.
	bool output;				//logical output of the gate.
	Gate *next_gate;
	unsigned short nof_inputs;	//size of the Input array.
	unsigned short leg_no;		//associated leg no.

	Gate();
	~Gate();
	bool Calculate(void);
	void Connect(unsigned int input_index, Gate *OutputGate);
	void SetProperties(GateType new_type,unsigned short new_nof_inputs = 0, unsigned short new_leg_no = 0);
	GateType GetGateType(void){return type;}
	unsigned short GetLegNo(void){return leg_no;}
	unsigned short GetNofInputs(void){return nof_inputs;}
	void SetNext(Gate *new_next_gate);
	Gate *GetNext(void);
	bool IsReady(void);
	void SetLeg(unsigned short leg_no);

	static GateType Gate::ChrToGateType(char * GateName);
	static bool IsOutputtingGate(GateType type);
};

GateType Gate::ChrToGateType(char * GateName)
{
	for(unsigned counter = 2; counter < GATE_NOF_TYPES - 2; counter++) //first and last two gates cannot be.
		if(!strcmp(GateName,GateNames[counter]))//GateType ChrToGateType(char * GateName)
			return (GateType) counter;
	return gtEmpty;
}
bool Gate::IsReady(void)
{
	return ready;
}

Gate::Gate()
{
	on = true;       //the gate gives logical 0, if it is off.
	type = gtEmpty;  //the code of the gate which indicates the type.
	output = false;  //the logical output of the gate.
	leg_no = 0;      //associated leg no. (0 means no association)
	nof_inputs = 0;
	Input = NULL;
	next_gate = NULL;
	ready = false;
}

Gate::~Gate()
{
	if(Input != NULL)
		delete Input;
}

void Gate::GiveError(char *func_name,char *e)
{
	printf("\n(e)(Gate): occured in Gate(Type[%s],NofInputs[%d],OutAsLeg[%d])::%s() : %s.",
		GateNames[type],nof_inputs,leg_no,func_name,e);
	exit(0);
}

void Gate::GiveMsg(char *func_name,char *msg)
{
	printf("\n(i)(Gate):(Type[%s],NofInputs[%d],OutAsLeg[%d])::%s() : %s.",
		GateNames[type],nof_inputs,leg_no,func_name,msg);
}
bool Gate::SetInputArray(unsigned int new_nof_inputs)//
{
	if(new_nof_inputs > GATE_MAX_GATE_INPUT_ARRAY) GiveError("SetInputArray","new_nof_inputs is greater than GATE_MAX_GATE_INPUT_ARRAY");

	if(Input != NULL) //if the nof_input is set again.
	{//Input array is cleared for regeneration."
		free(Input);
		nof_inputs = 0;		
	}

	Input = new Gate* [new_nof_inputs];

	nof_inputs = new_nof_inputs;
	for(int i=0; i<nof_inputs; i++)	Input[i] = NULL;

	//Input array is ready
	return true;
}

void Gate::Connect(unsigned int input_index, Gate *output_gate)//
{
	if (input_index >= nof_inputs)
		GiveError("Connect","too big input_index for nof_inputs");

	if (Input == NULL)
		GiveError("Connect","no input array; may be you forget to setting the properties of the gate");

	Input[input_index] = output_gate;

	//GiveMsg("Connect","Connection ready");
}

void Gate::SetType(GateType type)
{
	this->type = type;
	//GiveMsg("SetType","type is ready");
}

void Gate::SetLeg(unsigned short leg_no)
{
	this->leg_no = leg_no;
	//GiveMsg("SetLeg","leg_no is ready");
}
void Gate::SetProperties(GateType new_type,unsigned short new_nof_inputs, unsigned short new_leg_no)
{

	switch(new_type) //check gate type constraints.
	{
	case gtEmpty:
	case gtVoltage:
	case gtGround:
		if(new_nof_inputs != 0) GiveError("Gate","cannot have an input");
		if(new_type == gtVoltage) this->output = true;
		break;
	case gtInput:
	case gtNOT:
		if(new_nof_inputs != 1) GiveError("Gate","can have only 1 input");
		break;
	case gtAND:
	case gtNAND:
	case gtOR:
	case gtNOR:
	case gtXOR:
	case gtXNOR:
		if(new_nof_inputs < 2) GiveError("Gate","must have at least 2 input");
		break;
	default: GiveError("SetProperties","Unknown Type");
	}

	SetLeg(new_leg_no);
	SetInputArray(new_nof_inputs);
	SetType(new_type);

	if(type ==  gtVoltage) output = true;
	//gtGround has already 0 output as default.

	ready = true;
	GiveMsg("SetProps.","properties OK");
}


bool Gate::Calculate(void)
{//return value is not only the calc result but its completion also.
	static int i;

	if(!on) //shut down the gate.
		return output = false;
	if(!ready) GiveError("Calculate","Properties are not set");
	if(type == gtEmpty) GiveError("Calculate","Type is not set");
	if(type != gtInput)
        for(i=0; i<nof_inputs; i++) if(Input[i] == NULL) GiveError("Calculate","Connections are not complete");

	switch(type) //check gate type constraints.
	{
	case gtVoltage:
	case gtGround:
		GiveError("Calculate","This is a source gate...calculate for what?!");
		break;

	case gtInput:
		if(Input[0] == NULL) {output= false; break;}
		output = Input[0]->output;
		break;

	case gtNOT:
		output = ! (Input[0]->output);
		break;
	case gtAND:
		output = Input[0]->output;
		for(i=1; i<nof_inputs; i++)	output &= Input[i]->output;
		break;
	case gtNAND:
		output = Input[0]->output;
		for(i=1; i<nof_inputs; i++)	output &= Input[i]->output;
		output = !output;
		break;
	case gtOR:
		output = Input[0]->output;
		for(i=1; i<nof_inputs; i++)	output |= Input[i]->output;
		break;
	case gtNOR:
		output = Input[0]->output;
		for(i=1; i<nof_inputs; i++)	output |= Input[i]->output;
		output = !output;
		break;
	case gtXOR:
		output = Input[0]->output;
		for(i=1; i<nof_inputs; i++)	output ^= Input[i]->output;
		break;
	case gtXNOR:
		output = Input[0]->output;
		for(i=1; i<nof_inputs; i++)	output ^= Input[i]->output;
		output = !output;
		break;

	default: GiveError("Calculate","Unknown Type");
	}

	return output;//CHECK
}

void Gate::SetNext(Gate *new_next_gate)
{
	next_gate = new_next_gate;
}

Gate *Gate::GetNext(void)
{
	return next_gate;
}
bool Gate::IsOutputtingGate(GateType type)
{
	switch(type)
	{
	case gtInput:
	case gtCable:
		return false;
	}
	return true;
}