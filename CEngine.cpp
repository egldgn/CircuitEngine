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
*/
char *ScriptFilePath;
#include "Artist.h"
#include <time.h>
#include <io.h>
#include <new.h>


FILE *logfile;

int handle_program_memory_depletion(size_t mem)
{
	printf("(!e!)CEngine: Not enough memory. CEngine halted");	
	exit(1);
}

int main(int argc, char **argv)
{
	int CheckTheFile;
	char FileIdBuf[9];
	char LogFileName[32];

	if (argv[1] == NULL)
	{
		perror("Please give as a parameter or scroll a .CEC or a .CESL file onto me...");

		try{ MessageBox(NULL, "Please give as a parameter or scroll a .CEC or a .CESL file onto me...", "Error", MB_OK); }
		catch (...){ return 1; }

		return 1;
	}
	ScriptFilePath = argv[1];

	//log file name
	strcpy(LogFileName, "CEngineLog_");
	_strdate(LogFileName + 11);
	LogFileName[19] = '_';
	_strtime(LogFileName + 20);
	LogFileName[13] = '.';
	LogFileName[16] = '.';
	LogFileName[22] = '.';
	LogFileName[25] = '.';
	strcat(LogFileName + 28, ".txt");

	_set_new_handler(handle_program_memory_depletion);

	printf("\n%s\n", LogFileName);

	if((logfile = fopen(LogFileName, "w")) == 0)
	{
		perror("log create error");
		exit(1);
	}
	
	if(-1 == _dup2( _fileno( logfile ), 1) )
	{
		perror( "Can't _dup2 stdout to log file" );
		exit( 1 );
	}	


	CEngineArtist = new CECircuit();


	//resolve the file whether it is CESL of CEC file.
	//:check if the file has CEC_FILE id, if so, treat as CEC file
	//else send it to the CECircuit parser
	CheckTheFile = _open(ScriptFilePath, _O_RDONLY);
	_read(CheckTheFile, &FileIdBuf, sizeof(char)*9);
	_close(CheckTheFile);

	if(strncmp("CEC_FILE:", FileIdBuf, 9)) // this is surely not the CEC file, so treat the file as a CESL file
	{
		//sending to the parser
        CEngineArtist->ParseScriptFile(ScriptFilePath);	
		CEngineArtist->InsertTempVG();		
	}
	else //it is a CEC file, so retrieve the circuit information from the file
	{		
        CEngineArtist->LoadCircuit(ScriptFilePath);
		SelectedCE = (CircuitElement *)LastSelectedCE;
	}
	
	InitArtist();	
	EnterMainLoop();	

	return 0;
}