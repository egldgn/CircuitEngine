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
	BreadBoard.h : handles and manipulates the connections between CEs
*/

#pragma once
#include "BBNode.h"
#include "CircuitElement.h"



class BreadBoard
{
	friend CircuitElement;
	BBNode ***NodeHead;
	BBNode *CurrentHole;
    Binary ***NodeMapHead; //shows hole's availability.

	unsigned int nof_rows;//nof_rows * nof_cols = nof nodes.
	unsigned int nof_cols;
	unsigned int nof_holes_in_a_node;

	static unsigned int CurrentCableNodeID; //is set before call of SendCableNodeID recursion.

	void NewCurrentHole(void);
	void SetCurrentHole(Gate *GateInfo);
	void AddCurrentHole(unsigned int BBrow, unsigned int BBcol, unsigned int BBstride, CircuitElement* DontTriggerCable);
	void RemoveHole( unsigned int BBrow, unsigned int BBcol, Gate *RemGate, CircuitElement* DontTriggerCable);
	void SendCableNodeID(unsigned int BBrow,unsigned int BBcol, CircuitElement *DontTriggerCable);

	void GiveError(char *e);
	void CheckTableRange(unsigned int BBrow, unsigned int BBcol, unsigned int BBstride);
	Gate *IsThereOutputGateInBBNode(unsigned int BBrow, unsigned int BBcol);
	void CalculateCable(CircuitElement *Cable, unsigned int OutputRow, unsigned int OutputCol, bool OutputInsertion);

public:
    BreadBoard(unsigned int row, unsigned int col, unsigned int stride);
	~BreadBoard();

	void ToggleHole(unsigned int BBrow, unsigned int BBcol, unsigned int BBstride);
	bool IsHoleUsing(unsigned int BBrow, unsigned int BBcol, unsigned int BBstride);
	unsigned int GetStrideValue(){return nof_holes_in_a_node;}
	void SetLegPlugAbility(CircuitElement *CE);
	void PlugCE(CircuitElement *CE);
	void UnPlugCE(CircuitElement *CE);
	void debugprint(void);
	void debugInsertVG(Gate *V,Gate *G);
	void debugListCableNodes(CircuitElement *Cable);
	void BreadBoard::ListBBNode(unsigned int BBrow, unsigned int BBcol);
};

unsigned int BreadBoard::CurrentCableNodeID = 0;

void BreadBoard::GiveError(char *e)
{
	printf("\n(e)BreadBoard: %s",e);
	exit(1);
}
BreadBoard::BreadBoard(unsigned int row, unsigned int col, unsigned int stride)
{
	unsigned int i,j;

	nof_rows = row;
	nof_cols = col;
	nof_holes_in_a_node = stride;	

	//init NodeHead.
	printf("\nInitializing BreadBoard Node Table(r%d,c%d)...",nof_rows,nof_cols);
    NodeHead = (BBNode ***) malloc( sizeof(BBNode**) * nof_rows ); //create column pointers.
	if(NodeHead == NULL) GiveError("Initializing failed. (Not enough memory)");
	for(i=0 ; i<nof_rows  ; i++)
	{
		NodeHead[i] = (BBNode **) malloc( sizeof(BBNode*) * nof_cols );
	    if(NodeHead[i] == NULL) GiveError("Initializing failed. (Not enough memory)");
		for(j = 0 ; j < nof_cols ; j++)
			NodeHead[i][j] = NULL; //Init Node Heads as NULL...
	}
	printf("\nBreadBoard Node Table is ready.");
	//BBNodeHead Array is ready.

    CurrentHole = NULL;

	//init NodeMapHead.
	printf("\nInitializing BreadBoard Node Map Table(h%d)...",nof_holes_in_a_node);
	NodeMapHead = (Binary***) malloc( sizeof(Binary**) * nof_rows );
	if(NodeMapHead == NULL) GiveError("Initializing failed. (Not enough memory)");
	for(i = 0 ; i < nof_rows ; i++)
	{
		NodeMapHead[i] = (Binary**) malloc(sizeof(Binary*) * nof_cols );
		if(NodeMapHead[i] == NULL) GiveError("Initializing failed. (Not enough memory)");
		for(j = 0 ; j < nof_cols ; j++)
            NodeMapHead[i][j] = new Binary(nof_holes_in_a_node); //Init corresponding Binary Arrays...
	}
	printf("\nBreadBoard Node Map Table is ready.");
	//NodeMapHead is ready.
}

BreadBoard::~BreadBoard()
{

	unsigned int i,j;
	BBNode *del_1, *del_2;

    printf("\nDeallocating BreadBoard Node Table...");//clear NodeHead***
	for(i = 0 ; i < nof_rows ; i++)
	{
		for(j = 0 ; j < nof_cols ; j++)
		{
			del_1 = NodeHead[i][j];
			for(;del_1 != NULL;)
			{//free the linked list whose head is in (node [i][j]).
				del_2 = del_1->GetNextHole();
                delete del_1; //free the BBNode list element (the Hole).
				del_1 = del_2;
			}
		}
		free(NodeHead[i]);
	}
	free(NodeHead);


	printf("\nDeallocating BreadBoard Node Map Table...");
	for(i = 0 ; i < nof_rows ; i++)
	{
		for(j = 0 ; j < nof_cols ; j++)
            delete NodeMapHead[i][j];
        free(NodeMapHead[i]);
	}
	free(NodeMapHead);

	printf("\nDeallocation of BreadBoard is completed.");
}

void BreadBoard::debugprint(void)
{

	printf("\n<BB:INSIDE BBNodes listing>...\n");
	for(unsigned int i = 0; i < nof_rows; i++)
	{
		for(unsigned int j =0; j < nof_cols; j++)
		{
			printf("[%d,%d]:[",i,j);
			for(BBNode *pick_Hole = NodeHead[i][j]; pick_Hole != NULL; pick_Hole = pick_Hole->GetNextHole())
				if(pick_Hole->GetGateInfo()->GetGateType() == gtCable)
					printf("%p:%d-%d:%p(%d)",pick_Hole->GetGateInfo()->next_gate,((CircuitElement*)(pick_Hole->GetGateInfo()->next_gate))->GetID(),((CircuitElement*)(pick_Hole->GetGateInfo()->next_gate))->V.leg_no ,pick_Hole->GetGateInfo()->Input[0],pick_Hole->GetNodeMapIndex());
			printf("]");
		}
		printf("\n");

	}
	/*printf("\n<BB: BBNodeMaps listing>...\n");
	for(i = 0 ; i<nof_rows; i++)//node map of each node...
	{
        for(unsigned int j = 0; j<nof_cols; j++)
		{
			printf("[%d,%d]<",i,j);
			NodeMapHead[i][j]->debugprint();
			printf(">");
		}
		printf("\n");
	}*/
}
void BreadBoard::CheckTableRange(unsigned int BBrow, unsigned int BBcol, unsigned int BBstride)
{
	if(NodeMapHead == NULL) GiveError("Node Map couldn't found.");
	if(BBrow > nof_rows) GiveError("BBrow is too big.");
	if(BBcol > nof_cols) GiveError("BBcol is too big.");
	if(BBstride > nof_holes_in_a_node) GiveError("BBstride is too big.");
}
void BreadBoard::ToggleHole(unsigned int BBrow, unsigned int BBcol, unsigned int BBstride)
{
	CheckTableRange(BBrow, BBcol, BBstride);
	NodeMapHead[BBrow][BBcol][0][BBstride] == 0 ? NodeMapHead[BBrow][BBcol][0].SetIndex(BBstride) : NodeMapHead[BBrow][BBcol][0].ResetIndex(BBstride);
}
bool BreadBoard::IsHoleUsing(unsigned int BBrow, unsigned int BBcol, unsigned int BBstride)
{
	CheckTableRange(BBrow, BBcol, BBstride);
    return NodeMapHead[BBrow][BBcol][0][BBstride];
}

void BreadBoard::NewCurrentHole(void)
{
	if( CurrentHole != NULL )
		GiveError("BB::NewCurrentHole() : Missing AddCurrentHole().");
	CurrentHole = new BBNode();	//Current-Hole-Block start...
}
void BreadBoard::SetCurrentHole(Gate *GateInfo)
{
	if( CurrentHole == NULL ) GiveError("BB:SetCurrentHole() : Missing NewCurrentHole().");
    CurrentHole->SetGateInfo(GateInfo);
}
void BreadBoard::AddCurrentHole(unsigned int BBrow, unsigned int BBcol, unsigned int BBstride, CircuitElement* DontTriggerCable)
{
	BBNode * pick_HOLE;
	bool GateIsOutput = false;

	if(BBrow > this->nof_rows || BBcol > this->nof_cols || BBstride > this->nof_holes_in_a_node)
		GiveError("Fatal : Add Current Hole request exceeds the size of the BreadBoard");

	if((CurrentBBstride != BBstride) && (*(NodeMapHead[BBrow][BBcol]))[BBstride])
	{ //if the hole addition is not an output duplication && the corresponding hole is not empty phsically.
		GiveError("Fatal : BreadBoard::AddCurrentHole() - leg occupation physics crash (second insertion was requested on the same hole)");
	}

	NodeMapHead[BBrow][BBcol]->SetIndex(BBstride);// Set the corresponding map bit.
	CurrentHole->SetNodeMapIndex(BBstride);//save stride, to which hole the leg belongs to

	//if BBNode list is not empty && the inserting BBNode is not of a cable.
	if(NodeHead[BBrow][BBcol] != NULL && CurrentHole->GetGateInfo()->GetGateType() != gtCable) 
	{//don't make connection for cables (they are different than other CEs.)
		if(GateIsOutput = Gate::IsOutputtingGate(CurrentHole->GetGateInfo()->GetGateType()))
		{//make all inputting gates point to this new output leg(gate)
			for(pick_HOLE = NodeHead[BBrow][BBcol]; pick_HOLE != NULL; pick_HOLE = pick_HOLE->GetNextHole())
				if( pick_HOLE->GetGateInfo()->GetGateType() != gtCable ) //ignore cable BBNodes
					pick_HOLE->GetGateInfo()->Connect(0, CurrentHole->GetGateInfo());
		}
		else//if new leg is inputting and there is an outputting leg in the node.
		{//make new inputting gate point to the outputting gate.(IsThere...() returns NULL if there is not an output gate)
			CurrentHole->GetGateInfo()->Connect(0, IsThereOutputGateInBBNode(BBrow, BBcol));
		}
		//else: there is only inputting gates in the list and the new leg is inputting also
		//		so, do nothing about the connections...
	}

	//insert into corresponding list.

	//if this is the first BBNode in the list (Cable or Gate) 
	if(NodeHead[BBrow][BBcol] == NULL)
		NodeHead[BBrow][BBcol] = CurrentHole;
	else
	{//add after cable(s) (cables are always first in the list).
		
		//if this is the first cable insertion. (Gate)
		if(CurrentHole->GetGateInfo()->GetGateType() == gtCable && NodeHead[BBrow][BBcol]->GetGateInfo()->GetGateType() != gtCable)
		{//insert the cable as first hole element.
			CurrentHole->SetNextHole( NodeHead[BBrow][BBcol] );
			NodeHead[BBrow][BBcol] = CurrentHole;
		}
		else
		{//(Cable or Gate)
			//even if this is a cable insertion, it also places after previously inserted cables.		
			//pick_HOLE proceeds onto the last gtCable Node:
			for(pick_HOLE = NodeHead[BBrow][BBcol]; pick_HOLE->GetNextHole() != NULL && pick_HOLE->GetNextHole()->GetGateInfo()->GetGateType()==gtCable ; pick_HOLE=pick_HOLE->GetNextHole());
			CurrentHole->SetNextHole(pick_HOLE->GetNextHole());
			pick_HOLE->SetNextHole(CurrentHole);//the input/output node is placed into the list.
		}

		//there is/are cable(s),so output insertion triggers the cable(s).
		if(GateIsOutput) // (Gate)
		{//trigger the cable(s) for duplication.
			CurrentHole = NULL;//end of Current-Hole-Block... (In Calculate Cable, other insertion blocks are used also)
			for(pick_HOLE = NodeHead[BBrow][BBcol]; pick_HOLE->GetGateInfo()->GetGateType()==gtCable; pick_HOLE=pick_HOLE->GetNextHole())
				if(DontTriggerCable==NULL || (CircuitElement*)pick_HOLE->GetGateInfo()->next_gate != DontTriggerCable)
					//next gate of V in CABLE element points to the CE:CABLE which has this V.
                    CalculateCable((CircuitElement*)(pick_HOLE->GetGateInfo()->next_gate),BBrow,BBcol, true);
		}

	}
	
	CurrentHole = NULL;//end of Current-Hole-Block...

}
void BreadBoard::SetLegPlugAbility(CircuitElement *CE)
{//KIMT : coordinates X and Y in BBDS are swapped in GUI...
	int NofLegsOverTwo = CE->GetNofLegs()/2;
	int i;																//leg index counter
	unsigned int FLrow = CE->GetPositionY();							//first leg row coord.
	unsigned int FLcol = CE->GetPositionX()/(nof_holes_in_a_node+2);	//first leg col coord.
	unsigned int FLstride = CE->GetPositionX()%(nof_holes_in_a_node+2);	//first leg stride coord.

	CE->LegPlugAbility.Clear();	//make all RED at first

	switch(CE->GetType())
	{
	case cetCHIP:

		for( i = 0 ; i < NofLegsOverTwo ; i++)//for left legs up to down
		{
			//goto corr. hole map (leg position) and check if there is already any other plugged.
			if(NodeMapHead[FLrow + i][FLcol][0][FLstride])			
				continue;	//There is already plugged one, so leave it as RED (not pluggable)... <<<
			
			//if hole is empty, proceed for logical test (there cannot be more than 1 outputting gate within the same node)
			if( !CE->IsOutputtingLeg(i) )	//Is this leg of CE an inputting gate?
			{
				CE->LegPlugAbility.SetIndex(i);	//if it's not an outputting gate make it pluggable... <<< (N/A legs interpreted as input gates...)
				continue;
			}			

			//It's an outputting gate,so check if there is any other in the node.
			if(!IsThereOutputGateInBBNode(FLrow+i,FLcol))
			{
				CE->LegPlugAbility.SetIndex(i);	//if no any other outputting, make it pluggable... <<<
				continue;
			}
			
		}
		i--;
		FLcol = (CE->GetPositionX()+3)/(nof_holes_in_a_node+2);	//first leg col coord.
		FLstride = (CE->GetPositionX()+3)%(nof_holes_in_a_node+2);	//first leg stride coord.
		for(; i >= 0 ; i--)//for right legs down to up.
		{
			//goto corr. hole map (leg position) and check if there is already any other plugged.//CE->GetPositionX()+3 for chip
			if(NodeMapHead[FLrow + i][FLcol][0][FLstride])
				continue;//There is already plugged one, so leave it as RED (not pluggable)... <<<
			
			//if hole is empty, proceed for logical test (there cannot be more than 1 outputting gate within the same node)
			if( !CE->IsOutputtingLeg(CE->GetNofLegs() - i - 1) )		//Is this leg of CE an outputting gate?
			{
				CE->LegPlugAbility.SetIndex(CE->GetNofLegs() - i - 1); //if it's not an outputting gate make it pluggable... <<<
				continue;
			}

			//It's an outputting gate,so check if there is any other in the node.
			if(!IsThereOutputGateInBBNode(FLrow+i,FLcol))
			{
				CE->LegPlugAbility.SetIndex(CE->GetNofLegs() - i - 1);	//if no any other outputting make it pluggable... <<<
				continue;
			}			
			
		}
		break;
	case cetLED:
		if(!NodeMapHead[FLrow][FLcol][0][FLstride])//if up leg hole is not occupied
			CE->LegPlugAbility.SetIndex(0);		

		if(!NodeMapHead[FLrow+1][FLcol][0][FLstride])//if down leg hole is not occupied
			CE->LegPlugAbility.SetIndex(1);
		break;
	case cetCABLE:

		//first leg's position is already calculating for all CEs before the 'switch', so here is just for second leg.
		unsigned int SLrow = CE->GetPositionY2(); //CABLE second leg row position.
		unsigned int SLcol = CE->GetPositionX2()/(nof_holes_in_a_node + 2); //CABLE second leg col position.
		unsigned int SLstride = CE->GetPositionX2()%(nof_holes_in_a_node + 2); //CABLE second leg stride position.
		unsigned int SELrow, SELcol, SELstride;

		for(int i=0; i<2; i++)
		{	//SEL - selected , SL - second leg.
			SELrow = i ? SLrow : FLrow;	// 0 - first leg, 1 - second leg.
			SELcol = i ? SLcol : FLcol;
			SELstride = i ? SLstride : FLstride;

			if(!CE->CEInfo[ceqHover1 + i])	//if the leg i is already plugged, make its ability of plugging TRUE.
			{
				CE->LegPlugAbility.SetIndex(i);
				continue;
			}

			if(!NodeMapHead[SELrow][SELcol][0][SELstride])//if corrs. hole is physically empty.
			{
				if(CE->CEInfo[ceqHover2 - i] || CE->V.Input[0] == NULL || //if other is hover or there is no output report.
					!IsThereOutputGateInBBNode(SELrow,SELcol) )// or there is no output in selected leg's node.
					CE->LegPlugAbility.SetIndex(i);				

				if(NodeHead[SELrow][SELcol]!=NULL && NodeHead[SELrow][SELcol]->GetGateInfo()->GetGateType()==gtCable && CE->V.leg_no == NodeHead[SELrow][SELcol]->GetGateInfo()->leg_no)//if corresponding hole is the same node(connecting the two edges of the cable at the same node is meaningless)				
                    CE->LegPlugAbility.ResetIndex(i);					
				
			}
		}
		//if other is plugged and other has O and selected has output, leave it as RED.

		break;
	}
}


void BreadBoard::PlugCE(CircuitElement *CE)
{//before this procedure, the SetLegPlugAbility() must be called for circuit consistency and for GUI feedback.
 //(in this procedure the possibility of a second outputting gate in a BBNode list is not checked)
		
	/*printf("\nPrinting BBDS before PlugCE() (CE = %s:%p)",CE->GetName(),CE);
	debugprint();*/

	int i = 0;														//leg index counter
	unsigned int FLrow = CE->GetPositionY();						//first leg row coord.
	unsigned int FLcol = CE->GetPositionX()/(nof_holes_in_a_node+2);	//first leg col coord.
	unsigned int FLstride = CE->GetPositionX()%(nof_holes_in_a_node+2);	//first leg stride coord.
	Gate *AddrOfLegI; // the address of the gate which is connected to the leg number I.

	switch(CE->GetType())
	{
	case cetCHIP:
		for( ; i<(int)CE->GetNofLegs()/2 ; i++) //left legs
		{
			//printf("\nBB:PCE():Plugging Leg(row<%d>col<%d>str<%d>)",FLrow+i,FLcol,FLstride);//DEBUG

			AddrOfLegI = CE->GetGateAddrOfLeg(i);
			if(AddrOfLegI == NULL)
			{
				NodeMapHead[FLrow + i][FLcol][0].SetIndex(FLstride); //N/A leg only fills physically.
				continue;//It is a N/A leg, so proceed to next leg without any action.
			}

			//Create a new BBNode
			NewCurrentHole();
			//Load gate info into the BBNode
			SetCurrentHole( AddrOfLegI );

			//Add BBNode into the BBNode list and (this procedure also sets corresponding NodeMap bit)
			AddCurrentHole(FLrow + i, FLcol, FLstride, NULL);
		}
		i--;
		FLcol = (CE->GetPositionX()+3)/(nof_holes_in_a_node+2);
		FLstride = (CE->GetPositionX()+3)%(nof_holes_in_a_node+2);
		for( ; i>=0 ; i--) //right legs
		{
			//printf("\nBB:PCE():Plugging Leg(row<%d>col<%d>str<%d>)",FLrow+i,FLcol,FLstride);//DEBUG

			AddrOfLegI = CE->GetGateAddrOfLeg(CE->GetNofLegs() - i - 1);
			if(AddrOfLegI == NULL)
			{
				NodeMapHead[FLrow +i][FLcol][0].SetIndex(FLstride); //N/A leg only fills physically.
				continue;//It is a N/A leg, so proceed to next leg without doing nothing.
			}
			//Create a new BBNode
			NewCurrentHole();
			//Load gate info into the BBNode
			SetCurrentHole( AddrOfLegI );

			//Add BBNode into the BBNode list and (this procedure also sets corresponding NodeMap bit)
			AddCurrentHole(FLrow + i, FLcol, FLstride, NULL);//stride is 0 for right legs.
		}

		break;
	case cetLED:
		//insert necessary BBNodes for up and down leg and mark the corresponding Node Map bits
		for(i=0; i<2; i++) // for voltage and ground legs only.(up leg is the voltage)
		{
			NewCurrentHole();
			SetCurrentHole(CE->GetGateAddrOfLeg(i));
			AddCurrentHole(FLrow+i, FLcol, FLstride, NULL); //this proced. also sets corresponding map bit.
		}
		break;
	case cetCABLE:
		//cable BBNode insertion realizes the original output if there is in other side or selected side,
		//and if found, duplicates it into the selected leg's or the other leg's BBNode respectively.

		bool SelL = CE->V.output;												// get selected leg. 0 = first leg, 1 = second leg
		unsigned int SLrow = CE->GetPositionY2();								//CABLE second leg row position.
		unsigned int SLcol = CE->GetPositionX2()/(nof_holes_in_a_node + 2);		//CABLE second leg col position.
		unsigned int SLstride = CE->GetPositionX2()%(nof_holes_in_a_node + 2);	//CABLE second leg stride position.

		unsigned int SELrow = SelL?SLrow:FLrow;
		unsigned int SELcol = SelL?SLcol:FLcol;
		unsigned int OTHERrow = SelL?FLrow:SLrow;
		unsigned int OTHERcol = SelL?FLcol:SLcol;

		//printf("\n\nprocessing PLUGGING REQUEST FOR CABLE %p for %s leg :",CE,CE->V.output?"Second":"First");//DEBUG

		//insert the BBNode for cable.
		//printf("\n Inserting cable BBNode into node [%d,%d], the leg is in plugged state now.",SELrow,SELcol); //DEBUG
        CE->CEInfo.ResetIndex(ceqHover1 + SelL); //make plugged the selected leg.
		NewCurrentHole();
		SetCurrentHole(&(CE->V)); //this is the CABLE specific BBNode which specifies that the node has this CABLE plugged.
		AddCurrentHole(SELrow, SELcol, SelL?SLstride:FLstride, NULL);//insert the cable BBNode into the selected's node list.

		//PLUGGING CABLE LEG

		if(CE->CEInfo[ceqHover2-SelL]) 
		{//if other is not plugged
            //printf("\n cable's other leg is in hover mode."); // DEBUG
			CE->V.Input[0] = IsThereOutputGateInBBNode(SELrow,SELcol); // report Output if there is one.
			//printf("\n Output Report is done as %p.",CE->V.Input[0]); //DEBUG

			//CABLE_NODE_ID SETTING
			if(NodeHead[SELrow][SELcol]->GetGateInfo()->GetGateType() == gtCable) //if there is another cable in the node
			{//get the cable node id of other cables.
				CE->V.leg_no = NodeHead[SELrow][SELcol]->GetGateInfo()->leg_no;
				//printf("\n CNID is taken as %d.\n",CE->V.leg_no); // DEBUG
			}
		}
		else
		{//if other is plugged

			//printf("\n cable's other leg is plugged.\n pocessing CNID diffuseness..."); //DEBUG

			//CABLE_NODE_ID SETTING
			//inherit the cabe node id to the currently plugging and consecutive cables.
			if((CircuitElement*)(NodeHead[SELrow][SELcol]->GetGateInfo()->next_gate) != CE && NodeHead[SELrow][SELcol]->GetGateInfo()->GetGateType() == gtCable)//if there is other cables.
			{
				//give the CNID of the already existing first cable in the list to the new plugging.
				CurrentCableNodeID = CE->V.leg_no = NodeHead[SELrow][SELcol]->GetGateInfo()->leg_no;
				//printf("\n CurrentCableNodeID = %d",CurrentCableNodeID); //DEBUG
				//and attenuate the ID also on the other node.
				//printf("\n Sending CNID into its %s node.\n",CE,CE->V.output?"First Leg":"Second Leg"); //DEBUG
				SendCableNodeID(OTHERrow,OTHERcol, CE); // trigger cables on other node to make the CNID diffuseness (other than CE)
			}
			//EOF CABLE_NODE_ID SETTING

			//printf("\n EOF CNID diffuseness.\n\n checking output report..."); //DEBUG

            if(CE->V.Input[0])//if there is report => commit to the other side
			{//copy Od to this node
				//printf("\n output report is found as %p.", CE->V.Input[0]); //DEBUG

				//printf("\n Inserting Od into plugging node > (%d,%d).",SELrow,SELcol); //DEBUG
                NewCurrentHole();
				SetCurrentHole(CE->V.Input[0]);//this is Od which has been reported by other leg.
				AddCurrentHole(SELrow,SELcol, nof_holes_in_a_node, CE); //with stride nof_holes_in_a_node; it doesn't occupy a hole physically.

				CE->CEInfo.SetIndex(ceqHasOd1 + CE->V.output);

				//printf("\n The HasOd%d flag is set as %d", CE->V.output + 1, CE->CEInfo[ceqHasOd1 + CE->V.output]); //DEBUG
			}
			else
			{//if there is no report
				//printf("\n no output report found.\n Checking if there is an output in the plugging node..."); //DEBUG
				//check if this has O
				if(CE->V.Input[0] = IsThereOutputGateInBBNode(SELrow,SELcol))
				{//report it and copy Od to other
					//printf("\n output BBNode found as %p > Cable->V.Input[0] = %p",IsThereOutputGateInBBNode(SELrow,SELcol),CE->V.Input[0]); //DEBUG

					NewCurrentHole();
					SetCurrentHole(CE->V.Input[0]);//this is Od which will be reported to other leg.
					AddCurrentHole(OTHERrow, OTHERcol, nof_holes_in_a_node, CE);

					//printf("\n Output is duplicated into %s node (%d,%d) as %p.", CE->V.output?"First Leg":"Second Leg", OTHERrow, OTHERcol, CE->V.Input[0]); // DEBUG
					//indicate that the OTHER leg has Od.
					CE->CEInfo.SetIndex(ceqHasOd2 - CE->V.output);
					//printf("\n The HasOd%d flag is set as %d", 2 - CE->V.output, CE->CEInfo[ceqHasOd2 - CE->V.output]); //DEBUG
				}
				//else printf("\n no output BBNode found in the plugging node."); //DEBUG

			}


		}

		//printf("\nPLUGGING FOR CABLE process is completed."); // DEBUG

		SetLegPlugAbility(CE);
		break;
	}

	//printf("\nPrinting BBDS after PlugCE() (CE = %s:%p)",CE->GetName(),CE);
	//debugprint();

}
void BreadBoard::UnPlugCE(CircuitElement *CE)
{
	int i;																//leg index counter
	unsigned int FLrow = CE->GetPositionY();							//first leg row coord.
	unsigned int FLcol = CE->GetPositionX()/(nof_holes_in_a_node+2);	//first leg col coord.
	unsigned int FLstride = CE->GetPositionX()%(nof_holes_in_a_node+2);	//first leg stride coord.
	Gate *AddrOfLegI;

	/*printf("\nPrinting BBDS before UnPlugCE() (CE = %s:%p)",CE->GetName(),CE);
	debugprint();*/

	switch(CE->GetType())
	{
	case cetCHIP:
		for(i = 0 ; i < (int)CE->GetNofLegs()/2 ; i++)//left legs
		{
			AddrOfLegI = CE->GetGateAddrOfLeg(i);
			if(AddrOfLegI == NULL)
			{
				NodeMapHead[FLrow +i][FLcol][0].ResetIndex(FLstride); //N/A leg only fills physically.
				continue;//It is a N/A leg, so proceed to next leg without doing nothing.
			}
			RemoveHole(FLrow+i,FLcol,AddrOfLegI, NULL);

		}
		i--;
		FLcol = (CE->GetPositionX()+3)/(nof_holes_in_a_node+2); //set col and row positions for right legs.
		FLstride = (CE->GetPositionX()+3)%(nof_holes_in_a_node+2);
		for( ; i >= 0 ; i--) //right legs
		{
			AddrOfLegI = CE->GetGateAddrOfLeg(CE->GetNofLegs() - i - 1);
			if(AddrOfLegI == NULL)
			{
				NodeMapHead[FLrow +i][FLcol][0].ResetIndex(FLstride); //N/A leg only fills physically.
				continue;//It is a N/A leg, so proceed to next leg without doing nothing.
			}
			RemoveHole(FLrow+i,FLcol,AddrOfLegI, NULL);
		}

		break;
	case cetLED:
		//remove and delete BBNodes of V and G for up and down leg and unmark the corresponding Node Map bits
		for(i = 0 ; i<2 ; i++)
		{
			AddrOfLegI = CE->GetGateAddrOfLeg(i);
			RemoveHole(FLrow+i,FLcol,AddrOfLegI, NULL);
		}
		break;
	case cetCABLE:
		//remove dublication Od, if there is one at any side of the cable.
		//unmark corrs. node map bit.

		unsigned int SLrow = CE->GetPositionY2(); //CABLE second leg row position.
		unsigned int SLcol = CE->GetPositionX2()/(nof_holes_in_a_node + 2); //CABLE second leg col position.
		unsigned int SLstride = CE->GetPositionX2()%(nof_holes_in_a_node + 2); //CABLE second leg stride position.

		unsigned int SELrow = CE->V.output?SLrow:FLrow;
		unsigned int SELcol = CE->V.output?SLcol:FLcol;
		unsigned int OTHERrow = CE->V.output?FLrow:SLrow;
		unsigned int OTHERcol = CE->V.output?FLcol:SLcol;

		//remove the cable leg's node
		RemoveHole(SELrow,SELcol, &CE->V, NULL);

		//UNPLUGGING CABLE LEG
		if(CE->CEInfo[ceqHover2 - CE->V.output])
		{//if other is not plugged

			//delete report if any
            CE->V.Input[0] = NULL;

			//Cable Node Id setting
			CE->V.leg_no = CE->GetID(); //other leg is hover and SEL leg is also hover, so get initial CNID,which is the CEID.
		}
		else
		{//if other is plugged

			//if there is report
			if(CE->V.Input[0])
			{
				//if the removing (this) side has Od (Other node has original output O)
                if(CE->CEInfo[ceqHasOd1 + CE->V.output])
				{//remove Od from this and trigger >other cables!< in this node

					RemoveHole(SELrow,SELcol,CE->V.Input[0], NULL);		//remove Od and trigger remaining cables
						CE->CEInfo.ResetIndex(ceqHasOd1 + CE->V.output);	//indicate that this has no duplication.

				}
				//if other has Od (removing (this) leg has original output O)
				else
				{//remove Od from other and trigger cables in that node

					RemoveHole(OTHERrow, OTHERcol, CE->V.Input[0],CE); //remove Od, and trigger other cables in OTHER node.

					CE->V.Input[0] = NULL; //delete report
					CE->CEInfo.ResetIndex(ceqHasOd2 - CE->V.output); //OTHER has no more dublication
				}
			}

			//and send this CEID as CNID to other leg's node.
			CurrentCableNodeID = CE->GetID();
            SendCableNodeID(OTHERrow,OTHERcol, NULL);

		}

		//(Unplugging Side) set CNID of the remaining cables as the first cabe's CEID in the list.
		if(NodeHead[SELrow][SELcol]!=NULL && NodeHead[SELrow][SELcol]->GetGateInfo()->GetGateType()==gtCable)
		{
			//diffuse the ID of first plugged CABLE.
			CurrentCableNodeID = ((CircuitElement*)(NodeHead[SELrow][SELcol]->GetGateInfo()->next_gate ))->GetID();
			SendCableNodeID(SELrow,SELcol,NULL);
		}

		/*printf("\nLISTING AFTER UNPLUGGING");
		printf("\nSELHasOd?= %d OTHERHasOd? = %d", CE->CEInfo[ceqHasOd1 + CE->V.output], CE->CEInfo[ceqHasOd2 - CE->V.output]);
		printf("\nSEL corrs. BB Node:");
		ListBBNode(SELrow,SELcol);
		printf("\nOTHER corrs. BB Node:");
		ListBBNode(OTHERrow,OTHERcol);
		SetLegPlugAbility(CE);*/

		break;
	}

	//printf("\nPrinting BBDS after UnPlugCE() (CE = %s:%p)",CE->GetName(),CE);
	//debugprint();
}
Gate *BreadBoard::IsThereOutputGateInBBNode(unsigned int BBrow, unsigned int BBcol)
{
    for(BBNode *pick_HOLE = NodeHead[BBrow][BBcol]; pick_HOLE!=NULL; pick_HOLE = pick_HOLE->GetNextHole())
	{
		if(Gate::IsOutputtingGate(pick_HOLE->GetGateInfo()->GetGateType()))
		{
			//printf("\nBB:IsThereOutputtingLeg(): YES");//DEBUG
			return pick_HOLE->GetGateInfo();
		}
	}
	//printf("\nBB:IsThereOutputtingLeg(): NO");//DEBUG
	return NULL;
}
void BreadBoard::debugInsertVG(Gate *V,Gate *G)
{//DEBUG
    NewCurrentHole();
	SetCurrentHole(V);
	AddCurrentHole(0,0,0, NULL);

    NewCurrentHole();
	SetCurrentHole(G);
	AddCurrentHole(1,0,0, NULL);
}

void BreadBoard::RemoveHole( unsigned int BBrow, unsigned int BBcol, Gate *RemGate, CircuitElement* DontTriggerCable)
{
	BBNode *pick_HOLE, *IsolatedBBNode;

	//isolation of BBNode of the corresponding gate from BBNode list:
	pick_HOLE = NodeHead[BBrow][BBcol];

	if(pick_HOLE->GetNextHole()==NULL)
	{//if this is the last leg in the node, just remove it
		NodeHead[BBrow][BBcol] = NULL;
		NodeMapHead[BBrow][BBcol][0].ResetIndex(pick_HOLE->GetNodeMapIndex());
		delete pick_HOLE;
		return; //go on to the next leg.
	}
	else if( pick_HOLE->GetGateInfo() == RemGate)
	{//if it is the first leg in the list.
		IsolatedBBNode = NodeHead[BBrow][BBcol];
		NodeHead[BBrow][BBcol] = IsolatedBBNode->GetNextHole();
	}
	else //if it is in the middle of the list or at the end of the list.
	{
		//proceed onto the previous of the BBNode which will be isolated.
		for(pick_HOLE = NodeHead[BBrow][BBcol]; pick_HOLE->GetNextHole()->GetGateInfo() != RemGate ; pick_HOLE = pick_HOLE->GetNextHole());
		IsolatedBBNode = pick_HOLE->GetNextHole();
		pick_HOLE->SetNextHole(IsolatedBBNode->GetNextHole()); // <<<isolating...
	}
	//end of isolation of the unplugging leg.

	//adjust the connections other than the isolated(unplugging) one:
	if(Gate::IsOutputtingGate(IsolatedBBNode->GetGateInfo()->GetGateType()))
	{//the isolated leg is an outputting gate => make all (input) gates in the BBNode list point to NULL
		//pass cables
		for(pick_HOLE = NodeHead[BBrow][BBcol]; pick_HOLE!=NULL && pick_HOLE->GetGateInfo()->GetGateType()==gtCable; pick_HOLE=pick_HOLE->GetNextHole());
		for(;pick_HOLE != NULL; pick_HOLE = pick_HOLE->GetNextHole())
			pick_HOLE->GetGateInfo()->Connect(0, NULL);
		//if there any cable in the BBNode list, trigger them to make necessary alterations over other connected nodes.
		for(pick_HOLE = NodeHead[BBrow][BBcol];  pick_HOLE != NULL && pick_HOLE->GetGateInfo()->GetGateType()==gtCable; pick_HOLE = pick_HOLE->GetNextHole())
			if(DontTriggerCable == NULL || (CircuitElement*)pick_HOLE->GetGateInfo()->next_gate != DontTriggerCable)
				CalculateCable((CircuitElement*)(pick_HOLE->GetGateInfo()->next_gate), BBrow, BBcol, false);
	}
	else if(IsolatedBBNode->GetGateInfo()->GetGateType() != gtCable)//if the isolated leg is an inputting gate other than gtCable
	{//only make this input gate point to NULL
		IsolatedBBNode->GetGateInfo()->Connect(0, NULL);
	}

	//reset the corresponding NodeMap bit
	NodeMapHead[BBrow][BBcol][0].ResetIndex(IsolatedBBNode->GetNodeMapIndex());
	//destroy the isolated BBNode
	delete IsolatedBBNode;
}
//CALCULATECABLE WORKS, if:
//1) triggered by cable plug/unplug, it occurs when it is plugged or unplugged.
//2) triggered by Output gates,it occurs when an Output gate insertion/deletion is made.
void BreadBoard::CalculateCable(CircuitElement *Cable, unsigned int OutputRow, unsigned int OutputCol, bool OutputInsertion)
{
	unsigned int FLrow =	Cable->GetPositionY();								//first leg row coord.
	unsigned int FLcol =	Cable->GetPositionX()/(nof_holes_in_a_node+2);		//first leg col coord.
	unsigned int FLstride = Cable->GetPositionX()%(nof_holes_in_a_node+2);		//first leg stride coord.
	unsigned int SLrow =	Cable->GetPositionY2();								//CABLE second leg row position.
	unsigned int SLcol =	Cable->GetPositionX2()/(nof_holes_in_a_node + 2);	//CABLE second leg col position.
	unsigned int SLstride = Cable->GetPositionX2()%(nof_holes_in_a_node + 2);	//CABLE second leg stride position.


	bool triggered_leg = (OutputRow==SLrow)&&(OutputCol==SLcol); // 0 - leg_1 (FL), 1 - leg_2 (SL)
	unsigned int OTHERrow = triggered_leg?FLrow:SLrow;
	unsigned int OTHERcol =	triggered_leg?FLcol:SLcol;
	unsigned int OTHERstride = triggered_leg?FLstride:SLstride;


	if(OutputInsertion)
	{//if an O is inserted
		//printf("\nCalcCable(%p:%d) O insertion request : triggered from r.%d,c.%d",Cable,Cable->GetID(),OutputRow,OutputCol); //DEBUG

		//Report that the O gate has been had
		Cable->V.Input[0] = IsThereOutputGateInBBNode(OutputRow,OutputCol);
		//indicate that the leg other than the triggered has the Od
		Cable->CEInfo.SetIndex(ceqHasOd2 - triggered_leg);

		//printf(" -- Duplication request to other node r.%d,c.%d",OTHERrow,OTHERcol); //DEBUG
		//prepare BBNode for Od
		NewCurrentHole();
		SetCurrentHole(Cable->V.Input[0]);
        AddCurrentHole(OTHERrow,OTHERcol,CurrentBBstride, Cable);// call adding without triggering this cable.
		//the BBNode Od does not occupy physical hole; that's why the stride value is CurrentBBstride.

		//now, the cable is in the hasOutput state(i.e. it conducts)
	}
	else
	{//if an O is removed
		//printf("\nCalcCable(%p:%d) O removal request : triggered from r.%d,c.%d",Cable,Cable->GetID(),OutputRow,OutputCol); //DEBUG

		//printf(" -- Removing Od request to other node r.%d,c.%d",OTHERrow,OTHERcol); //DEBUG
		//remove the remaining Od from other node as no giving a triggering to itself.
		RemoveHole(OTHERrow,OTHERcol, Cable->V.Input[0], Cable);

		//indicate that the cable has no Od any more
		Cable->CEInfo.ResetIndex(ceqHasOd2 - triggered_leg);
		//Report that there is no O gate any more
		Cable->V.Input[0] = NULL;

		//now, the cable is in the hasNoOuput state(i.e. it doesn't conduct)
	}

}

void BreadBoard::ListBBNode(unsigned int BBrow, unsigned int BBcol)
{
    BBNode *pick_HOLE = NodeHead[BBrow][BBcol];
	printf("\nHole[%d][%d] Inside (",BBrow,BBcol);

	NodeMapHead[BBrow][BBcol]->debugprint();

	printf(")");

	for(;pick_HOLE!=NULL; pick_HOLE = pick_HOLE->GetNextHole())
		printf("\nGateType = %d,   ",pick_HOLE->GetGateInfo()->GetGateType());
	printf("\n");
}
unsigned int FLrow,FLcol,SLrow,SLcol;
CircuitElement *Cable;

void BreadBoard::SendCableNodeID(unsigned int BBrow,unsigned int BBcol, CircuitElement *DontTriggerCable)
{//diffuses the CNID over the given node coord. in order to distinguish Cable Nodes.
	//printf("\nCNID(%d) diffuse request at r.%d,c.%d donttouchcable(%p)",CurrentCableNodeID, BBrow, BBcol, DontTriggerCable); //DEBUG

	//for each cable on the node (BBrow,BBcol) except the cable DontTriggerCable:
	for(BBNode *pick_HOLE = NodeHead[BBrow][BBcol]; pick_HOLE != NULL && pick_HOLE->GetGateInfo()->GetGateType() == gtCable; pick_HOLE = pick_HOLE->GetNextHole())
	{
		if((CircuitElement*)(pick_HOLE->GetGateInfo()->next_gate) != DontTriggerCable)
		{
			
			pick_HOLE->GetGateInfo()->leg_no = CurrentCableNodeID;//give Cable Node ID to the cable			
			//send the ID to other node also
			Cable = (CircuitElement*)(pick_HOLE->GetGateInfo()->next_gate);
			FLrow =	Cable->GetPositionY();								//first leg row coord.
			FLcol =	Cable->GetPositionX()/(nof_holes_in_a_node+2);		//first leg col coord.
			SLrow =	Cable->GetPositionY2();								//CABLE second leg row position.
			SLcol =	Cable->GetPositionX2()/(nof_holes_in_a_node + 2);	//CABLE second leg col position.
			
			//printf("\nTrg->Cable(%p,l1(r.%d,c.%d),l2(r.%d,c.%d)) CNID : %d",pick_HOLE->GetGateInfo()->next_gate,FLrow,FLcol,SLrow,SLcol,pick_HOLE->GetGateInfo()->leg_no); // DEBUG

			if(BBrow == FLrow && BBcol == FLcol)
			{
				if(!Cable->CEInfo[ceqHover2])//if second leg is plugged, affect the others on the second leg's node.
				{
					//printf("\nrequesting other node (r.%d,c.%d)",SLrow,SLcol); //DEBUG					
					SendCableNodeID(SLrow,SLcol,Cable);
					//printf("\nEnd of CNID diffuse request at (%d,%d)",SLrow,SLcol); //DEBUG
				}
			}
			else
			{
				if(!Cable->CEInfo[ceqHover1])//if first leg is not hover affect the others on the first leg's node.
				{
					//printf("\n requesting other node (r.%d,c.%d)",FLrow,FLcol); //DEBUG
                    SendCableNodeID(FLrow,FLcol,Cable);
					//printf("\nEnd of CNID diffuse request at (%d,%d)",FLrow,FLcol); //DEBUG
				}
			}
			
		}
	}
}

void BreadBoard::debugListCableNodes(CircuitElement *Cable)
{
	if(Cable->GetType() != cetCABLE) return;
	//list the node lists under the corresponding cable legs.
	unsigned int FLrow =	Cable->GetPositionY();								//first leg row coord.
	unsigned int FLcol =	Cable->GetPositionX()/(nof_holes_in_a_node+2);		//first leg col coord.	
	unsigned int SLrow =	Cable->GetPositionY2();								//CABLE second leg row position.
	unsigned int SLcol =	Cable->GetPositionX2()/(nof_holes_in_a_node + 2);	//CABLE second leg col position.

	BBNode *pick_HOLE;
	unsigned int i;

    //list the first leg's node.
	printf("\nLISTING FIRST LEG'S NODE OF CABLE %p at (%d,%d)...\n", Cable, FLrow,FLcol);
	if(pick_HOLE = NodeHead[FLrow][FLcol])
	{
        for(i=0;pick_HOLE!=NULL; pick_HOLE = pick_HOLE->GetNextHole(), i++)
		{
			printf("\n  BBNode %d :", i);
			printf("\n  Pointing Gate Address : %p", pick_HOLE->GetGateInfo());
			if(pick_HOLE->GetGateInfo()->GetGateType() == gtCable) 
				printf(" (CE - %p)",pick_HOLE->GetGateInfo()->next_gate);
			printf("\n  Pointing Gate Type : %s", GateNames[pick_HOLE->GetGateInfo()->GetGateType()]);
			if(pick_HOLE->GetGateInfo()->GetGateType() == gtInput)
				printf(" (getting input from %p)",pick_HOLE->GetGateInfo()->Input[0]);
			printf("\n  Node Map Index : %d", pick_HOLE->GetNodeMapIndex());
			if(Gate::IsOutputtingGate(pick_HOLE->GetGateInfo()->GetGateType()))
				if(pick_HOLE->GetNodeMapIndex() == CurrentBBstride)
					printf(" (diffuse)");
				else
					printf(" (original)");
		}
		printf("\n");
	}
	else printf("\n  empty node list.\n");
	printf("\nEND OF FIRST LEG'S NODE LIST.");

	//list the second leg's node.
	printf("\nLISTING SECOND LEG'S NODE OF CABLE %p at (%d,%d)...\n", Cable, SLrow,SLcol);
	if(pick_HOLE = NodeHead[SLrow][SLcol])
	{
        for(i=0;pick_HOLE!=NULL; pick_HOLE = pick_HOLE->GetNextHole(), i++)
		{
			printf("\n  BBNode %d :", i);
			printf("\n  Pointing Gate Address : %p", pick_HOLE->GetGateInfo());
			if(pick_HOLE->GetGateInfo()->GetGateType() == gtCable) 
				printf(" (CE - %p)",pick_HOLE->GetGateInfo()->next_gate);
			printf("\n  Pointing Gate Type : %s", GateNames[pick_HOLE->GetGateInfo()->GetGateType()]);
			if(pick_HOLE->GetGateInfo()->GetGateType() == gtInput)
				printf(" (getting input from %p)",pick_HOLE->GetGateInfo()->Input[0]);
			printf("\n  Node Map Index : %d", pick_HOLE->GetNodeMapIndex());
			if(Gate::IsOutputtingGate(pick_HOLE->GetGateInfo()->GetGateType()))
				if(pick_HOLE->GetNodeMapIndex() == CurrentBBstride)
					printf(" (diffuse)");
				else
					printf(" (original)");
		}
		printf("\n");
	}
	else printf("\n  empty node list.\n");
	printf("\nEND OF SECOND LEG'S NODE LIST.");

}