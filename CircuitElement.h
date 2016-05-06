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
	CircuitElement.h :	Holds and handles list of Gates in order
						to constitude and manipulate a circuit element.
*/

/*
  CE::CABLE differencies:
	nof_inputs => Position X2
	nof_outputs => Position Y2
	V.nof_inputs => height
	V.output => Selected Leg
	V.next_gate => points to CE::CABLE (explicitly)
	V.Input[0] => output gate address reported by a plugged leg of cable
	V.leg_no => CableNodeId //connected cables consists one node.

	and the queries: ceqHover1, ceqHover2, ceqHasOd1, ceqHasOd2
	belong to CE::CABLE only.
*/


#pragma once
#include "gate.h" //Gate.h inherits Binary.h
#include "BreadBoard.h"


//CE constant definitions.
#define CE_NOF_TYPES 5
#define CE_NOF_STATE_QUERIES 6

//CE state and type enumerations:
enum CEQuery {ceqHover,ceqOn,  ceqHover1,ceqHover2,ceqHasOd1,ceqHasOd2,};
/*last four queries are for cetCABLE:
	ceqHover1 : is first leg of the cable in hover mode.
	ceqHover2 : is second leg of the cable in hover mode.
	ceqHasOd1 : is there output dublication in first leg's BBNode list of the cable.
	ceqHasOd2 : is there output dublication in second leg's BBNode list of the cable.
*/
//enum CabStats {CabStatINPUT, CabStatOUTPUT};
enum CEtype {cetEmpty,cetLED,cetCHIP,cetCABLE,cetSOURCE,cetBREADBOARD};
char *CEnames[]  = {"Empty","LED","CHIP","CABLE","SOURCE","BREADBOARD"}; //for logging.

unsigned int CurrentBBrow,CurrentBBcol,CurrentBBstride; //are set by Artist for movement restriction of CEs.
unsigned int LastOutputRow, LastOutputCol;	//are filled by BreadBoard. this is the list address to which the last output enters.
BBNode *LastOutputAddres;					//and they are used by the cetCABLE::CalculateCE().


class CircuitElement
{
private:
	Gate *gate;				//The list of gates of the CE.
	CircuitElement *next_ce;//Concequtive CE in a CE list.
	CircuitElement *prev_ce;
	unsigned int id;		//identifier of CE.
	unsigned int nof_gates;	//Number of gates that the CE contains.
	CEtype type;			//Type of the CE: CHIP LED or CABLE.
	static unsigned int ce_id_counter; //identifier counter.


	//GUI related variables for CE positioning over/on BreadBoard.
	unsigned int position_x; //BB(row,col,stride) (the upper left leg position on BB hole array)
					//for cetCABLE, this is first leg position X.
	unsigned int position_y; //these vars. are to check plugability of this CE on breadboard.
					//for cetCABLE, this is first leg position Y.

	//CE Message Functions:
	void GiveError(char *e);
	void GiveMsg(char *msg);

public:
	char name[27];			//in-script name of the CE:CHIP or, "LED" or "CABLE".
	unsigned int
		nof_inputs,  //to check nof_inputs; in-script CHIP header is equal with the declarations in CHIP block.
					//for cetCABLE, this is second leg position X.
		nof_outputs; //to check nof_outputs
					//for cetCABLE, this is second leg position Y.

	Gate V, G;				//the connection numbers for V&G in-script is saved in these.

	Binary CEInfo; //Holds information about CE states.(Inspect CEQuery enums for binary queries.)
	Binary LegPlugAbility;
	CircuitElement();
	~CircuitElement();
	Gate *AddGate(GateType gate_type, unsigned int nof_inputs = 0, unsigned int leg_no = 0); //add to tail of the list.
	void print(void); //Prints the CE for logging purpose.
	void CalculateCE(void);	//Calculation of all gates in the CE.

	//CE Adjustment Functions:
	void SetType(CEtype new_type);
	void SetNext(CircuitElement *new_next_ce);
	void SetPrev(CircuitElement *new_prev_ce);
	void SetName(char *new_name);
	void SetNofInputs(unsigned int new_nof_inputs);
	void SetNofOutputs(unsigned int new_nof_inputs);
	void SetVoltageLegNo(unsigned short voltage_leg_no);
	void SetGroundLegNo(unsigned short ground_leg_no);
	//void SetNofGates(unsigned int new_nof_gates);


	//CE Query Functions:
	unsigned int	GetNofGates(void);
	CircuitElement	*GetNext(void);
	CircuitElement	*GetPrev(void);
	Gate			*GetHead(void);
	CEtype			GetType(void);
	char			*GetName(void);
	unsigned int	GetNofLegs(void);
	unsigned int	GetPositionX(void);
	unsigned int	GetPositionY(void);
	Gate			*GetGateAddrOfLeg(unsigned int LegNo);
	bool IsOutputtingLeg(unsigned int LegNo);// may be dublication check it later...
	unsigned int	GetPositionX2(void);
	unsigned int	GetPositionY2(void);
	unsigned int    GetID(void);

	//CE Binary Query Functions:
	void SetStateInfo(CEQuery enumQ);
	void ResetStateInfo(CEQuery enumQ);
	bool GetStateInfo(CEQuery enumQ);

	//Visual
	void MoveTo(unsigned int pos_x, unsigned int pos_y);
	void MoveTo2(unsigned int pos_x2, unsigned int pos_y2);//for cable other leg.
	void MoveUp(void);
	void MoveDown(void);
	void MoveLeft(void);
	void MoveRight(void);
};

unsigned int CircuitElement::ce_id_counter = 0;

CircuitElement::CircuitElement()
{
	strcpy(name,"CE_NONAME");
	id = ce_id_counter++;
	GiveMsg("New circuit element requested. initializing...");

	nof_gates = 0;
	gate = NULL;
	next_ce = NULL;
	prev_ce = NULL;
	type = cetEmpty;

	nof_inputs = 0;
	nof_outputs = 0;

	position_x = 0;
	position_y = 0;

	GiveMsg("Voltage leg is creating...");
	V.SetProperties(gtInput,1);
	GiveMsg("Ground leg is creating...");
	G.SetProperties(gtInput,1);

    CEInfo.CreateBitArray(CE_NOF_STATE_QUERIES);
	CEInfo.SetIndex(ceqHover);
	CEInfo.Clear();
	SetStateInfo(ceqHover); // initially the CE is in hover mode before its birth in GUI.

	//LegPlugAbility is created by CECircuit class after parse.
}

CircuitElement::~CircuitElement()
{
	Gate *pick = gate;
	Gate *prev_gate = NULL;
	for(unsigned int i=0; i<nof_gates; i++)
	{
		prev_gate = pick;
		pick = pick->GetNext();
		delete prev_gate;
	}
	gate = NULL;
}

Gate *CircuitElement::AddGate(GateType gate_type, unsigned int nof_inputs ,unsigned int leg_no )
{//in destructor, place deallocation lines
	Gate *new_gate = new Gate();
	new_gate->SetProperties(gate_type,nof_inputs,leg_no);

	if(nof_gates == 0)
	{
		gate = new_gate;
	}
	else
	{
		Gate *pick = gate;
		for(; pick->GetNext() != NULL; pick = pick->GetNext()) ;//move on last.
		pick->SetNext(new_gate);
	}

	nof_gates ++;
	return new_gate;
}

void CircuitElement::print(void)
{
	unsigned int i=0;
	Gate *pick = gate;
	if(pick == NULL) {printf("\nEmpty CE : %s",name); return;}

	printf("\nGate List CE %s.id_%d [gtType(Addr).no >> nof_inputs (input addr. list)]",name,id);
	printf("\nV = leg_%d (%p), G = leg_%d (%p)",V.GetLegNo(), V.Input[0],G.GetLegNo(),G.Input[0]);
	do
	{
		printf("\n%s(%p)(leg_%d).%d >> %d (",GateNames[pick->GetGateType()],pick,pick->GetLegNo(),++i,pick->GetNofInputs());
		for(unsigned short input_no=0; input_no<pick->GetNofInputs(); input_no++)
			printf(" %p ",pick->Input[input_no]);
		printf(")");
		pick = pick->GetNext();

	}
	while(pick != NULL);

}

void CircuitElement::CalculateCE(void)
{
	switch (type)
	{
	case cetCHIP:
		V.Calculate();
		G.Calculate();
		
		if(V.output && !G.output)//behave normal.
			for(Gate *pick = gate; pick!=NULL; pick->Calculate(),pick=pick->GetNext());
		else
		{//the chip has no power -> take all inputs as 0.
			for(Gate *pick = gate; pick!=NULL; pick=pick->GetNext())
				if(pick->GetGateType() == gtInput)
                    pick->output = false;

			for(Gate *pick = gate; pick!=NULL; pick=pick->GetNext())
				if(pick->GetGateType() != gtInput)
					pick->Calculate();
		}
		

		break;
	case cetLED://if one of the legs of CE:LED is NULL turn off it else both connected to some gate check correctness of poles of the LED, if so turn on it.
		V.Input[0] == NULL || G.Input[0] == NULL ? CEInfo.ResetIndex(ceqOn) : V.Calculate() == true && G.Calculate() == false ? CEInfo.SetIndex(ceqOn) : CEInfo.ResetIndex(ceqOn);
		break;
	case cetCABLE:
		//cable only completes the connections between CEs while it is plugging/unplugging
		//so, no calculation stage occurs.
		break;
	}
}
void CircuitElement::GiveError(char *e)
{
	printf("\n(e)(CircuitElement %s.%d): %s",name,id,e);
	exit(0);
}

void CircuitElement::GiveMsg(char *msg)
{
	printf("\n(i)(CircuitElement %s.%d): %s",name,id,msg);
}

unsigned int CircuitElement::GetNofGates(void)
{
	return nof_gates;
}

void CircuitElement::SetName(char *new_name)
{
	GiveMsg("Renaming...");
	strncpy(name,new_name,sizeof(name));
	name[sizeof(name)-1] = '\0';
	GiveMsg("New name is OK.");
}

void CircuitElement::SetType(CEtype new_type)
{
	type = new_type;
}

void CircuitElement::SetNext(CircuitElement *new_next_ce)
{
	next_ce = new_next_ce;
}
void CircuitElement::SetPrev(CircuitElement *new_prev_ce)
{
    prev_ce = new_prev_ce;
}

void CircuitElement::SetNofInputs(unsigned int new_nof_inputs)
{
	nof_inputs = new_nof_inputs;
}
void CircuitElement::SetNofOutputs(unsigned int new_nof_outputs)
{
	nof_outputs = new_nof_outputs;
}
void CircuitElement::SetVoltageLegNo(unsigned short voltage_leg_no)
{
	GiveMsg("Setting Voltage Leg Association");
	V.SetProperties(gtInput,1,voltage_leg_no);
}
void CircuitElement::SetGroundLegNo(unsigned short ground_leg_no)
{
	GiveMsg("Setting Ground Leg Association");
	G.SetProperties(gtInput,1,ground_leg_no);
}
//void CircuitElement::SetNofGates(unsigned int new_nof_gates)
//{
//	nof_gates = new_nof_gates;
//}
CircuitElement *CircuitElement::GetNext(void)
{
	return next_ce;
}
CircuitElement *CircuitElement::GetPrev(void)
{
	return prev_ce;
}

Gate *CircuitElement::GetHead(void)
{
 return gate;
}

CEtype CircuitElement::GetType(void)
{
	return type;
}
char *CircuitElement::GetName(void)
{
	return name;
}

void CircuitElement::SetStateInfo(CEQuery enumQ)
{
    CEInfo.SetIndex(enumQ);
}
bool CircuitElement::GetStateInfo(CEQuery enumQ)
{
    return CEInfo[enumQ];
}
void CircuitElement::ResetStateInfo(CEQuery enumQ)
{
    CEInfo.ResetIndex(enumQ);
}
unsigned int CircuitElement::GetNofLegs(void)
{
    return (nof_inputs+nof_outputs) + (nof_inputs+nof_outputs)%2 + 2;
}
unsigned int CircuitElement::GetPositionX(void)
{
	return position_x;
}
unsigned int CircuitElement::GetPositionY(void)
{
	return position_y;
}
void CircuitElement::MoveTo(unsigned int pos_x, unsigned int pos_y)
{
    position_x = pos_x;
	position_y = pos_y;
}
void CircuitElement::MoveTo2(unsigned int pos_x2, unsigned int pos_y2)
{//!a cetCABLE specific function!
    nof_inputs = pos_x2;
	nof_outputs = pos_y2;
}
void CircuitElement::MoveUp(void)
{
	if(type == cetCABLE)
	{
		if(!V.output)//if the selected leg is the first one
		{
            if(position_y != 0) position_y -= 1;
		}
		else//if the selected leg is the second one
		{
			if(nof_outputs != 0) nof_outputs -= 1;
		}
		return;
	}
	if(position_y != 0) position_y -= 1;
}
void CircuitElement::MoveDown(void)
{
	switch(type)
	{
	case cetCHIP: if( position_y < CurrentBBrow - (GetNofLegs()/2) ) position_y += 1;
		break;
	case cetLED: if(position_y < CurrentBBrow-2) position_y += 1;
		break;
	case cetCABLE:
		if(!V.output)//if the selected leg is the first one
		{
            if(position_y < CurrentBBrow-1) position_y += 1;
		}
		else//if the selected leg is the second one
		{
			if(nof_outputs < CurrentBBrow-1) nof_outputs += 1;
		}
		break;
	default:;// if(position_y > CurrentBBrow-1)position_y += 1;
	}
}
void CircuitElement::MoveLeft(void)
{
	switch(type)
	{
	case cetCHIP:
		if(position_x != CurrentBBstride-1) //if not on the left most hollow
			position_x -= CurrentBBstride +  2;// 2 is the x length of a hollow.
		break;
	case cetLED:
        if( position_x!=0 && (position_x)%(CurrentBBstride+2) == 0 ) position_x -= 2; //jump over stride.
		if(position_x > 0) position_x -= 1;
		break;
	case cetCABLE:
		if(!V.output)//if the selected leg is the first one
		{
            if( position_x!=0 && (position_x)%(CurrentBBstride+2) == 0 ) position_x -= 2; //jump over stride.
			if(position_x > 0) position_x -= 1;
		}
		else//if the selected leg is the second one
		{
            if( nof_inputs!=0 && (nof_inputs)%(CurrentBBstride+2) == 0 ) nof_inputs -= 2; //jump over stride.
			if(nof_inputs > 0) nof_inputs -= 1;
		}
		break;
	default: ;//if(position_x > 0) position_x -= 1;
	}

}
void CircuitElement::MoveRight(void)
{
	switch(type)
	{
	case cetCHIP:
		if(position_x != (CurrentBBstride-1) + (CurrentBBcol-2)*(CurrentBBstride+2))
			//if not on the right most hollow move right.
			position_x += CurrentBBstride +  2;// 2 is the width of a hollow.
		break;
	case cetLED://if one right is hollow add 2 more (as hollow width) to the default case(no break!)
        if((position_x+1)!= CurrentBBcol*(CurrentBBstride+2)-2 && (position_x+3) % (CurrentBBstride+2) == 0 )position_x+=2;//jump over stride.
		if(position_x < (CurrentBBcol*CurrentBBstride) + (CurrentBBcol-1)*2 - 1) position_x += 1;
		break;
	case cetCABLE:
		if(!V.output)//if the selected leg is the first one
		{
			if((position_x+1)!= CurrentBBcol*(CurrentBBstride+2)-2 && (position_x+3) % (CurrentBBstride+2) == 0 )position_x+=2;//jump over stride.
			if(position_x < (CurrentBBcol*CurrentBBstride) + (CurrentBBcol-1)*2 - 1) position_x += 1;
		}
		else//if the selected leg is the second one
		{
			if((nof_inputs+1)!= CurrentBBcol*(CurrentBBstride+2)-2 && (nof_inputs+3) % (CurrentBBstride+2) == 0 )nof_inputs+=2;//jump over stride.
			if(nof_inputs < (CurrentBBcol*CurrentBBstride) + (CurrentBBcol-1)*2 - 1) nof_inputs += 1;
		}
		break;
	}
}
bool CircuitElement::IsOutputtingLeg(unsigned int LegNo)
{
    for(Gate *pick_G = gate; pick_G != NULL; pick_G = pick_G->GetNext())
		if(pick_G->GetLegNo() == LegNo+1) //leg numbers in gate starts from 1 inst. of 0
			return pick_G->GetGateType() == gtInput ? false : true;
	return false;
}
Gate *CircuitElement::GetGateAddrOfLeg(unsigned int LegNo)
{
	if(V.GetLegNo() == (LegNo+1)) return &V;
	if(G.GetLegNo() == (LegNo+1)) return &G;
	for(Gate *pick_G = gate; pick_G != NULL; pick_G = pick_G->GetNext())
		if(pick_G->GetLegNo() == (LegNo + 1))
			return pick_G;
	return NULL;
}
unsigned int CircuitElement::GetPositionX2(void)
{
	return nof_inputs; //for second leg X position of CE::cetCABLE.
}

unsigned int CircuitElement::GetPositionY2(void)
{
    return nof_outputs; //for second leg Y position of CE::cetCABLE.
}
unsigned int CircuitElement::GetID(void)
{
	return id;
}