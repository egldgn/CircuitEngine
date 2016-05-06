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
	CEngine.h : The commander of other classes. He manages the CEs and
				BreadBoard, and handles communication and organization
				between them with the help of Artist.h.
*/
#pragma once
#include "BreadBoard.h"
#include "CircuitElement.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <io.h>

//CEC file buffer definitions

typedef struct CircuitElementHeadBuf
{
	CEtype type;
	unsigned char CEInfo;
	char name [27];
	unsigned short V_leg_no;
	unsigned short G_leg_no;
	unsigned int Position_X1;
	unsigned int Position_Y1;
	unsigned int nof_inputs;
	unsigned int nof_outputs;
	unsigned int nof_gates;
} CEH_BUF;

typedef struct GateHeadBuf
{
	GateType type;
	unsigned short nof_inputs;
	unsigned short leg_no;
} GH_BUF;

typedef struct BreadBoardBuf
{
	unsigned int rows;
	unsigned int cols;
	unsigned int nof_holes_in_a_node;
} BB_BUF;

//struct camcone is also used for the last cam position

//eof CEC file definitions


enum TypeInsideChipBlock {ticbVoltage,ticbGround,ticbInput,ticbGate,ticbEND,ticbEndchip,ticbEmpty};

class CECircuit //Circuit Engine Circuit.
{
	friend void CalculateTraverseCircuit(void);
	friend void AddCable(void);
	friend void AddLED(void);
	friend void AddSourceBar(bool SourceBit);
	friend void SpecialKeys(int key, int x, int y);
	friend void InitArtist();
	friend void initOGL(void);
	friend void DrawCircuit(void);
	friend void Keyboard(unsigned char key, int x, int y);
protected:
	CircuitElement *circuit; // the circuit
	BreadBoard *theBreadBoard; // the bread-board
	unsigned int nof_elements; //nof CEs added into the circuit.

private:

	//Parse Check Variables used by ParseScriptFile() (CEngine Parser).
	CircuitElement *CurrentCE; // temporary place for compilation procedure.
	Gate *CurrentGate; // is the last added into a circuit element.(for connection purpose)
	FILE *ScriptFile;
	//SAVE FILE WILL BE HERE ALSO.

	//gate connection variables. using by AddGate() and ParseScriptFile().
	static unsigned int creation_counter; //is to find the <creation_ordered>th gate in the CurrentCE.
	static unsigned int connection_counter; //is for Index increment of Input array of the current Gate which is adding into the CurrentCE.

	void GiveError(char *e, unsigned int line = 0,char *add = NULL);
	void GiveMsg(char *msg);
	void GiveCompMsg(char *msg,char *a1 = NULL,char *a2 = NULL, char *a3 = NULL, char *a4 = NULL);
	GateType ChrToGateType(char * GateName); // for script parsing.

public:
	CECircuit();
	~CECircuit();

	//Script file operations.
	void ParseScriptFile(char *file_path);//Compiles CESL file
	void SaveCircuit(void); //saves current circuit as CEC file
	void LoadCircuit(char *file_path); //Loads CEC file

	//These 4 methods is for Circuit Element Insertion Block.
	void NewCurrentCE(void);
	void AddGate(GateType new_type,unsigned short new_nof_inputs, unsigned short new_leg_no);
	void Connect(GateType gate_type, unsigned creation_order);
	void AddCurrentCE(void); //Add Current circuit element into the circuit.
	void DeleteCurrentCE(void);
	void RemoveCE(CircuitElement *CE);
	void SetLegPlugAbilityOfCurrentCE (void);

	void PrintCurrentCircuit(void);
	void InsertTempVG(void);

	void debugPrintCable(void)
	{
		CircuitElement *cable = circuit;

		for(;cable->GetType() != cetCABLE; cable = cable->GetNext());

		for(;cable != circuit->GetNext();cable = cable->GetNext())
		{
			printf("\n\nCABLE %p (%d,%d)",cable,cable->GetPositionX(),cable->GetPositionY());
            printf("\nCable Node ID = %d",cable->V.leg_no);
		}

	}

};

unsigned int CECircuit::creation_counter = 0;
unsigned int CECircuit::connection_counter = 0;

void CECircuit::GiveError(char *e, unsigned int line, char *add)
{
	char *space = "";
	if(!line)
		printf("\n(e)(CEngine): %s.",e);
	else
	{
		if(add == NULL) add = space;
		printf("\n(e)(CEngine) Syntax Error on Line(%d) : %s%s.",line,e,add);
	}

	this->~CECircuit();
	printf("\n(!)(CEngine) : CEngine has been halted.");

	exit(0);
}

void CECircuit::GiveMsg(char *msg)
{
	printf("\n(i)(CEngine): %s.",msg);
}

void CECircuit::GiveCompMsg(char *msg,char *a1,char *a2, char *a3, char *a4)
{
	char buffer[200];
	sprintf(buffer,msg,a1,a2,a3,a4);
	printf("\n(c)(CEngine): %s.",buffer);
}

void CECircuit::PrintCurrentCircuit(void)
{
	unsigned int nof_e = nof_elements;
	CircuitElement *pick_ce = circuit;
	printf("\n\nLISTING CURRENT CIRCUIT ELEMENTS:\n");
	printf("\nBREADBOARD : %d rows, %d cols and %d holes in a node.\n", CurrentBBrow,CurrentBBcol,CurrentBBstride);
	do
	{
		pick_ce->print();
		pick_ce = pick_ce->GetNext();
		printf("\n");
		nof_e--;
	}
	while(nof_e);

	printf("\nEND OF CIRCUIT LIST\n\n",nof_elements);
}

CECircuit::CECircuit()
{
	theBreadBoard = NULL;
	CurrentCE = NULL;
	circuit = NULL;
	nof_elements = 0;
	ScriptFile = NULL;
	CurrentGate = NULL;

	//Create Source CE (just V&G).
	GiveMsg("Creating source level (V&G)...");
	NewCurrentCE();
	CurrentCE->SetName("Source(V&G)");
	AddGate(gtVoltage,0,0);
	AddGate(gtGround,0,0);
	AddCurrentCE();
}

CECircuit::~CECircuit()
{
	CircuitElement *pick_ce = circuit;
	CircuitElement *prev_element = NULL;

	for(unsigned int i=0; i<nof_elements; i++)
	{
		prev_element = pick_ce;
		pick_ce = pick_ce->GetNext();
		delete prev_element;
	}
	circuit = NULL;

	if(CurrentCE != NULL) DeleteCurrentCE();
	if(theBreadBoard!=NULL) theBreadBoard->~BreadBoard();
}

void CECircuit::ParseScriptFile(char *file_path)
{
	char buf[512];

	Binary LegAssociations; // 'set state' of each leg number is hold in index (leg_number - 1).i.e. starts from zero instead of one.
	unsigned short total_number_of_legs;
	bool Current_CHIP_has_NOTAVAILABLE_leg;
	unsigned int current_chip_nof_inputs, current_chip_nof_outputs;

	CEtype				TInsideScript;
	TypeInsideChipBlock	TInsideChipBlock;
	GateType			TInsideGateBlock;

	unsigned short depth = 0; // 0 in script,1 in chip,2 in gate blocks.

	unsigned long line_counter;
	char *token;
	char *delims = "	\n ";//delimiters are 'tab, new-line, space'

	unsigned int ce_leg_no;
	unsigned int BBrow,BBcol,BBstride;

	//variables for connection process in depth 1:ticbEndchip 
	Gate *pick_GATE;
	Gate *connect_gate;
	unsigned int i,j;


	if(file_path == NULL) GiveError("no given script file");
	if((ScriptFile = fopen(file_path,"r")) == NULL)
	{
		GiveCompMsg("Script file couldn't be found (%s)", file_path);
		GiveError("CEngine parser is halted");
	}

	GiveCompMsg("compilation started (%s)...",file_path);

	for(line_counter = 1; !feof(ScriptFile); line_counter++) //find and load the breadboard configuration.
	{
		fgets(buf,511,ScriptFile); //get line.
		token = strtok(buf,delims); // go on to the first token on the line.

		if(token == NULL || !strcmp(token,"//")) // an empty line or a comment.
			continue;

		//Get BreadBoard Information (row, col, stride)
		 if(strcmp(token,"BREADBOARD")) //first statement in the CESL file must be the BREADBOARD row col stride
             GiveError("First statement in the .CESL file must be BREADBOARD <row> <col> <nof_holes_in_one_node>",line_counter);

		 //resolve the BreadBoard dimensions
		 token = strtok(NULL,delims); //on <BBrow>.
		 if(token == NULL || !strncmp(token,"//",1)) GiveError("Unexpected end of line, there must be <BBrow> <BBcol> <nof_holes_in_one_node>",line_counter);
		 BBrow = atoi(token);
		 if(BBrow == 0) GiveError("Invalid <BBrow>",line_counter);

		 token = strtok(NULL,delims); //on <BBcol>.
		 if(token == NULL || !strncmp(token,"//",1)) GiveError("Unexpected end of line, there must be <BBcol> <nof_holes_in_one_node>",line_counter);
		 BBcol = atoi(token);
		 if(BBcol < 2) GiveError("Invalid <BBcol>, must be at least 2",line_counter);

		 token = strtok(NULL,delims); //on <BBstride>.
		 if(token == NULL || !strncmp(token,"//",1)) GiveError("Unexpected end of line, there must be <nof_holes_in_one_node>",line_counter);
		 BBstride = atoi(token);
		 if(BBstride < 2) GiveError("Invalid <nof_holes_in_a_node>, must be at least 2",line_counter);

		 theBreadBoard = new BreadBoard(BBrow,BBcol,BBstride);
		 CurrentBBrow = BBrow;
		 CurrentBBcol= BBcol;
		 CurrentBBstride = BBstride;
		 line_counter++;
		 break; //eof Get BreadBoard Information		
	}

	for(; !feof(ScriptFile); line_counter++)
	{	
		fgets(buf,511,ScriptFile); //get line.
		token = strtok(buf,delims); // go on to the first token on the line.

		if(token == NULL || !strcmp(token,"//")) // an empty line or a comment.
			continue;

		else //TOKEN_ON_BLOCK_HEAD
		{
			if(depth == 0) // IN_SCRIPT_ROOT
			{
				TInsideScript    = !strcmp(token,"CHIP") ? cetCHIP : !strcmp(token,"BREADBOARD") ? cetBREADBOARD : cetEmpty;

				if(TInsideScript != cetBREADBOARD)
				{
					NewCurrentCE();
					CurrentCE->SetType(TInsideScript);
				}

				switch(TInsideScript)
				{
				case cetCHIP://CHIP <ce_name> <ce_nof_inputs> <ce_nof_outputs>

					//Place into the initial position.
					CurrentCE->MoveTo(theBreadBoard->GetStrideValue()-1,0);//initially a Chip is on the first 'between cols'(on the first Hollow and will always be moved over hollows...)

					token = strtok(NULL,delims);//on name.
					if(token == NULL || !strncmp(token,"//",1)) GiveError("Unexpected end of line, there must be <ce_name> <ce_nof_inputs> <ce_nof_outputs>",line_counter);
					CurrentCE->SetName(token);

					token = strtok(NULL,delims); //on nof_inputs.
					if(token == NULL || !strncmp(token,"//",1)) GiveError("Unexpected end of line, there must be <ce_nof_inputs> and <ce_nof_outputs>",line_counter);
					current_chip_nof_inputs = atoi(token);
					if(current_chip_nof_inputs == 0) GiveError("invalid number of inputs", line_counter);
					CurrentCE->SetNofInputs(current_chip_nof_inputs);

					token = strtok(NULL,delims); //on nof_outputs.
					if(token == NULL || !strncmp(token,"//",1)) GiveError("Unexpected end of line. no <ce_nof_outputs>",line_counter);
					current_chip_nof_outputs = atoi(token);
					if(current_chip_nof_outputs == 0) GiveError("invalid number of outputs", line_counter);
					CurrentCE->SetNofOutputs(current_chip_nof_outputs);

					depth = 1;//down to depth 1 : IN_CHIP_BLOCK
					GiveCompMsg("CE %s:%s is now setting up by in-script information ...",CEnames[CurrentCE->GetType()],CurrentCE->GetName());

					//initialize binary leg association control array which is used during parsing.
					total_number_of_legs = current_chip_nof_inputs + current_chip_nof_outputs + 2; // here + 2 for V&G legs of the chip.
					total_number_of_legs += (Current_CHIP_has_NOTAVAILABLE_leg = (total_number_of_legs % 2) ? true : false) ? 1L : 0L; // make chip even number of legs.
					if( (unsigned int)(total_number_of_legs/2) > CurrentBBrow) GiveError("Chip size exceeds breadboard rows -> ",line_counter,CurrentCE->GetName());
					LegAssociations.CreateBitArray( total_number_of_legs + 2);

					//>LegAssociations array STARTS FROM ZERO. Set bit of a leg is (leg_number - 1).
					//>explanation for (+2) more bits in the array, These bits are special and have different meanings than other bits:
					//>index total_number_of_legs = Is voltage leg set? (index before last)
					//>index total_number_of_legs = Is ground leg set? (last index)

					break;

				case cetBREADBOARD:	GiveError("BREADBOARD statement is duplicated",line_counter); break;
				case cetEmpty:		GiveError("Unrecognized CESL keyword!",line_counter); break;
				default:			GiveError("Unexpected CEngine Parser failure!",line_counter); break;
				}
			}//EOF IN_SCRIPT

			else if(depth == 1) //DEPTH 1: IN_CHIP_BLOCK.
			{//inside chip block. //token is on an inside-chip block.

				TInsideChipBlock = !strcmp(token,"VOLTAGE") ? ticbVoltage : !strcmp(token,"GROUND") ? ticbGround : !strcmp(token,"INPUT") ? ticbInput : !strcmp(token,"GATE") ? ticbGate : !strcmp(token,"END") ? ticbEndchip : ticbEmpty;

				if(CurrentCE == NULL) GiveError("Element without CHIP block!.",line_counter);

				switch(TInsideChipBlock)
				{
				case ticbVoltage:

					token = strtok(NULL,delims); // token is on <ce_leg_no>(Voltage connection)
					if(token == NULL || !strncmp(token,"/",1)) GiveError("Unexpected end of line. Voltage must have <ce_leg_no>",line_counter);
					ce_leg_no = atoi(token);
					if(ce_leg_no == 0) GiveError("Voltage must have <ce_leg_no> (other than 0)",line_counter);

					if( ce_leg_no > total_number_of_legs ) GiveError("Specified legs are greater than the total number of legs declared in the CHIP header",line_counter);
					if( LegAssociations[ce_leg_no-1] ) GiveError("The leg number is assigned more than once",line_counter);
					LegAssociations.SetIndex(ce_leg_no - 1); // indicate that this leg is occupied.

					LegAssociations.SetIndex( total_number_of_legs ); //indicate that V leg is set.
					CurrentCE->SetVoltageLegNo(ce_leg_no);

					break;

				case ticbGround:

					token = strtok(NULL,delims); // token is on <ce_leg_no>(Ground connection)
					if(token == NULL || !strncmp(token,"/",1)) GiveError("Unexpected end of line. Ground must have <ce_leg_no>",line_counter);
					ce_leg_no = atoi(token);
					if(ce_leg_no == 0) GiveError("Ground must have <ce_leg_no> (other than 0)",line_counter);

                    if( ce_leg_no > total_number_of_legs ) GiveError("Specified legs are greater than the total number of legs declared in the CHIP header",line_counter);
					if( LegAssociations[ce_leg_no-1] ) GiveError("The leg number is assigned more than once",line_counter);
					LegAssociations.SetIndex(ce_leg_no - 1); // indicate that this leg is occupied.

					LegAssociations.SetIndex( total_number_of_legs + 1 ); //indicate that G leg is set.
					CurrentCE->SetGroundLegNo(ce_leg_no);

					break;

				case ticbInput:

					token = strtok(NULL,delims); // token is on <ce_leg_no>(Input connection)
					if(token == NULL || !strncmp(token,"/",1)) GiveError("INPUT must be associated a <ce_leg_no>",line_counter);					
					ce_leg_no = atoi(token);
					if(ce_leg_no == 0) GiveError("INPUT must have <ce_leg_no> (other than 0)",line_counter);

					if( ce_leg_no > total_number_of_legs ) GiveError("Specified legs are greater than the total number of legs declared in the CHIP header",line_counter);
					if( LegAssociations[ce_leg_no-1] ) GiveError("The leg number is assigned more than once",line_counter);
					LegAssociations.SetIndex( ce_leg_no-1 ); // indicate that this leg is occupied.

					CurrentCE->AddGate(gtInput,1,ce_leg_no);
                    if(current_chip_nof_inputs-- == 0) GiveError("Specified number of inputs does not match the declared in CHIP block header",line_counter);

					break;

				case ticbGate://GATE <gtType> <gt_nof_inputs> [output <ce_leg_no>]

					GateType TCurrentGate;
					unsigned short nof_inputs;

					token = strtok(NULL,delims);// <gtType> .
					if(token == NULL || !strncmp(token,"/",1)) GiveError("Unexpected end of line. > GATE <gt_type> <nof_inputs> [output <ce_leg_no>]",line_counter);
					TCurrentGate = Gate::ChrToGateType(token); // get gtType.

					token = strtok(NULL,delims); // <gt_nof_inputs>
					if(token == NULL || !strncmp(token,"/",1)) GiveError("Unexpected end of line. > GATE <gt_type> <gt_nof_inputs> [output <ce_leg_no>]",line_counter);
					nof_inputs = atoi(token);
					if(nof_inputs == 0) GiveError("Invalid number of inputs for gate",line_counter,GateNames[TCurrentGate]);

					ce_leg_no = 0; //if no [output] part, zero will be the the output leg number (i.e. no outputting gate).
					token = strtok(NULL,delims); // [output <ce_leg_no>] or CONNECTION.
					if(token != NULL && !strcmp(token,"output")) // if optional output leg is specified.
					{
						token = strtok(NULL,delims); // token is on <ce_leg_no> (output leg number).
						if(token == NULL || !strncmp(token,"/",1)) GiveError("Unexpected end of line",line_counter);
						ce_leg_no = atoi(token);
						if(ce_leg_no == 0) GiveError("Invalid output leg declaration for gate",line_counter,GateNames[TCurrentGate]);

						if( ce_leg_no > total_number_of_legs ) GiveError("Gate leg number exceeds the total number of legs declared in the CHIP header",line_counter);
						if( LegAssociations[ce_leg_no-1] ) GiveError("The leg number is assigned more than once",line_counter);
						LegAssociations.SetIndex( ce_leg_no-1 );
						if(current_chip_nof_outputs-- == 0) GiveError("Specified number of outputs does not match the declared in CHIP block header",line_counter);
					}

					//type , nof_inputs and output-legs are ready. Now Add the gate to CurrentCE.
					AddGate(TCurrentGate, nof_inputs, ce_leg_no);
					
					//this is the end of a GATE declaration (Gate Header), after that a new line
					//comes out. The connection part requires an new switch statement
					//with the use of TypeInsideGateBlock type.
					//thus, another else if structure is required for general parsing structure (depth 2)...

					depth = 2; // down to IN_GATE_BLOCK
												
					break;

				case ticbEndchip:

					//check if the "END" keyword concludes with "CHIP" keyword.if not, give block error.
					token = strtok(NULL,delims);
					if(token == NULL || !strncmp(token,"//",2)) GiveError("Unexpected end of line. Expecting keyword CHIP",line_counter);
					if(strcmp(token,"CHIP")) GiveError("Unrecognized 'END <ce_block>' block header",line_counter);
					if( !LegAssociations[ total_number_of_legs ] ) GiveError("The CHIP has not a VOLTAGE leg declaration",line_counter);
					if( !LegAssociations[ total_number_of_legs + 1 ] ) GiveError("The CHIP has not a GROUND leg declaration",line_counter);
					if( LegAssociations.GiveNofOnes() != (total_number_of_legs - Current_CHIP_has_NOTAVAILABLE_leg  + 2) )
					{	//number of ones in LegAssociations array must be equal to
						//tot_nof_legs - N/A(if there is one -- i.e. if user must omit one leg as N/A) + 2(special check bits of V&G, las two bits of the array)
						GiveError("defined nof inputs and outputs in header does't macth with nof leg associations, check nof_inputs, nof_outputs, voltage and ground leg specifications");
					}

					CurrentCE->LegPlugAbility.CreateBitArray(CurrentCE->GetNofLegs());

					_itoa(CurrentCE->GetID(),buf,10);
					GiveCompMsg("Gate List is OK for %s.%s.",CurrentCE->GetName(),buf);
					GiveCompMsg("Gate connection process is starting for %s.%s...",CurrentCE->GetName(),buf);

					//As explained in the IN_GATE_BLOCK, here is the converting process
					//of creation_no's into in-memory addresses before the CurrentCE
					//is added into the circuit:

                    //CONVERTING PROCESS (creation_no to in-memory-gate-address)					
					for(pick_GATE = CurrentCE->GetHead() ; pick_GATE!=NULL ; pick_GATE = pick_GATE->GetNext())
					{
                        if(pick_GATE->GetGateType() == gtInput) continue; //ignore input gate connection (it'll be done in CEngine)
						for(i=0; i<pick_GATE->GetNofInputs(); i++)
						{//connect corresponding input array element which it points to by its creation_no inside.
                            //proceed onto the specified gate by the creation_no.
							for(j=0,connect_gate = CurrentCE->GetHead(); j<(unsigned int)pick_GATE->Input[i] ; ++j,connect_gate = connect_gate->GetNext());
							//now the connect_gate is on the gate which is to be connected to pick_GATE->Input[i].
							pick_GATE->Input[i] = connect_gate;
						}

					}
					//eof CONVERTING PROCESS.

					GiveCompMsg("Gate connection process is completed.");

					AddCurrentCE(); // Chip declaration is completed. Add Current Circuit Element to the Circuit.					

					depth = 0; // up to IN_SCRIPT_ROOT.

					break;

				case ticbEmpty:	GiveError("Unrecognized CESL keyword : expected 'END CHIP'",line_counter);break;
				default:		GiveError("Unexpected CEngine failure!",line_counter);break;
				}
			}//EOF IN_CHIP_BLOCK

			else if(depth == 2) // DEPTH 2: IN_GATE_BLOCK.
			{

				TInsideGateBlock = !strcmp(token,"INPUT") ? gtInput :!strcmp(token,"AND") ? gtAND : !strcmp(token,"NAND") ? gtNAND : !strcmp(token,"OR") ? gtOR : !strcmp(token,"NOR") ? gtNOR : !strcmp(token,"XOR") ? gtXOR : !strcmp(token,"XNOR") ? gtXNOR : !strcmp(token,"NOT") ? gtNOT : !strcmp(token,"END") ? gtGround : gtEmpty;
			    //here gtGround is used to recognize 'END GATE' block.

				unsigned int creation_no;
				Gate *pick_gate = CurrentCE->GetHead();

				//current gate is the last inserted gate into the CE.
				switch(TInsideGateBlock)
				{
					case gtAND:
					case gtNAND:
					case gtOR:
					case gtNOR:
					case gtXOR:
					case gtXNOR:
					case gtNOT:
					case gtInput:

						if(connection_counter == CurrentGate->GetNofInputs())
							GiveError("number of inputs is too much",line_counter);

						token = strtok(NULL,delims);//token must be on <creation_no>
						if(token == NULL || !strncmp(token,"/",1)) GiveError("Expecting <creation_order>",line_counter);
						creation_no = atoi(token); //<creation_no of the gate to be connected.>
						
						//Connect(TInsideGateBlock,creation_no);

						//->instead of real address connection, put into input array the numerical connections
						//this will allow you to use the ability of sync/async circuit design of the data structure.
						//(a gate which is wanted to be connected to current gate may not be created(declared) currently yet...)
						//(later this numerical connections can be converted to real addresses by retraversing the CurrentCE.)

                        //previously the connection was made by (gtType, creation_no) pair
						//but now, it is made by (creation_no) only!!!
						//(in order to make later conversion of these creation_no's into real addresses)
						//UPDATE THIS ALSO IN THE CESL DOCUMENTATION!.

                        CurrentGate->Input[connection_counter++] = (Gate*)creation_no;
						//this will converted into actual address of gate which will be connected to this gate.
						//at the end of the CHIP block.

						break;

					case gtGround://!!!here gtGround is used to recognize End gate block.

						if(connection_counter < CurrentGate->GetNofInputs())
							GiveError("number of inputs are less than declared",line_counter);

						token = strtok(NULL,delims);
						if(token == NULL) GiveError("Unexpected end of line",line_counter);
						//check if END is concluded by GATE or not.
						if(strcmp(token,"GATE")) GiveError("Cannot found corresponding GATE block header");
						depth = 1; // up to IN_CHIP_BLOCK depth.
						break;

					case gtEmpty:	GiveError("Unrecognized CESL keyword : expected 'END GATE'",line_counter);break;
					default:		GiveError("Unexpected CEngine failure!",line_counter);break;
				}

			}//EOF IN_GATE_BLOCK

			else //UNKNOWN BLOCK DEPTH
				GiveError("Unknown script depth!");
		}//EOF TOKEN_ON_BLOCK_HEAD

	}//CHECK NEXT LINE.

	fclose(ScriptFile);

	GiveMsg("compilation completed...");

	this->PrintCurrentCircuit();
}

void CECircuit::NewCurrentCE(void)
{
	if(CurrentCE != NULL) GiveError("Invalid Circuit Element Block!. May be you forgot to AddCurrentCE()");

	CurrentCE = new CircuitElement();
}

void CECircuit::AddGate(GateType new_type,unsigned short new_nof_inputs, unsigned short new_leg_no)
{
	if(CurrentCE == NULL) GiveError("CurrentCE is not ready for gate insertion");
	//if(CurrentGate != NULL) GiveMsg("Invalid AddGate() block"); This is handled by the CEngine Parser (using connections less or greater than declared Input Array size of the current Gate)
	CurrentGate = CurrentCE->AddGate(new_type,new_nof_inputs,new_leg_no);
	connection_counter = 0; //connection_counter is for Input array index of the Gate class
}

void CECircuit::Connect(GateType gate_type, unsigned int creation_order)
{
	if(CurrentCE == NULL) GiveError("CurrentCE is not ready for connection setting");

	unsigned int i;
	Gate *pick_gate = CurrentCE->GetHead();

	creation_counter = 0;

	if(pick_gate == NULL)
		GiveError("Current Circuit Element is not ready");

	for(i=0; i<CurrentCE->GetNofGates(); i++) //including the last current gate.
	{
		if(pick_gate->GetGateType() == gate_type)
		{
			if(creation_order == creation_counter)
			{
				CurrentGate->Connect(connection_counter, pick_gate); //meanwhile point to next input.
				printf("\n(c)(CEngine): connecting gate [%s(I%d)] -> [%s_%d]",GateNames[CurrentGate->GetGateType()], connection_counter , GateNames[gate_type], creation_order);
				++connection_counter;
				break;
			}
			creation_counter ++;
		}

		pick_gate = pick_gate->GetNext();
	}

	if(i == CurrentCE->GetNofGates())
	{
		//printf("%p",pick_gate);
		GiveError("Connection couldn't be made. Wrong gate declaration.");
	}
}

void CECircuit::AddCurrentCE(void)
{
	if(CurrentCE == NULL) GiveError("AddCurrentCE(): couldn't be found corresponding NewCurrentCE()");
	if(circuit == NULL)
		circuit = CurrentCE;
	else
	{
		CircuitElement *ce_pick = circuit;
		for(unsigned int i=1; i<nof_elements; i++) ce_pick = ce_pick->GetNext();
		ce_pick->SetNext(CurrentCE);
		CurrentCE->SetPrev(ce_pick);
		CurrentCE->SetNext(circuit->GetNext());//maintain the circular doubly linked list.(source level is excluded)
		circuit->GetNext()->SetPrev(CurrentCE);
	}
	nof_elements ++;
	GiveCompMsg("CE %s:%s is added into circuit",CEnames[CurrentCE->GetType()], CurrentCE->GetName());
	CurrentCE = NULL;
}

void CECircuit::DeleteCurrentCE(void)
{
	if(CurrentCE != NULL)
	{
	delete CurrentCE;
	CurrentCE = NULL;
	}
}

void CECircuit::RemoveCE(CircuitElement *CE)
{
	if(CE == NULL) GiveError("Removing NULL CE is not possible! Removing nothing?, you must be kidding :)");
	switch(CE->GetType())
	{
	case cetCABLE:
		if(!CE->CEInfo[ceqHover1])
		{
			CE->V.output = 0; //select first leg.
			theBreadBoard->UnPlugCE(CE);//unplug the leg.
			CE->CEInfo.SetIndex(ceqHover1);
		}
		if(!CE->CEInfo[ceqHover2])
		{
            CE->V.output = 1;
			theBreadBoard->UnPlugCE(CE);
			CE->CEInfo.SetIndex(ceqHover2);
		}
		break;

	case cetLED:
		if(!CE->CEInfo[ceqHover])
		{
			theBreadBoard->UnPlugCE(CE);
			CE->CEInfo.SetIndex(ceqHover);
		}
		break;
	default : return;
	}

	CE->GetPrev()->SetNext(CE->GetNext());
	CE->GetNext()->SetPrev(CE->GetPrev());
	if(CE == circuit->GetNext()) circuit->SetNext(CE->GetNext());
	delete CE;
	nof_elements--;
	if(nof_elements == 1) circuit->SetNext(NULL);
}

void CECircuit::SaveCircuit(void)
{			
	BB_BUF bb_buf;
	CEH_BUF ceh_buf;
	GH_BUF gh_buf;	

	int CEC_file;
	unsigned int ce_count,i,creation_no;	
	CircuitElement *pick_CE;
	Gate *pick_Gate, *found_Gate;
	unsigned short *CableHeight; //for cable height to be overwritten onto the name field.
	
	//GIVE CEC EXTENSION TO FILE NAME
	char *extension;
	for(extension = ScriptFilePath + strlen(ScriptFilePath) - 1 ; *extension != '.'; extension--)
	{
		if(*extension == '\\' || extension == ScriptFilePath)
		{
            extension = ScriptFilePath + strlen(ScriptFilePath);
			break;
		}
	}
	strcpy(extension, ".cec");
	//eof GIVING EXTENSION


	//initialize the file. Corresponding CESL file gets the CEC extension
	CEC_file = _creat(ScriptFilePath, _S_IREAD | _S_IWRITE); //create or if exists truncate the file.
	

	if(CEC_file == -1)
	{
		perror("Error while creating the CEC file");
        exit(1);
	}
	_setmode(CEC_file,_O_BINARY);

	//write CEC file Identification
	write(CEC_file, "CEC_FILE:", 9);

	//Write Last Cam Location
	if(_write(CEC_file, &camcone, sizeof(CamCone)) < sizeof(CamCone))
	{
		_unlink(ScriptFilePath);
		GiveError("CEC file save error while writing Last Camera Location: %s",0,ScriptFilePath);
	}	

	//Write BB (BreadBoard) information
	bb_buf.rows = CurrentBBrow;
	bb_buf.cols = CurrentBBcol;
	bb_buf.nof_holes_in_a_node = CurrentBBstride;

	if(_write(CEC_file, &bb_buf, sizeof(BB_BUF)) < sizeof(BB_BUF))
	{
		_unlink(ScriptFilePath);
		GiveError("CEC file save error while writing BB information: %s",0,ScriptFilePath);
	}
	//eof BB information

	//write the last LastSelectedCE
		//search the list order of the last LastSelectedCE.
		for(ce_count = 0, pick_CE = circuit->GetNext(); pick_CE != LastSelectedCE; pick_CE = pick_CE->GetNext(),ce_count++);
		if(_write(CEC_file, &ce_count, sizeof(unsigned int)) < sizeof(unsigned int))
		{
			_unlink(ScriptFilePath);
			GiveError("CEC file save error while writing Last LastSelectedCE list order: %s",0,ScriptFilePath);
		}
	//eof write last LastSelectedCE.

	//Write the Circuit
	for(ce_count = 0,pick_CE = circuit->GetNext(); ce_count < nof_elements-1; ce_count++,pick_CE = pick_CE->GetNext())
	{//for each circuit element		

		//get information about the ce.
		ceh_buf.type = pick_CE->GetType();
		ceh_buf.CEInfo = pick_CE->CEInfo.GetByte(0);
		strcpy( ceh_buf.name, pick_CE->GetName());		
		ceh_buf.V_leg_no = pick_CE->V.leg_no;
		ceh_buf.G_leg_no = pick_CE->G.leg_no;
		ceh_buf.Position_X1 = pick_CE->GetPositionX();
		ceh_buf.Position_Y1 = pick_CE->GetPositionY();
		ceh_buf.nof_inputs = pick_CE->GetPositionX2();
		ceh_buf.nof_outputs = pick_CE->GetPositionY2();
		ceh_buf.nof_gates = pick_CE->GetNofGates();
		//eof ce header information
		
		if(pick_CE->GetType() == cetCABLE)
		{//for cables save the cable height info(CE->V.nof_inputs) into the name field.
            CableHeight = (unsigned short *) ceh_buf.name; 
			*CableHeight = pick_CE->V.nof_inputs;
		}

		//write the CEH (Circuit Element Header)
        if(_write(CEC_file, &ceh_buf, sizeof(CEH_BUF)) < sizeof(CEH_BUF))
		{
			_unlink(ScriptFilePath);
			GiveError("CEC file save error while writing CE Header: %s",0,ScriptFilePath);
		}		

		//Write GL (Gate List) (KIMT: for CABLE and LED, the gate head is NULL)
		for(pick_Gate = pick_CE->GetHead(); pick_Gate != NULL; pick_Gate = pick_Gate->GetNext())
		{//for each gate in this circuit element.

            gh_buf.leg_no = pick_Gate->GetLegNo();
			gh_buf.type = pick_Gate->GetGateType();
			gh_buf.nof_inputs = gh_buf.type == gtInput ? 0 : pick_Gate->GetNofInputs(); //dont save (CL) for gtInput.

			//Write GH (Gate Header)
			if(_write(CEC_file, &gh_buf, sizeof(GH_BUF)) < sizeof(GH_BUF))
			{
				_unlink(ScriptFilePath);
				GiveError("CEC file save error while writing the Gate Header: %s",0,ScriptFilePath);
			}					

			//Write CL (Connection List) (KIMT: for gtInput the (CL) is not saved)
			for(i=0 ; i < gh_buf.nof_inputs ; i++)
			{//for each connection write creation order of the connected gate.
				
				//find the list_order of the gate which gives output to this gate.
                for(creation_no=0, found_Gate = pick_CE->GetHead() ; found_Gate!=NULL; found_Gate = found_Gate->GetNext(), creation_no++)
					if(pick_Gate->Input[i] == found_Gate) //there is no possibility of a gate not to be connected to some other gate,
						break;								//so, it is not necessary to check the case if the gate not found in the list.

				//write the list order of the gate which gives output to this gate.
				if(_write(CEC_file, &creation_no, sizeof(unsigned int)) < sizeof(unsigned int))
				{
					_unlink(ScriptFilePath);
					GiveError("CEC file save error while writing the Gate Connection List: %s",0,ScriptFilePath);
				}
			}//eof CL
			
		}//eof GL

	}//eof for each CE.



	if(_commit(CEC_file) == -1)
	{//be sure the information is written in the CEC file else remove the damaged file.		
		GiveError("CEC file save error: %s",0,ScriptFilePath);
        _unlink(ScriptFilePath);
	}
	
	_close(CEC_file);
}

void CECircuit::LoadCircuit(char *file_path)
{	
	BB_BUF bb_buf; //breadboard buffer.
	CEH_BUF ceh_buf; //circuit element header buffer.
	GH_BUF gh_buf; //gate header buffer.
	char CEC_file_id[9];
	unsigned int count_gates;
	unsigned int count_connections;
	unsigned int LastSelectedCEListOrder;
	unsigned int list_order, j, i;
	Gate *connect_gate, *pick_GATE;

	SetCamCone = true; //tell the Artist that set the new Camcone.

	int CEC_file;

	GiveCompMsg("File is reading now (%s)...",file_path);

    CEC_file = _open(file_path, _O_RDONLY | _O_BINARY);

	if(CEC_file == -1)
	{
		perror("CEC file Error:");
        GiveError("CEC file open error occured");
	}

	//check CEC file identification
	_read(CEC_file, &CEC_file_id, 9);
	if(strncmp("CEC_FILE:", CEC_file_id, 9))
		GiveError("CE_FILE id confliction: This file is not a Circuit Engine Circuit file or it has been corrupted: %s",0,file_path);
	//eof checking CEC file identification

	GiveCompMsg("File ID is OK, the Circuit is Loading...");

	//Get Last Cam view
	_read(CEC_file, &NewCamCone, sizeof(CamCone));

	GiveCompMsg("Setting up the BreadBoard information...");

    //Get BreadBoard Specification and set BB.
	_read(CEC_file, &bb_buf, sizeof(bb_buf));
	theBreadBoard = new BreadBoard(CurrentBBrow = bb_buf.rows,	CurrentBBcol = bb_buf.cols, CurrentBBstride = bb_buf.nof_holes_in_a_node);
	//eof BB specification loading.

	GiveCompMsg("BreadBoard is ready.");

	//Get the Last LastSelectedCE list order
	_read(CEC_file, &LastSelectedCEListOrder, sizeof(unsigned int));

	//theBreadBoard->debugInsertVG(circuit->GetHead(), circuit->GetHead()->GetNext()); // DEBUG

	//Get Circuit
	//for each circuit element (until eof)
	for(;!_eof(CEC_file);)
	{
		//get Circuit Element Header (CEH)
		_read(CEC_file, &ceh_buf, sizeof(ceh_buf));

		NewCurrentCE();

		CurrentCE->SetType(ceh_buf.type);
		CurrentCE->CEInfo.SetByte(0, ceh_buf.CEInfo);
		CurrentCE->SetName(ceh_buf.name);
		CurrentCE->SetVoltageLegNo(ceh_buf.V_leg_no);
		CurrentCE->SetGroundLegNo(ceh_buf.G_leg_no);
		CurrentCE->MoveTo(ceh_buf.Position_X1,ceh_buf.Position_Y1);
		CurrentCE->SetNofInputs(ceh_buf.nof_inputs); //position X2 for CABLE
		CurrentCE->SetNofOutputs(ceh_buf.nof_outputs); //position Y2 for CABLE

		if(ceh_buf.type == cetCABLE) 
		{// (cable specific settings)
			CurrentCE->V.next_gate = (Gate *)(CurrentCE);
			CurrentCE->V.SetType(gtCable); 
			CurrentCE->V.leg_no = CurrentCE->GetID(); //cable node id setting.
			CurrentCE->V.Input[0] = NULL;
			CurrentCE->CEInfo.ResetIndex(ceqHasOd1);
			CurrentCE->CEInfo.ResetIndex(ceqHasOd2);
			CurrentCE->V.nof_inputs = *((unsigned short *)ceh_buf.name); // retrieve the cable height
			CurrentCE->SetName("CABLE"); // repare the name.
		}

		//Get Gate List (GL)
		for(count_gates=0; count_gates < ceh_buf.nof_gates; count_gates++)
		{
            //Get Gate Head (GH)
			_read(CEC_file, &gh_buf, sizeof(gh_buf));
			AddGate(gh_buf.type, gh_buf.type == gtInput ? 1 : gh_buf.nof_inputs, gh_buf.leg_no);
			//eof (GH)

			//Set up connection list of the Gate (CL) (KIMT: for gtInput, this 'for' is not processed)
			for(count_connections = 0 ; count_connections < gh_buf.nof_inputs ; count_connections++)
			{
				_read(CEC_file, &list_order, sizeof(int));
				CurrentGate->Input[count_connections] = (Gate *) list_order;
			}
			//eof (CL)

		}
		//eof (GL)

		//CONVERTING PROCESS (list-order to in-memory-gate-address)					
		for(pick_GATE = CurrentCE->GetHead() ; pick_GATE!=NULL ; pick_GATE = pick_GATE->GetNext())
		{
            if(pick_GATE->GetGateType() == gtInput) continue; //ignore input gate connection (it'll be done in CEngine)
			for(i=0; i<pick_GATE->GetNofInputs(); i++)
			{//connect corresponding input array element which it points to by its list_order inside.
                //proceed onto the specified gate by the list_order.
				for(j=0,connect_gate = CurrentCE->GetHead(); j<(unsigned int)pick_GATE->Input[i] ; ++j,connect_gate = connect_gate->GetNext());
				//now the connect_gate is on the gate which is to be connected to pick_GATE->Input[i].
				pick_GATE->Input[i] = connect_gate;
			}

		}
		//eof CONVERTING PROCESS.

		//LegPlugability binary array creation for the CE.
		switch(CurrentCE->GetType())
		{
		case cetCABLE:
		case cetLED:
			CurrentCE->LegPlugAbility.CreateBitArray(2);
			break;
		case cetCHIP:
			CurrentCE->LegPlugAbility.CreateBitArray( ceh_buf.nof_inputs + ceh_buf.nof_outputs + 2 + ((ceh_buf.nof_inputs+ceh_buf.nof_outputs)%2) );
			break;
		}	
		//eof LegPlugability

		AddCurrentCE();
	}//eof Circuit Element

	GiveCompMsg("In-CircuitElement connections completed, printing retrieved circuit (%s)",file_path);
	PrintCurrentCircuit();

	_close(CEC_file);

	//Set the last selected CE.
	CircuitElement *pick_CE = circuit->GetNext(); // pass source level.

	for(i=0; i<LastSelectedCEListOrder; i++, pick_CE = pick_CE->GetNext());
	LastSelectedCE = pick_CE;

	InsertTempVG();//TEST (upper left two bit)
	//(RE)PLUGGING PROCESS
    //Check All CEs if they were plugged, if so, make them plugged again for the session.
	GiveCompMsg("(Re)plugging process is initiated (%s)",file_path);
	
	pick_CE = circuit->GetNext(); // pass source level.
	bool OtherHover; //for maintenance of corresponding cable nodes.

	for(i = 0; i<nof_elements-1; i++, pick_CE = pick_CE->GetNext())
	{
		GiveCompMsg("Plugging %s",pick_CE->GetName());
		theBreadBoard->SetLegPlugAbility(pick_CE);
		

		switch(pick_CE->GetType())
		{
		case cetCABLE:			

			OtherHover = pick_CE->CEInfo[ceqHover2];
			pick_CE->CEInfo.SetIndex(ceqHover2); //stay uplugged while first leg is plugging.			

			pick_CE->CEInfo.ResetIndex(ceqHasOd1);
			pick_CE->CEInfo.ResetIndex(ceqHasOd2);			

			if(pick_CE->CEInfo[ceqHover1] == false) // if first leg is plugged
			{
				pick_CE->V.output = 0; //select leg 1
				pick_CE->CEInfo.SetIndex(ceqHover1);
				theBreadBoard->PlugCE(pick_CE);
			}
			if(OtherHover == false) // if second leg is plugged
			{				
				pick_CE->V.output = 1; //select leg 2				
				theBreadBoard->PlugCE(pick_CE);
			}

			break;

		case cetCHIP:			
		case cetLED:

			if(pick_CE->CEInfo[ceqHover] == false)//if it was plugged
			{//do real plug operation.
				theBreadBoard->PlugCE(pick_CE);
			}

			break;
		}
	}
	GiveCompMsg("(Re)plugging process is completed");
	//EOF (RE)PLUGGING PROCESS

	GiveCompMsg("Circuit is printing after (replugging process)");
	PrintCurrentCircuit();

}
void CECircuit::InsertTempVG(void)
{
	theBreadBoard->debugInsertVG(circuit->GetHead(),circuit->GetHead()->GetNext());
}

void CECircuit::SetLegPlugAbilityOfCurrentCE (void)
{
    theBreadBoard->SetLegPlugAbility(CurrentCE);
	return;
}