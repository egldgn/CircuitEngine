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
	Artist.h : handles drawing duties as requested by user
				with the help of CEngine.h

	To improve the speed : 
	  - make use of display lists for the color alterations of chips, LEDs, etc.
*/

#pragma once
#include <windows.h>
#include "glut-3.7.6-bin\glut.h"

struct CamCone // cam movement path is a semisphere which encircles a conic.
{
    GLfloat theta_xy; //rotation factor for xy plane.-89.999962
	GLfloat theta_yz; //rotation factor for xz plane.
	GLfloat radius;
	GLfloat altitute;
	GLfloat pos_x,pos_y;
};
struct CamCone camcone = {0.001126f, 90.0f, 0.81f, -3.0f, -6.451996f, -4.253003f};//{0.0f, 127.0f, 13.839970f, 0.0f, 0.0f, 0.0f};//CAM CONE
struct CamCone NewCamCone; //when circuit is loaded from CEC file, this is used to set the cam back to the last position (by CECircuit::LoadCircuit())
bool SetCamCone = false;

char MessageBufInfo[150] = {"Circuit Engine (Beta Version) - Eastern Mediterranean University, Spring 2004"};
bool ShowInfo = true, IsInfoCleared;

void *LastSelectedCE = NULL;

#include "CEngine.h"


#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

//KEY MAP DEFINITIONS
#define KEY_CE_PLUG_UNPLUG		0x0D //Enter
#define KEY_CE_MOVE_LEFT		GLUT_KEY_LEFT //Left Arrow
#define KEY_CE_MOVE_RIGHT		GLUT_KEY_RIGHT // Right Arrow
#define KEY_CE_MOVE_UP			GLUT_KEY_UP // Up Arrow
#define KEY_CE_MOVE_DOWN		GLUT_KEY_DOWN // Down Arrow
#define KEY_FULLSCREEN			GLUT_KEY_F1
#define KEY_CE_SELECT_NEXT		0x5D //]
#define KEY_CE_SELECT_PREV		0x5B //[
#define KEY_PROGRAM_EXIT		0x1B //ESC
#define KEY_CE_TURN_AROUND_L	0X74 //t cam instance
#define KEY_CE_TURN_AROUND_U	0x54 //T
#define KEY_STANDARD_DISTANCE_L	0X63 //c cam instance
#define KEY_STANDARD_DISTANCE_U 0x43 //C
#define KEY_ADD_CABLE_L			0x6B //k add CABLE
#define KEY_ADD_CABLE_U			0x4B //K
#define KEY_ADD_LED_L			0x6C //l add LED
#define KEY_ADD_LED_U			0x4C //L
#define KEY_REMOVE_CE			0x7F //DEL Remove selected Cable or LED from the circuit.
#define KEY_CABLE_NODE_SELECT	0x09 //TAB select other node of selected cable.
#define KEY_CABLE_HEIGHT_UP_L	0x73 //w raise cable
#define KEY_CABLE_HEIGHT_UP_U	0x57 //W
#define KEY_CABLE_HEIGHT_DOWN_L	0x77 //s
#define KEY_CABLE_HEIGHT_DOWN_U	0x53 //S
#define KEY_ADD_SOURCE_v		0x76 //v
#define KEY_ADD_SOURCE_V		0x56 //V
#define KEY_ADD_SOURCE_g		0x67 //g
#define KEY_ADD_SOURCE_G		0x47 //G
#define KEY_SAVE_CIRCUIT		0x13 //CTRL + S (saves the circuit into corresponding .CEC file)

//Artist instant Commands
bool TurnAround = false;
bool StandardDistance = false;
bool StandardAngle = false;
bool LegWarning = false;
bool Fade = false;

Binary MouseButtons(3);
CECircuit *CEngineArtist;
CircuitElement *SelectedCE;
CircuitElement *CircuitHeadPtr;
//CircuitElement *SelectedCE; //moved to top for save operation.
char MessageBuf[200];
unsigned int CalReqTimInt = 100; // calculate request time interval is 100 ms
bool CalReqTimInt_Finished = true; // initiate the Calculate Request in Idle().

int prev_x, prev_y;

GLfloat  RateChip = 7.0f;
GLfloat  RateBB = 7.979978f;
GLfloat  ChipPositionY = -0.00f, ChipPositionX = 0.186f, ChipPositionZ = 0.134f;
GLfloat  LEDposX = 0.095f, LEDposY = 0.095f, LEDposZ = 0.035f;
//unsigned int  BBrow = 0.0f, CurrentBBcol = 0.0f, CurrentBBstride = 0.0f;

//Global chip vertices and dimensions for DrawChip() which is not a display list
	GLfloat Leg2LegWidth = 0.9f / RateChip;
	GLfloat LegWidth = 1.0f / RateChip;

	GLfloat Xbody = 5.0f/RateChip;
	GLfloat Zbody = 1.5f/RateChip;
	GLfloat Ybody;// changes according to chip, so it is in the DrawChip().

	GLfloat LegUpX = 0.6f / RateChip;
	GLfloat LegUpZ = LegWidth; //(leg width)
	GLfloat LegSUZ = 1.0f / RateChip; //LegSideUpZ
	GLfloat LegSUY = 2.0f / RateChip; //LegSideUpY
	GLfloat LegSDZ = 0.5f / RateChip; //LegSideDownZ
	GLfloat LegSDY = 3.0f / RateChip; //LegSideDownY
	GLfloat LegSC = 0.25f / RateChip; //LegSideCut

	GLfloat ChipBodyVertex [72] = {
	  0.0f,0.0f,Zbody, Xbody,0.0f,Zbody, Xbody,Ybody,Zbody, 0.0f,Ybody,Zbody, //top
	  0.0f,0.0f,0.0f, 0.0f,Ybody,0.0f, Xbody,Ybody,0.0f, Xbody,0.0f,0.0f,   //bottom
	  0.0f,Ybody,0.0f, 0.0f,0.0f,0.0f, 0.0f,0.0f,Zbody, 0.0f,Ybody,Zbody, //left
	  Xbody,0.0f,0.0f, Xbody,Ybody,0.0f, Xbody,Ybody,Zbody, Xbody,0.0f,Zbody, //right
	  0.0f,0.0f,0.0f, Xbody,0.0f,0.0f, Xbody,0.0f,Zbody, 0.0f,0.0f,Zbody, //rear
	  Xbody,Ybody,0.0f, 0.0f,Ybody,0.0f, 0.0f,Ybody,Zbody, Xbody,Ybody,Zbody //front
  };
	int ChipYBodyIndex[12] = {7,10,16,19,25,34,40,43,61,64,67,70}; //for fast referencing to modify Ybody in ChipBodyVertex[].

	GLfloat ChipBodyNormal [72] = {
	  0.0f,1.0f,0.0f, 0.0f,1.0f,0.0f, 0.0f,1.0f,0.0f, 0.0f,1.0f,0.0f,
	  0.0f,-1.0f,0.0f, 0.0f,-1.0f,0.0f, 0.0f,-1.0f,0.0f, 0.0f,-1.0f,0.0f,
	  -1.0f,0.0f,0.0f, -1.0f,0.0f,0.0f, -1.0f,0.0f,0.0f, -1.0f,0.0f,0.0f,
	  1.0f,0.0f,0.0f, 1.0f,0.0f,0.0f, 1.0f,0.0f,0.0f, 1.0f,0.0f,0.0f,
	  0.0f,0.0f,1.0f, 0.0f,0.0f,1.0f, 0.0f,0.0f,1.0f, 0.0f,0.0f,1.0f,
	  0.0f,0.0f,-1.0f, 0.0f,0.0f,-1.0f, 0.0f,0.0f,-1.0f, 0.0f,0.0f,-1.0f
  };

	GLfloat LegVertex [] = {
	      0.0f,    0.0f,            0.0f,
		  LegUpX, 0.0f,            0.0f,
		  LegUpX, 0.0f,            -LegUpZ,
		  0.0f,    0.0f,            -LegUpZ,

		  0.0f,    0.0f,            -LegUpZ,
		  0.0f,    -LegSUY,        -LegUpZ,
		  0.0f,    -LegSUY,        0.0f,
	      0.0f,    0.0f,            0.0f,

		  0.0f,    -LegSUY,        -LegSC-LegSDZ,
		  0.0f,    -LegSUY-LegSDY, -LegSC-LegSDZ,
		  0.0f,    -LegSUY-LegSDY, -LegSC,
		  0.0f,    -LegSUY,        -LegSC,
  };


	GLfloat LegNormal [] =  {
			0.0f,1.0f,0.0f,
		  0.0f,1.0f,0.0f,
		  0.0f,1.0f,0.0f,
		  0.0f,1.0f,0.0f,
		  -1.0f,0.0f,0.0f,
		  -1.0f,0.0f,0.0f,
		  -1.0f,0.0f,0.0f,
		  -1.0f,0.0f,0.0f,
		  -1.0f,0.0f,0.0f,
		  -1.0f,0.0f,0.0f,
		  -1.0f,0.0f,0.0f,
		  -1.0f,0.0f,0.0f
  };
//eof chip vertices

GLfloat A,B,C,OS,OT,OT2,OT3; //BB one hole percentages (A,B,C) and lengths (OS,OT,OT2,OT3)

//Cable vertice coordinates : OT3 is initialized actually in OGLinit(), and 100.0f's are height value initialized in every drawing of a cable in CableDraw().
GLfloat CableVertice[] = {0.0f,0.0f,0.0f, OT3,0.0f,0.0f, OT3,OT3,0.0f, 0.0f,OT3,0.0f, 0.0f,0.0f,100.0f , OT3,0.0f,100.0f, OT3,OT3,100.0f, 0.0f,OT3,100.0f};
GLint CablePassVertice[] = {3,2,1,0, 5,6,7,4, 0,1,5,4, 2,3,7,6, 6,5,1,2, 3,0,4,7}; //bottom, top, front,rear, right,left
GLfloat CableNormal[] = {0.0f,0.0f,-1.0f, 0.0f,0.0f,1.0f, 0.0f,-1.0f,0.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,0.0f, -1.0f,0.0f,0.0f}; //bottom,top front,rear, right, left

GLfloat const CEPositionXYFactor = 0.271427f; //1-unit step of CE movement.
GLfloat const CEPositionZFactor = 1.0f;

// Global environment variables for light materials.
GLfloat light_position[] = {15.0f, 15.0f, 15.0f, 1.0f};
GLfloat light_position1[] = {-1.0f, -1.0f, -1.0f, 0.0f};
GLfloat light1_color[]= {0.90f, 0.90f, 1.0f, 1.0f};
GLfloat light1_ambient[]= {0.0f, 0.0f, 0.05f, 0.05f};
GLfloat no_mat[] = {0.01f, 0.01f, 0.01f, 1.0f};
GLfloat mat_ambient[] = {0.5f, 0.5f, 0.5f, 1.0f};
GLfloat mat_diffuse[] = {0.1f, 0.1f, 0.1f, 1.0f};
GLfloat mat_specular[]= {0.3f, 0.3f, 0.3f, 0.3f};
GLfloat mat_shininess[] = {5.0f};

GLuint DisplayListHead = 0;
bool FullScreen = false;
GLfloat HoleMarkerColor[] = {1.0f,0.4f,0.4f, 0.8f, 0.4f,1.0f,0.4f, 0.8f};

GLint ViewPortHeight;

void InitArtist(unsigned int _BBrow, unsigned int _BBcol, unsigned int _BBstride);
void GiveMsg(char *);
void DrawCircuit(void); //Main drawing function.
void initOGL(void);
void EnterMainLoop(void);
void GiveUserMsg(bool top = false);
void ClearInfo(int a);
void FlyCam(void); //flies the cam the to the NewCamCone position
//Artist's drawings.
void DrawChip(unsigned int nof_legs);
void DrawBB(int row, int col, int stride);
void DrawLED(void);
void DrawCEMarker(void); //the marker for the selected CE.
void DrawHoleMarker(void);
void DrawCable(CircuitElement *Cable);
//Callback functions for GLUT.
void Keyboard(unsigned char key, int x, int y);
void SpecialKeys(int key, int x, int y);
void ReshapeWindow(int x, int y);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void Idle(void);
void CalculateRequestTimer(int CalcInterval);

void InitArtist()
{
	CircuitHeadPtr = CEngineArtist->circuit;
	if(LastSelectedCE == NULL)  // if the last selected one is not loaded from a CEC file...
		SelectedCE = CircuitHeadPtr->GetNext();	// ...load it as the default one which is the head.
}

void GiveMsg(char *msg)
{
	printf("\n(i)Artist : %s.",msg);
}

void DrawChip(unsigned int nof_legs) //rate => 1 GLunit = RATE mm.
{
	//calculate Ybody according to the nof_legs and place them in the Vertex array of chip.
	Ybody = (Leg2LegWidth +(Leg2LegWidth+LegWidth)*nof_legs)/2; //legth of the chip. // (betweenLegsWidth(0.9mm)) + (1legWidth(1mm) + betweenLegsWidth(0.9mm)) * nof_legs = CHIPLENGTH.
	for(int i=0; i<12; i++)
		ChipBodyVertex[ChipYBodyIndex[i]] = Ybody;

	glPushMatrix();
	glTranslatef(ChipPositionX,ChipPositionY,ChipPositionZ);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glVertexPointer( 3, GL_FLOAT, 0, ChipBodyVertex);
	glNormalPointer( GL_FLOAT, 0, ChipBodyNormal);

	mat_ambient[0] = 0.1f,mat_ambient[1] = 0.1f,mat_ambient[2] = 0.1f;
	mat_diffuse[0] = 0.1f,mat_diffuse[1] = 0.1f,mat_diffuse[2] = 0.1f;
	mat_specular[0] = 0.5f,mat_specular[1] = 0.5f,mat_specular[2] = 0.5f;

	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);

	//glMaterialfv( GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
	glEnable(GL_CULL_FACE);
		glDrawArrays(GL_QUADS, 0, 24); //draw chip body.
	glDisable(GL_CULL_FACE);

	glVertexPointer( 3, GL_FLOAT, 0, LegVertex);
	glNormalPointer( GL_FLOAT, 0, LegNormal);

	mat_ambient[0] = 0.3f,mat_ambient[1] = 0.3f,mat_ambient[2] = 0.3f;
	mat_diffuse[0] = 0.1f,mat_diffuse[1] = 0.1f,mat_diffuse[2] = 0.1f;
	mat_specular[0] = 5.0f,mat_specular[1] = 5.0f,mat_specular[2] = 5.0f;

	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 15.0f);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	
	unsigned int i;
	for(i=0; i<nof_legs/2; i++) //left legs.
	{
		glPushMatrix();
		glTranslatef(-LegUpX, (Ybody - LegWidth - Leg2LegWidth)-(i*(Leg2LegWidth+LegWidth))  ,Zbody/2);
		glRotatef(90.0f,1.0f,0.0f,0.0f);
		glDrawArrays(GL_QUADS, 0, 12);//draw one left leg.
		glPopMatrix();
	}

	for(i=0; i<nof_legs/2; i++) //right legs.
	{
		glPushMatrix();
		glTranslatef(LegUpX + Xbody , (Ybody - Leg2LegWidth)-(i*(Leg2LegWidth+LegWidth))  ,Zbody/2);
		glRotatef(180.0f,0.0f,0.0f,1.0f);
		glRotatef(90.0f,1.0f,0.0f,0.0f);
		glDrawArrays(GL_QUADS, 0, 12);//draw one right leg.
		glPopMatrix();
	}
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glPopMatrix();
}


void DrawBB(int row, int col, int stride)
{
	glEnable(GL_CULL_FACE); //default culling face is already GL_BACK.

	 GLfloat depth = 0.6f / RateBB;

	 GLfloat Hole [] = { //Bread Board one Hole. OS = Outside OT = Outthick.
		0.0f,0.0f,0.0f,	//1 (0)
		OS,0.0f,0.0f,	//2 (3)
		OS,OT,0.0f,	//3 (6)
		0.0f,OT,0.0f,	//4 (9)
		OS,OS-OT,0.0f,	//5 (12)
		OS-OT,OS-OT,0.0f,	//6 (15)
		OS-OT,OT,0.0f,	//7 (18)
		OS,OS,0.0f,	//8 (21)
		0.0f,OS,0.0f,	//9 (24)
		0.0f,OS-OT,0.0f,	//10 (27)
		OT,OT,0.0f,	//11 (30)
		OT,OS-OT,0.0f,	//12 (33)
		OT+OT2+OT3, OT+OT2, -depth,	//13 (35)
		OT+OT2, OT+OT2, -depth,	//14 (38)
		OT+OT2+OT3, OT+OT2+OT3, -depth,	//15 (41)
		OT+OT2, OT+OT2+OT3, -depth		//16 (44)
	};

	 GLint HolePassVertices[36] = {0,1,2,3, 6,2,4,5, 4,7,8,9, 3,10,11,9, 10,6,12,13, 12,6,5,14, 15,14,5,11, 10,13,15,11, 13,12,14,15};

	 int Col,S,R; //col, stride, and row counters.
	 GLfloat BetweenCols = 4.332f/RateBB;//

	 mat_ambient[0] = 0.1f,mat_ambient[1] = 0.1f,mat_ambient[2] = 0.1f;
	 mat_diffuse[0] = 0.7f,mat_diffuse[1] = 0.7f,mat_diffuse[2] = 0.7f;
	 mat_specular[0] = 0.0f,mat_specular[1] = 0.0f,mat_specular[2] = 0.0f;

	 glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	 glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	 glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	 glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10.0f);

	 GLfloat xHollow = BetweenCols, yHollow = OS*row, zHollow = 0.2;
	 GLfloat CoverDepth = 0.5f, alpha = col*stride*OS + (col-1)*BetweenCols , beta = OS*row; //cover dimensions

	 //draw breadboard-cover first
	 glBegin(GL_QUADS);
	 glNormal3f(0.0f, -1.0f, 0.0f);// south side
	 glVertex3f(0.0f, 0.0f, -zHollow);
	 glVertex3f(0.0f, 0.0f, -zHollow-CoverDepth);
	 glVertex3f(alpha, 0.0f, -zHollow-CoverDepth);
	 glVertex3f(alpha, 0.0f, -zHollow);
	 glNormal3f(-1.0f, 0.0f, 0.0f);// west side
	 glVertex3f(0.0f, 0.0f, -zHollow-CoverDepth);
	 glVertex3f(0.0f, 0.0f, 0.0f);
	 glVertex3f(0.0f, beta, 0.0f);
	 glVertex3f(0.0f, beta, -zHollow-CoverDepth);
	 glNormal3f(1.0f, 0.0f, 0.0f);// east side
	 glVertex3f(alpha, 0.0f, 0.0f);
	 glVertex3f(alpha, 0.0f, -zHollow-CoverDepth);
	 glVertex3f(alpha, beta, -zHollow-CoverDepth );
	 glVertex3f(alpha, beta, 0.0f);
	 glNormal3f(0.0f, 1.0f, 0.0f);// north side
	 glVertex3f(0.0f, beta, -zHollow-CoverDepth);
	 glVertex3f(0.0f, beta, -zHollow);
	 glVertex3f(alpha ,beta, -zHollow);
	 glVertex3f(alpha ,beta, -zHollow-CoverDepth);
	 glEnd();

	 for(Col = 0; Col<col; Col++)
	 {//Draw BB Col
		glPushMatrix();
		glTranslatef(Col*(BetweenCols+OS*stride),0.0f,0.0f); // move ((one row length) + (BetweenCols))*the column no
		if(Col) //draw hollow
		{
			glBegin(GL_QUADS); //3 quads for hollow
			glNormal3f(-1.0f, 0.0f, 0.0f);//Surface 1
            glVertex3f(0.0f, 0.0f, -zHollow);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(0.0f, yHollow, 0.0f);
            glVertex3f(0.0f, yHollow, -zHollow);
			glNormal3f(0.0f, 0.0f, 1.0f);//Surface 2
            glVertex3f(-xHollow, 0.0f, -zHollow);
            glVertex3f(0.0f, 0.0f, -zHollow); //1
            glVertex3f(0.0f, yHollow, -zHollow);//4
            glVertex3f(-xHollow, yHollow, -zHollow);
			glNormal3f(1.0f, 0.0f, 0.0f);//Surface 3
            glVertex3f(-xHollow, 0.0f, -zHollow);//5
            glVertex3f(-xHollow, yHollow, -zHollow);//6
            glVertex3f(-xHollow, yHollow, 0.0f);
            glVertex3f(-xHollow, 0.0f, 0.0f);
			glNormal3f(0.0f, -1.0f, 0.0f);//this quads is to cover the sides of individual columns (north and south sides).
            glVertex3f(-xHollow, 0.0f, -zHollow);//5
            glVertex3f(-xHollow, 0.0f, 0.0f); //8
            glVertex3f(-xHollow - (OS*stride), 0.0f, 0.0f);
            glVertex3f(-xHollow - (OS*stride), 0.0f, -zHollow);
			glNormal3f(0.0f, 1.0f, 0.0f); // north side
            glVertex3f(-xHollow, yHollow, -zHollow);
            glVertex3f(-xHollow - (OS*stride), yHollow, -zHollow);
            glVertex3f(-xHollow - (OS*stride), yHollow, 0.0f);
            glVertex3f(-xHollow, yHollow, 0.0f);
			glEnd();
		}
		if(Col==(col-1))
		{
			glPushMatrix();
			glTranslatef(BetweenCols+(OS*stride),0.0f,0.0f);
			glBegin(GL_QUADS);
			glNormal3f(0.0f, -1.0f, 0.0f);//additional north/south quads for last col
            glVertex3f(-xHollow, 0.0f, -zHollow);//5
            glVertex3f(-xHollow, 0.0f, 0.0f); //8
            glVertex3f(-xHollow - (OS*stride), 0.0f, 0.0f);
            glVertex3f(-xHollow - (OS*stride), 0.0f, -zHollow);
			glNormal3f(0.0f, 1.0f, 0.0f); // north side
            glVertex3f(-xHollow, yHollow, -zHollow);
            glVertex3f(-xHollow - (OS*stride), yHollow, -zHollow);
            glVertex3f(-xHollow - (OS*stride), yHollow, 0.0f);
            glVertex3f(-xHollow, yHollow, 0.0f);
			glEnd();
			glPopMatrix();
		}
		for(S=0; S<stride; S++)
		{//Draw one col in BB col
			glPushMatrix();
			glTranslatef(S*OS,0.0f,0.0f);// col is completed on x
			for(R=0; R<row; R++)
			{//Draw one hole
				glPushMatrix();
				glTranslatef(0.0f,R*OS,0.0f); //row lies on y axis.

				glBegin(GL_QUADS);
				//top. z (normal vector) (4 quads)
					glNormal3f(0.0f,0.0f,1.0f);
				
				int i;
				for(i=0; i<16; i++)
				{
					glVertex3fv(Hole+HolePassVertices[i]*3);
				}
				//from top the bottom side. y,z (normal vector)  (1 quad)
					glNormal3f(0.0f,1.0f,1.0f);
				for(i=16; i<20; i++)
				{
					glVertex3fv(Hole+HolePassVertices[i]*3);
				}
				//from top the right side. -x,z (normal vector)  (1 quad)
					glNormal3f(-1.0f,0.0f,1.0f);
				for(i=20; i<24; i++)
				{
					glVertex3fv(Hole+HolePassVertices[i]*3);
				}
				//from top the upper side. -y,z (normal vector)  (1 quad)
					glNormal3f(0.0f,-1.0f,1.0f);
				for(i=24; i<28; i++)
				{
					glVertex3fv(Hole+HolePassVertices[i]*3);
				}
				//from top the left side. x,z (normal vector)  (1 quad)
					glNormal3f(1.0f,0.0f,1.0f);
				for(i=28; i<32; i++)
				{
					glVertex3fv(Hole+HolePassVertices[i]*3);
				}
				glEnd();

				glDisable(GL_LIGHTING);
				glColor3f(0.0f,0.0f,0.0f);
				//bottom
				glBegin(GL_QUADS);
					glNormal3f(0.0f,0.0f,1.0f);
				for(;i<36;i++)
				{
					glVertex3fv(Hole+HolePassVertices[i]*3);
				}
				glEnd();
				glEnable(GL_LIGHTING);
				glPopMatrix();
			}
			glPopMatrix();
		}
		glPopMatrix();
	}
	glDisable(GL_CULL_FACE);
}
void DrawLED(void)
{
	glPushMatrix();

	glTranslatef(LEDposX, LEDposY, LEDposZ);

	GLfloat xLED = OT3,yLED = 2*OT3+2*OT2+OT*2 ,zLED = 0.5f;
	GLfloat LED_vertice[] = { 0.0f,0.0f,0.0f, 0.0f,yLED,0.0f, xLED,yLED,0.0f, xLED,0.0f,0.0f, 0.0f,0.0f,zLED, xLED,0.0f,zLED, xLED,yLED,zLED, 0.0f,yLED,zLED };
	//3*(0-7) are red LED face vertices.

	//Set LED leg color same as the chip leg color.
	mat_ambient[0] = 0.3f,mat_ambient[1] = 0.3f,mat_ambient[2] = 0.3f;
	mat_diffuse[0] = 0.1f,mat_diffuse[1] = 0.1f,mat_diffuse[2] = 0.1f;
	mat_specular[0] = 1.0f,mat_specular[1] = 1.0f,mat_specular[2] = 1.0f;
	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 15.0);

	GLfloat LEDlegX = xLED-0.1f,LEDlegZ = zLED; //LED legs are 2D as chip legs.

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glPushMatrix();
			glTranslatef(0.0f, (GLfloat)(yLED/2) + (GLfloat)(OT+OT2+OT3/2) ,-0.3f);
		glBegin(GL_QUADS);
		    glNormal3f(0.0,1.0,0.0);
			glVertex3fv( LED_vertice+0*3 ); //front leg
			glVertex3fv( LED_vertice+3*3 );
			glVertex3fv( LED_vertice+5*3 );
			glVertex3fv( LED_vertice+4*3 );
		glEnd();
	glPopMatrix();
	glPushMatrix();
			glTranslatef(0.0f, (GLfloat)(yLED/2) - (GLfloat)(OT+OT2+OT3/2) ,-0.3f);
		glBegin(GL_QUADS);
			glNormal3f(0.0,-1.0,0.0);
	        glVertex3fv( LED_vertice+0*3 ); //rear leg
			glVertex3fv( LED_vertice+4*3 );
			glVertex3fv( LED_vertice+5*3 );
			glVertex3fv( LED_vertice+3*3 );
		glEnd();
	glPopMatrix();
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_QUADS);

		glNormal3f(0.0f,0.0f,-1.0f);
		glVertex3fv( LED_vertice+0*3 ); //bottom
		glVertex3fv( LED_vertice+1*3 );
		glVertex3fv( LED_vertice+2*3 );
		glVertex3fv( LED_vertice+3*3 );

		glNormal3f(0.0f,0.0f,1.0f);
		glVertex3fv( LED_vertice+4*3 ); //top
		glVertex3fv( LED_vertice+5*3 );
		glVertex3fv( LED_vertice+6*3 );
		glVertex3fv( LED_vertice+7*3 );

		glNormal3f(0.0f,-1.0f,0.0f);
		glVertex3fv( LED_vertice+0*3 ); //front
		glVertex3fv( LED_vertice+3*3 );
		glVertex3fv( LED_vertice+5*3 );
		glVertex3fv( LED_vertice+4*3 );


		glNormal3f(1.0f,0.0f,0.0f);
		glVertex3fv( LED_vertice+3*3 ); //right
		glVertex3fv( LED_vertice+2*3 );
		glVertex3fv( LED_vertice+6*3 );
		glVertex3fv( LED_vertice+5*3 );

		glNormal3f(0.0f,1.0f,0.0f);
		glVertex3fv( LED_vertice+2*3 ); //rear
		glVertex3fv( LED_vertice+1*3 );
		glVertex3fv( LED_vertice+7*3 );
		glVertex3fv( LED_vertice+6*3 );

		glNormal3f(-1.0f,0.0f,0.0f);
		glVertex3fv( LED_vertice+1*3 ); //left
		glVertex3fv( LED_vertice+0*3 );
		glVertex3fv( LED_vertice+4*3 );
		glVertex3fv( LED_vertice+7*3 );
	glEnd();

	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}



void DrawCEMarker(void)
{
	GLfloat MX= .5,MY = .5,MZ = .1;

				glDisable(GL_LIGHTING);
				glEnable(GL_BLEND);
				glTranslatef(0.0,0.0,0.4);

				glBegin(GL_TRIANGLES);

                 glVertex3f(0.0,0.0,0.0);
				 glVertex3f(-MX,0.0,MZ);
				 glVertex3f(0.0,-MY,MZ);

				 glVertex3f(0.0,0.0,0.0);
				 glVertex3f(-MX,0.0,0.0);
				 glVertex3f(-MX,0.0,MZ);

                 glVertex3f(0.0,0.0,0.0);
				 glVertex3f(0.0,-MY,MZ);
				 glVertex3f(0.0,-MY,0.0);

				 //this is one quad (by two triangles)
                 glVertex3f(0.0,-MY,MZ);
                 glVertex3f(-MX,0.0,MZ);
                 glVertex3f(-MX,0.0,0.0);

                 glVertex3f(0.0,-MY,MZ);
                 glVertex3f(-MX,0.0,0.0);
                 glVertex3f(0.0,-MY,0.0);

				glEnd();

				glEnable(GL_LIGHTING);
				glDisable(GL_BLEND);
}
void DrawHoleMarker(void)
{
	GLfloat depth = -0.5f / RateBB;
	GLfloat top = 0.05f;

	glPushMatrix();
	glTranslatef(OT+OT2,OT+OT2,0.0);
	GLfloat HollowMarker[] = {	0.0f,0.0f,depth, 0.0f,OT3,depth, OT3,OT3,depth, OT3,0.0f,depth, //bottom
								0.0f,0.0f,top, OT3,0.0f,top, OT3,OT3,top, 0.0f,OT3,top,   //top
	};
	GLint PassVertice [] = {0,1,2,3, 4,5,6,7, 0,4,7,1, 2,6,5,3, 3,5,4,0, 1,7,6,2};

	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);

	glBegin(GL_QUADS);
	 for(int i=0; i<24; i++)
		 glVertex3fv(HollowMarker + PassVertice[i]*3);
	glEnd();
	glPopMatrix();

	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
}
GLfloat CableTopCoverLength;
GLfloat CableTopCoverTheta = 0.0f;
GLfloat CableTopPullUp1,CableTopPullUp2;

void DrawCable(CircuitElement *pCable)
{
	if(pCable == SelectedCE) //if currently drawing cable is the selected one, its color is light green.
	{
		mat_diffuse[0] = 0.9f,mat_diffuse[1] = 9.0f,mat_diffuse[2] = 0.1f;
	}
	else // dark green.
	{
		mat_diffuse[0] = 0.1f,mat_diffuse[1] = 1.0f,mat_diffuse[2] = 0.1f;
	}

	mat_ambient[0] = 0.1f,mat_ambient[1] = 0.1f,mat_ambient[2] = 0.1f;
	mat_specular[0] = 0.4f,mat_specular[1] = 0.4f,mat_specular[2] = 0.4f;

	glMaterialfv( GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv( GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, 1.0f);

	glEnable(GL_CULL_FACE);
	//set the height of the cable (for vertices 14 17 20 23)(they were marked as 100.0f units initially)
	for(int i=14; i<24; i+=3)
        CableVertice[i] = ((GLfloat)pCable->V.nof_inputs / 10.0);

	glPushMatrix();//First leg bar (vertical)
	 glTranslatef(OT+OT2+OT3/4+(CEPositionXYFactor * pCable->GetPositionX()),OT+OT2+OT3/4+(CurrentBBrow - 1 - pCable->GetPositionY())*CEPositionXYFactor, pCable->CEInfo[ceqHover1] * CEPositionZFactor);
	 glBegin(GL_QUADS);//CableVertices 0123, 0154, 1265, 2376, 3047, 5674 (bottom,sides,top)
	 for(int i=0; i<24; i++)
		 for(int i=0; i<24; i++)
		 {
			 if(!(i%4)) glNormal3fv( CableNormal + (i/4)*3 );
			 glVertex3fv( CableVertice + 3*CablePassVertice[i]);
		 }
	 glEnd();
	glPopMatrix();

	glPushMatrix();//Second leg bar (vertical)
	 glTranslatef(OT+OT2+OT3/4+(CEPositionXYFactor * pCable->GetPositionX2()),OT+OT2+OT3/4+(CurrentBBrow - 1 - pCable->GetPositionY2())*CEPositionXYFactor, pCable->CEInfo[ceqHover2] * CEPositionZFactor);
	 glBegin(GL_QUADS);//second leg
		for(int i=0; i<24; i++)
		{
			if(!(i%4)) glNormal3fv( CableNormal + (i/4)*3 );
			glVertex3fv( CableVertice + 3*CablePassVertice[i]);
		}
	 glEnd();
	glPopMatrix();


	if(pCable->GetPositionX() == pCable->GetPositionX2() && pCable->GetPositionY() == pCable->GetPositionY2())
		return;
	glPushMatrix();//connection line of first and second leg bars (horizontal)
	 //cable top cover parameters:
	 CableTopCoverLength = (double)sqrt( (double)(((CurrentBBrow - 1 - pCable->GetPositionY2())-(CurrentBBrow - 1 - pCable->GetPositionY()))*((CurrentBBrow - 1 - pCable->GetPositionY2())-(CurrentBBrow - 1 - pCable->GetPositionY())) + (pCable->GetPositionX2()-pCable->GetPositionX())*(pCable->GetPositionX2()-pCable->GetPositionX())) )*(OT3+2*OT2+2*OT);//sqrt( (Y2-Y1)(Y2-Y1) + (X2-X1)(X2-X1) );


	 if(pCable->GetPositionY() == pCable->GetPositionY2())
	 {//when asin(~1), double values are sometimes exceeds exact 1, so explicitly specify the case:
         if( pCable->GetPositionX() < pCable->GetPositionX2() )
			 CableTopCoverTheta = 90.0f;
		 else
			 CableTopCoverTheta = 270.0f;
	 }
	 else
	 {
		 CableTopCoverTheta = (GLfloat) (180/M_PI)*asin( (long double) ( (abs((int)(pCable->GetPositionX2() - pCable->GetPositionX()))*(long double)(OT3+2*OT2+2*OT)) / (long double)CableTopCoverLength) );

		 if(pCable->GetPositionX() < pCable->GetPositionX2() )
		 {
			 //region 1 is +0
			 if(pCable->GetPositionY()<pCable->GetPositionY2())
			 {
				 CableTopCoverTheta = 180.0f - CableTopCoverTheta;//region 2
			 }
		 }
		 else//x1>x2
		 {
			 if(pCable->GetPositionY()<pCable->GetPositionY2())
			 {
				 CableTopCoverTheta = CableTopCoverTheta + 180.0f;//region 3
			 }
			 else
			 {
				 CableTopCoverTheta = -CableTopCoverTheta;//region 4
			 }
		 }
	 }

	 //calculation of CableTopPullUp for leg 1 and for leg 2 sides;
	 CableTopPullUp1 = CableTopPullUp2 = (CEPositionZFactor*(pCable->V.nof_inputs/10.0f));
	 if(pCable->CEInfo[ceqHover1]) CableTopPullUp1 += CEPositionZFactor;
	 if(pCable->CEInfo[ceqHover2]) CableTopPullUp2 += CEPositionZFactor;
	 //eof CableTopPullUpx calculation.

	 glTranslatef(0.0f,OT3/4,0.0f);
	 glTranslatef(OT+OT2+OT3/4+(CEPositionXYFactor * pCable->GetPositionX()),OT+OT2+OT3/4+(CurrentBBrow - 1 - pCable->GetPositionY())*CEPositionXYFactor, 0.0f);
	 glTranslatef(OT3/4,0.0f,0.0f);
	 glRotatef(CableTopCoverTheta,0.0f,0.0f,-1.0f);
	 glTranslatef(-OT3/4,0.0f,0.0f);

	 glBegin(GL_QUADS);
	//front
	  glVertex3f(0.0f,0.0f,0.0f + CableTopPullUp1);					//all vertices at leg 1 side
	  glVertex3f(OT3/2, 0.0f, 0.0f + CableTopPullUp1);
	  glVertex3f(OT3/2, 0.0f, OT3/2 + CableTopPullUp1);
	  glVertex3f(0.0f, 0.0f, OT3/2 + CableTopPullUp1);
	//rear
	  glVertex3f(0.0f,CableTopCoverLength,0.0f + CableTopPullUp2);	//all vertices at leg 2 side
	  glVertex3f(0.0f, CableTopCoverLength, OT3/2 + CableTopPullUp2);
	  glVertex3f(OT3/2, CableTopCoverLength, OT3/2 + CableTopPullUp2);
	  glVertex3f(OT3/2, CableTopCoverLength, 0.0f + CableTopPullUp2);
	//left
	  glVertex3f(0.0f,0.0f,0.0f + CableTopPullUp1);					//leg 1 side
	  glVertex3f(0.0f,0.0f,OT3/2 + CableTopPullUp1);					//leg 1 side
	  glVertex3f(0.0f,CableTopCoverLength,OT3/2 + CableTopPullUp2);	//leg 2 side
	  glVertex3f(0.0f,CableTopCoverLength,0.0f + CableTopPullUp2);	//leg 2 side
	//right
	  glVertex3f(OT3/2,0.0f,0.0f + CableTopPullUp1);					//leg 1 side
	  glVertex3f(OT3/2,CableTopCoverLength,0.0f + CableTopPullUp2);	//leg 2 side
	  glVertex3f(OT3/2,CableTopCoverLength,OT3/2 + CableTopPullUp2);	//leg 2 side
	  glVertex3f(OT3/2,0.0f,OT3/2 + CableTopPullUp1);					//leg 1 side
	//bottom
	  glVertex3f(0.0f,0.0f,0.0f + CableTopPullUp1);					//leg 1 side
	  glVertex3f(0.0f,CableTopCoverLength,0.0f + CableTopPullUp2);	//leg 2 side
	  glVertex3f(OT3/2,CableTopCoverLength,0.0f + CableTopPullUp2);	//leg 2 side
	  glVertex3f(OT3/2,0.0f,0.0f + CableTopPullUp1);					//leg 1 side
	//top
	  glVertex3f(0.0f,0.0f,OT3/2 + CableTopPullUp1);					//leg 1 side
	  glVertex3f(OT3/2,0.0f,OT3/2 + CableTopPullUp1);					//leg 1 side
	  glVertex3f(OT3/2,CableTopCoverLength,OT3/2 + CableTopPullUp2);	//leg 2 side
	  glVertex3f(0.0f,CableTopCoverLength,OT3/2 + CableTopPullUp2);	//leg 2 side
	 glEnd();
	glPopMatrix();
	glDisable(GL_CULL_FACE);
}
/*
void DrawSourceBar(void)
{
	glPushMatrix();
	glTranslatef(OT+OT2, OT+OT2, 0.0f);
	glBegin(GL_QUADS);//Bar from CableVertices 0123, 0154, 1265, 2376, 3047, 5674 (bottom,sides,top)
		for(int i=0; i<20; i++)
			glVertex3fv( CableVertice + 3*CablePassVertice[i]);
	glEnd();
	glPopMatrix();
}
*/

void DrawCircuit(void)
{//Circuit traverse and redraw...

	glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glPushMatrix();

		gluLookAt(0.0f, camcone.radius*(GLfloat)cos(camcone.theta_yz*(M_PI/180.0f)), camcone.radius*(GLfloat)sin(camcone.theta_yz*(M_PI/180.0f)) , 0.0f, 0.0f, 0.0f , 0.0f,0.0f,1.0f);
		glRotatef( camcone.theta_xy, 0.0f, 0.0f, 1.0f);
		glTranslatef( camcone.pos_x , camcone.pos_y , -0.1f );

		static CircuitElement *pick_CE = CircuitHeadPtr->GetNext();
		static unsigned int hole_count;
		
		glCallList(DisplayListHead);	// draw BB.

		pick_CE = CircuitHeadPtr->GetNext();
		if(pick_CE!=NULL)// if there is at least one CE, pass into drawing phase.
		do
		{
			glPushMatrix();
			switch(pick_CE->GetType())
			{
			case cetCHIP:
				if(pick_CE == SelectedCE && pick_CE->CEInfo[ceqHover])//draw hole markers
					for(hole_count = 0; hole_count < pick_CE->GetNofLegs()/2; hole_count++)
					{
						glPushMatrix();
						glTranslatef( pick_CE->GetPositionX()*CEPositionXYFactor, (CurrentBBrow - pick_CE->GetPositionY() - hole_count -1)*CEPositionXYFactor,0.0);
						glColor4fv( HoleMarkerColor + (int)pick_CE->LegPlugAbility[hole_count]*4 );
						glCallList(DisplayListHead+3); //draw left hole marker.
						glTranslatef( CEPositionXYFactor*3, 0.0f , 0.0f);
						glColor4fv( HoleMarkerColor + (int)pick_CE->LegPlugAbility[pick_CE->GetNofLegs()-1 -hole_count]*4 );
						glCallList(DisplayListHead+3);  //draw corresponding right hole marker.
						glPopMatrix();
					}
				glTranslatef( pick_CE->GetPositionX() * CEPositionXYFactor , (CurrentBBrow - pick_CE->GetNofLegs()/2 - pick_CE->GetPositionY()) * CEPositionXYFactor, CEPositionZFactor * pick_CE->CEInfo[ceqHover]);
				DrawChip( pick_CE->GetNofLegs() );
				break;
			case cetLED:
				if(pick_CE == SelectedCE && pick_CE->CEInfo[ceqHover])
				{//draw hole markers for LED
                    glPushMatrix();
					glTranslatef( pick_CE->GetPositionX()*CEPositionXYFactor, (CurrentBBrow - 1 - pick_CE->GetPositionY())*CEPositionXYFactor,0.0);
					glColor4fv( HoleMarkerColor + (int)pick_CE->LegPlugAbility[0]*4 );
					glCallList(DisplayListHead+3);// draw for up leg.
					glTranslatef( 0.0f, -CEPositionXYFactor, 0.0f);
					glColor4fv( HoleMarkerColor + (int)pick_CE->LegPlugAbility[1]*4 );
					glCallList(DisplayListHead+3);//draw for down leg.
					glPopMatrix();
				}
				glTranslatef( pick_CE->GetPositionX() * CEPositionXYFactor, (CurrentBBrow - 2 - pick_CE->GetPositionY()) * CEPositionXYFactor, CEPositionZFactor * pick_CE->CEInfo[ceqHover]);
				pick_CE->CEInfo[ceqOn] ? glColor4f(1.0f,0.5f,0.5f, 0.9f) : glColor4f(5.0f,0.0f,0.0f, 0.8f);
				glCallList(DisplayListHead+1); //Call LED drawing
				break;
			case cetCABLE:
				if(pick_CE == SelectedCE)
				{//draw hole markers for CABLE
					if(SelectedCE->CEInfo[ceqHover1])
					{
                    glPushMatrix();
					glTranslatef( pick_CE->GetPositionX()*CEPositionXYFactor, (CurrentBBrow - 1 - pick_CE->GetPositionY())*CEPositionXYFactor,0.0);
					glColor4fv( HoleMarkerColor + (int)pick_CE->LegPlugAbility[0]*4 );
					glCallList(DisplayListHead+3);// draw for up leg.
					glPopMatrix();
					}
					if(SelectedCE->CEInfo[ceqHover2])
					{
					glPushMatrix();
					glTranslatef( pick_CE->GetPositionX2()*CEPositionXYFactor, (CurrentBBrow - 1 - pick_CE->GetPositionY2())*CEPositionXYFactor,0.0);
					glColor4fv( HoleMarkerColor + (int)pick_CE->LegPlugAbility[1]*4 );
					glCallList(DisplayListHead+3);//draw for down leg.
					glPopMatrix();
					}
				}
				DrawCable(pick_CE);
				break;
			}
			if(pick_CE == SelectedCE)
			{
				if(pick_CE->GetType() == cetCABLE)
				{//CEMarkers for cable.

					//draw the CEMarker at the selected node of the cable.

					if(!pick_CE->V.output) // if the first leg is the selected one
					{ //draw the marker, over the first leg.
						glPushMatrix();
						pick_CE->CEInfo[ceqHover1] ? glColor4f(0.0,0.8,0.0,0.8) : glColor4f(0.8,0.3,0.8,0.8);
						glTranslatef( pick_CE->GetPositionX()*CEPositionXYFactor, (CurrentBBrow - 1 - pick_CE->GetPositionY())*CEPositionXYFactor, pick_CE->CEInfo[ceqHover1] * CEPositionZFactor + (GLfloat)pick_CE->V.nof_inputs / 10.0f);						
						glCallList(DisplayListHead+2);
						glPopMatrix();
					}
					else
					{ //draw the marker, over the second leg.
						glPushMatrix();
						pick_CE->CEInfo[ceqHover2] ? glColor4f(0.0,0.8,0.0,0.8) : glColor4f(0.8,0.3,0.8,0.8);
						glTranslatef( pick_CE->GetPositionX2()*CEPositionXYFactor, (CurrentBBrow - 1 - pick_CE->GetPositionY2())*CEPositionXYFactor, (pick_CE->CEInfo[ceqHover2] * CEPositionZFactor + (GLfloat)pick_CE->V.nof_inputs / 10.0f) - 0.2);						
						glCallList(DisplayListHead+2);
						glPopMatrix();
					}					
				}
				else
				{
					pick_CE->CEInfo[ceqHover] ? glColor4f(0.0,0.8,0.0,0.8) : glColor4f(0.8,0.3,0.8,0.8);
					glCallList(DisplayListHead+2);//Call CEMarker drawing
				}
			}
			glPopMatrix();

			pick_CE = pick_CE->GetNext();
		}while( pick_CE != CircuitHeadPtr->GetNext() );

	glPopMatrix();

	//prepare lower left message buffer to inform about the SelectedCE.
	if(CEngineArtist->nof_elements == 1)
	{
		strcpy(MessageBuf, "No Circuit Element was Loaded");
	}
	else
	{
		strcpy(MessageBuf,SelectedCE->name);
		strcat(MessageBuf,".");
		static char buf[500];
		sprintf(buf,"%d",SelectedCE->GetID());
		strcat(MessageBuf,buf);

		//sprintf(buf,"[%d,%d]X[%d,%d] Adrr(%p),CNID(%d),Od1(%c),Od2(%c),Hov1(%c),Hov2(%c),SEL(%s)",SelectedCE->GetPositionY(),(SelectedCE->GetPositionX()/(CurrentBBstride+2)),SelectedCE->GetPositionY2(),(SelectedCE->GetPositionX2()/(CurrentBBstride + 2)),SelectedCE, SelectedCE->V.leg_no, SelectedCE->CEInfo[ceqHasOd1]?'Y':'N', SelectedCE->CEInfo[ceqHasOd2]?'Y':'N', SelectedCE->CEInfo[ceqHover1]?'Y':'N', SelectedCE->CEInfo[ceqHover2]?'Y':'N',SelectedCE->V.output?"Second":"First"); // DEBUG		
		//strcat(MessageBuf,buf);	// DEBUG
	}
	GiveUserMsg(); // update the message on screen for information about the SelectedCE.

	if(ShowInfo)	
		GiveUserMsg(true);

	glutSwapBuffers();
}
void initOGL(void)
{
	glClearColor(.0f,.19f,.27f,0.0f);
	//glClearColor(0.35f,0.53f,0.58f,0.0f);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);

	glLightfv( GL_LIGHT1, GL_DIFFUSE , light1_color);
	glLightfv( GL_LIGHT1, GL_AMBIENT, light1_color);
	glLightfv( GL_LIGHT1, GL_POSITION, light_position1);

	glLightfv( GL_LIGHT0, GL_AMBIENT, light1_ambient);
	glLightfv( GL_LIGHT0 , GL_POSITION, light_position);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FLAT);
	//glLineWidth(1.5);
	glLightModeli( GL_LIGHT_MODEL_LOCAL_VIEWER , GL_FALSE);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	//Bread Board Hole Dimensions and percentages
	A = 0.10f;//0.1508;
	B = 0.6059f; //0.4459;
	C = 1.0f - A-B; //0.3231
	OS = 2.166f / RateBB;//OS and OTs are same terms as in DrawBB.
	OT = (OS*A)/2.0f, OT2 = OS*B/2.0f, OT3 = (OS*C);

	//initialize cable OT3(width) vertices://3 6 7 10 15 18 19 22
	CableVertice[6] = CableVertice[7] = CableVertice[10] = 	CableVertice[15] = 	CableVertice[18] = 	CableVertice[19] = 	CableVertice[22] = CableVertice[3] = OT3/2;

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	//generate display lists for BreadBoard and LED drawings:
	DisplayListHead = glGenLists(4); //create display list for BreadBoard,LED and CEMarker and HoleMarker repectively
	glNewList(DisplayListHead, GL_COMPILE);
	  DrawBB(CurrentBBrow,CurrentBBcol,CurrentBBstride);
    glEndList();
	glNewList(DisplayListHead+1, GL_COMPILE);
	  DrawLED();
    glEndList();
	glNewList(DisplayListHead+2, GL_COMPILE);
	  DrawCEMarker();
    glEndList();
	glNewList(DisplayListHead+3, GL_COMPILE);
	  DrawHoleMarker();
    glEndList();

	if(SelectedCE!=NULL) CEngineArtist->theBreadBoard->SetLegPlugAbility(SelectedCE);
}
void EnterMainLoop(void)
{
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowPosition( 500, 50 );
	glutInitWindowSize( 500, 500 );
	glutCreateWindow("Circuit Engine - Test");
	initOGL();

	glutDisplayFunc(DrawCircuit);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeys);
	glutReshapeFunc(ReshapeWindow);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutIdleFunc(Idle);
	//SpecialKeys( KEY_FULLSCREEN,0,0); //get into fullscreen mode on load.
	
	if( !SetCamCone)
		Keyboard( KEY_STANDARD_DISTANCE_L,0,0);//initiate default raise cam at startup. it looks nice;1

	glutMainLoop(); // Artist gets control...
}
void Keyboard(unsigned char key, int x, int y)
{
	/*sprintf(MessageBufInfo,"Pressed Key : %X",key);
	ShowInfo = true;
	GiveUserMsg(true);*/

	CircuitElement *tmp = SelectedCE?SelectedCE->GetNext():NULL;//for KEY_REMOVE_CE

	switch(key)
	{
	case 'q':
		CEngineArtist->theBreadBoard->debugListCableNodes(SelectedCE);
		break;

	case KEY_CE_PLUG_UNPLUG:

		if(SelectedCE == NULL) break;

		if(SelectedCE->GetType() == cetCABLE)//<<<<<<
		{
			if(!SelectedCE->LegPlugAbility[SelectedCE->V.output])
			{
				LegWarning = true;//ArtistCommands.SetIndex(ACLegWarning);
				return;
			}
			if(SelectedCE->CEInfo[ceqHover1 + SelectedCE->V.output]) //if selected leg is in hover mode, plug it
			{
                SelectedCE->CEInfo.ResetIndex(ceqHover1 + SelectedCE->V.output);
				CEngineArtist->theBreadBoard->PlugCE(SelectedCE);
			}
			else // if selected leg is not in hover mode, unplug it
			{
				SelectedCE->CEInfo.SetIndex(ceqHover1 + SelectedCE->V.output);
				CEngineArtist->theBreadBoard->UnPlugCE(SelectedCE);
			}
			CEngineArtist->theBreadBoard->SetLegPlugAbility(SelectedCE);
			return;
		}

		if(SelectedCE->CEInfo[ceqHover] && SelectedCE->LegPlugAbility.GiveIndexOfFirstZero() != -1)	{LegWarning=true;return;}

		if(SelectedCE->CEInfo[ceqHover])
		{
            SelectedCE->CEInfo.ResetIndex(ceqHover);
            CEngineArtist->theBreadBoard->PlugCE(SelectedCE);
		}
		else
		{
			SelectedCE->CEInfo.SetIndex(ceqHover);			
			CEngineArtist->theBreadBoard->UnPlugCE(SelectedCE);
			CEngineArtist->theBreadBoard->SetLegPlugAbility(SelectedCE);
		}

		break;

	case KEY_CE_SELECT_NEXT:

		if(SelectedCE == NULL) break;
        SelectedCE = SelectedCE->GetNext();
		if(	SelectedCE->CEInfo[ceqHover] ||
			(SelectedCE->GetType() == cetCABLE && (SelectedCE->CEInfo[ceqHover1] || SelectedCE->CEInfo[ceqHover2]))
			)
			CEngineArtist->theBreadBoard->SetLegPlugAbility(SelectedCE);

		break;

	case KEY_CE_SELECT_PREV:

		if(SelectedCE == NULL) break;
		SelectedCE = SelectedCE->GetPrev();
		if(	SelectedCE->CEInfo[ceqHover] ||
			(SelectedCE->GetType() == cetCABLE && (SelectedCE->CEInfo[ceqHover1] || SelectedCE->CEInfo[ceqHover2]))
			)
			CEngineArtist->theBreadBoard->SetLegPlugAbility(SelectedCE);

		break;

	case KEY_CE_TURN_AROUND_U:
	case KEY_CE_TURN_AROUND_L:

		if(IsInfoCleared)
		{
			strcpy(MessageBufInfo,"'Turn Around' cam mode - Press a mouse button to stop.");
			ShowInfo = true;
		}

        TurnAround = true;
		break;

	case KEY_PROGRAM_EXIT:
		
		CEngineArtist->~CECircuit();
		fcloseall();
		exit(0);

	case KEY_STANDARD_DISTANCE_U:
	case KEY_STANDARD_DISTANCE_L:

		StandardDistance = true;
		StandardAngle = true;
		break;

	case KEY_ADD_CABLE_U:
	case KEY_ADD_CABLE_L:

		AddCable();
		SelectedCE = CEngineArtist->circuit->GetNext()->GetPrev();
		break;

	case KEY_ADD_LED_U:
	case KEY_ADD_LED_L:

		AddLED();
		SelectedCE = CEngineArtist->circuit->GetNext()->GetPrev();
		break;

	case KEY_REMOVE_CE:

		if(SelectedCE->GetType() != cetCHIP)
		{
		if(CEngineArtist->nof_elements == 1) break;
		CEngineArtist->RemoveCE(SelectedCE);
		SelectedCE = (CEngineArtist->nof_elements == 1) ? NULL : tmp;
		}
		break;

	case KEY_CABLE_NODE_SELECT:

		if(SelectedCE == NULL) break;
		if(SelectedCE->GetType() == cetCABLE)
			SelectedCE->V.output = !SelectedCE->V.output;
		break;

	case KEY_CABLE_HEIGHT_UP_U:
	case KEY_CABLE_HEIGHT_UP_L:

		if(SelectedCE == NULL) break;
		if(SelectedCE->GetType() == cetCABLE && SelectedCE->V.nof_inputs > 1)
			--SelectedCE->V.nof_inputs;
		break;

	case KEY_CABLE_HEIGHT_DOWN_U:
	case KEY_CABLE_HEIGHT_DOWN_L:

		if(SelectedCE == NULL) break;
		if(SelectedCE->GetType() == cetCABLE && SelectedCE->V.nof_inputs < 100)
			++SelectedCE->V.nof_inputs;
		break;

	case KEY_ADD_SOURCE_v:
	case KEY_ADD_SOURCE_V:
		break;

	case KEY_ADD_SOURCE_g:
	case KEY_ADD_SOURCE_G:
		break;

	case 'z':
		CEngineArtist->debugPrintCable();
		break;

	case KEY_SAVE_CIRCUIT:

		sprintf(MessageBufInfo,"Circuit is being saved now...");
		ShowInfo = true;
		GiveUserMsg(true);

		LastSelectedCE = (void *)SelectedCE; // commit the last SelectedCE into CECircuit.h
		CEngineArtist->SaveCircuit();
		
		sprintf(MessageBufInfo,"Circuit is saved.");
		ShowInfo = true;
		GiveUserMsg(true);

	}
}
void SpecialKeys(int key, int x, int y)
{
	switch(key)
	{
	case KEY_CE_MOVE_LEFT:
		if(SelectedCE == NULL) break;
		if(SelectedCE->CEInfo[ceqHover] || (SelectedCE->GetType()==cetCABLE?SelectedCE->CEInfo[ceqHover1 + SelectedCE->V.output]:false))
		{
            SelectedCE->MoveLeft();
			CEngineArtist->theBreadBoard->SetLegPlugAbility(SelectedCE);

		}
		break;
	case KEY_CE_MOVE_RIGHT:
		if(SelectedCE == NULL) break;
		if(SelectedCE->CEInfo[ceqHover]  || (SelectedCE->GetType()==cetCABLE?SelectedCE->CEInfo[ceqHover1 + SelectedCE->V.output]:false))//&& SelectedCE->GetPositionX() < ChipConstraint.MAX_X_POS )
		{
			SelectedCE->MoveRight();
			CEngineArtist->theBreadBoard->SetLegPlugAbility(SelectedCE);
//			glutPostRedisplay();
		}
		break;
	case KEY_CE_MOVE_UP:
		if(SelectedCE == NULL) break;
		if(SelectedCE->CEInfo[ceqHover] || (SelectedCE->GetType()==cetCABLE?SelectedCE->CEInfo[ceqHover1 + SelectedCE->V.output]:false))// && SelectedCE->GetPositionY() > 0)// MIN_Y_POS
		{
			SelectedCE->MoveUp();
			CEngineArtist->theBreadBoard->SetLegPlugAbility(SelectedCE);
//			glutPostRedisplay();
		}
		break;
	case KEY_CE_MOVE_DOWN:
		if(SelectedCE == NULL) break;
		if(SelectedCE->CEInfo[ceqHover]  || (SelectedCE->GetType()==cetCABLE?SelectedCE->CEInfo[ceqHover1 + SelectedCE->V.output]:false))//&& SelectedCE->GetPositionY() < CurrentBBrow-(SelectedCE->GetNofLegs()/2)) //CurrentBBrow-(SelectedCE->GetNofLegs()) is MAX_Y_POS for a chip
		{
			SelectedCE->MoveDown();
			CEngineArtist->theBreadBoard->SetLegPlugAbility(SelectedCE);
//			glutPostRedisplay();
		}
		break;
	case KEY_FULLSCREEN:
		FullScreen = !FullScreen;
        if(FullScreen)
			glutFullScreen();
		else
		{
			glutPositionWindow(100,100);
			glutReshapeWindow(500, 500);
		}
		break;
	}
}
void ReshapeWindow(int w, int h)
{
	ViewPortHeight = h;
	glViewport(0,0,w,h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, w, 0.0, h);
	glColor3f(0.1,0.7,0.0);
	glDisable(GL_LIGHTING);
	glRasterPos2f(0.0,0.0);
	glEnable(GL_LIGHTING);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.0f, (GLfloat)w/h, 0.1f,500.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//printf("Resize : w = %d h = %d",w,h);
}

void Mouse(int button, int state, int x, int y)
{
	if(state == GLUT_DOWN)
		MouseButtons.SetIndex(button);
	else
		MouseButtons.ResetIndex(button);

	//ArtistCommands.ResetIndex(ACTurnAround);
	TurnAround = false;
	SetCamCone = false;

	if(MouseButtons[GLUT_MIDDLE_BUTTON])
	{
		sprintf(MessageBufInfo,"camcone = {%f,%f,%f,%f,%f,%f}",camcone.theta_xy,camcone.theta_yz,camcone.radius,camcone.altitute,camcone.pos_x,camcone.pos_y);
		ShowInfo = true;
	}

	prev_x = x;
	prev_y = y;
}
void Motion(int x, int y)
{
	static int addition_1,addition_2;
	if(MouseButtons[GLUT_LEFT_BUTTON] && MouseButtons[GLUT_RIGHT_BUTTON])
	{
        addition_1 = y - prev_y;
		camcone.radius += (GLfloat)addition_1*0.01f;
		if(camcone.radius <= 0.1f) camcone.radius = 0.1f;
	prev_y = y;
//	glutPostRedisplay();
	}
	else if(MouseButtons[GLUT_LEFT_BUTTON])//rotate about the cam_cone origin.
	{
		addition_1 = x - prev_x;
		addition_2 = y - prev_y;

		camcone.theta_xy += (GLfloat)addition_1*0.1f;
		if(camcone.theta_xy >= 360.0f ) camcone.theta_xy -= 360.0f;
		else if(camcone.theta_xy <= -360.0f) camcone.theta_xy += 360.0f;

		camcone.theta_yz += addition_2*0.1f;
		if(camcone.theta_yz < 90.0f) camcone.theta_yz = 90.0f;
	    else if(camcone.theta_yz > 180.0f) camcone.theta_yz = 180.0f;

	prev_x = x;
	prev_y = y;
//	glutPostRedisplay();
	}
	else if(MouseButtons[GLUT_RIGHT_BUTTON])
	{//move the cam_cone
		addition_1 = x - prev_x;
		addition_2 = y - prev_y;
		camcone.pos_x += (GLfloat)addition_1*0.004f;
		camcone.pos_y -= (GLfloat)addition_2*0.004f;
	prev_x = x;
	prev_y = y;

//	glutPostRedisplay();
	}
}
void Idle(void)
{
	if(TurnAround)//ArtistCommands[ACTurnAround])
	{
        camcone.theta_xy += 0.3f;
		if(camcone.theta_xy >= 360.0f) camcone.theta_xy -= 360.0f;
		//glutPostRedisplay();
	}

	if(StandardAngle)//ArtistCommands[ACStandardAngle])//goto standard angle = 45.0
	{
		if(camcone.theta_yz > 135.0f) {camcone.theta_yz -= 1.0f;if(camcone.theta_yz <= 135.0f) StandardAngle = false;}//ArtistCommands.ResetIndex(ACStandardAngle);}
		else if(camcone.theta_yz < 135.0f) {camcone.theta_yz += 1.0f;if(camcone.theta_yz >= 135.0f) StandardAngle = false;}//ArtistCommands.ResetIndex(ACStandardAngle);}
		else if(camcone.theta_yz == 135.0f) StandardAngle = false;//ArtistCommands.ResetIndex(ACStandardAngle);
		//glutPostRedisplay();
	}

	if(StandardDistance)//ArtistCommands[ACStandardDistance])	//goto standard distance = 15.0 GL units.
	{
		if(camcone.radius < 15.0f) {camcone.radius += 0.2;if(camcone.radius >= 15.0f) StandardDistance = false;}//ArtistCommands.ResetIndex(ACStandardDistance);}
		else if(camcone.radius > 15.0f) {camcone.radius -= 0.2;if(camcone.radius <= 15.0f) StandardDistance = false;}//ArtistCommands.ResetIndex(ACStandardDistance);}
		else if(camcone.radius == 15.0f) StandardDistance = false;//ArtistCommands.ResetIndex(ACStandardDistance);
		//glutPostRedisplay();
	}

	if(LegWarning)
	{
		if(Fade)
		{
			HoleMarkerColor[1] += 0.1f;
            HoleMarkerColor[2] += 0.1f;
			if(HoleMarkerColor[1] >= 1.0f) Fade = false;
		}
		else
		{
            HoleMarkerColor[1] -= 0.1f;
            HoleMarkerColor[2] -= 0.1f;
			if(HoleMarkerColor[1] <= 0.4f) {LegWarning = false; Fade = true;}
		}
		//glutPostRedisplay();
	}

	if(ShowInfo)
	{
		IsInfoCleared = false;
        glutTimerFunc(9000, ClearInfo, 0);
	}

	if( SetCamCone ) //here the SetCamCone is set by CECircuit::LoadCircuit().
		FlyCam();

	if(CalReqTimInt_Finished == true)
	{
		CalReqTimInt_Finished = false;
		glutTimerFunc(CalReqTimInt, CalculateRequestTimer, 0); // calculate circuit
	}

	glutPostRedisplay();
}

void CalculateRequestTimer(int CalcInterval)
{	
	CalculateTraverseCircuit();	
	CalReqTimInt_Finished = true;
}
void CalculateTraverseCircuit(void)
{
	static CircuitElement *CE;
	static int count;
	CE = CEngineArtist->circuit->GetNext();
	for(count = 0; count< (int)CEngineArtist->nof_elements-1; count++,CE=CE->GetNext())
	{
		CE->CalculateCE();
	}
}
void AddCable(void)
{
    CEngineArtist->NewCurrentCE();

	CEngineArtist->CurrentCE->SetType(cetCABLE);
	CEngineArtist->CurrentCE->SetName("CABLE");
	CEngineArtist->CurrentCE->V.leg_no = CEngineArtist->CurrentCE->GetID();//set cable node ID(cable specific).

	CEngineArtist->CurrentCE->CEInfo.SetIndex(ceqHover1);
	CEngineArtist->CurrentCE->CEInfo.SetIndex(ceqHover2);
	CEngineArtist->CurrentCE->CEInfo.ResetIndex(ceqHover);//not used for cable, just for distinction when it moves.
	CEngineArtist->CurrentCE->LegPlugAbility.CreateBitArray(2);

	CEngineArtist->CurrentCE->V.SetType(gtCable);
	CEngineArtist->CurrentCE->V.nof_inputs = 3; //height (a CABLE specific event)
    CEngineArtist->CurrentCE->V.next_gate = (Gate *)CEngineArtist->CurrentCE; //next_gate of V points to CE (a CABLE special event)

	CEngineArtist->CurrentCE->MoveTo(SelectedCE?SelectedCE->GetPositionX():0,SelectedCE?SelectedCE->GetPositionY():0);
	CEngineArtist->CurrentCE->MoveTo2(SelectedCE?SelectedCE->GetPositionX():0,SelectedCE?SelectedCE->GetPositionY():0);

	CEngineArtist->AddCurrentCE();
}
void AddLED(void)
{
    CEngineArtist->NewCurrentCE();
	CEngineArtist->CurrentCE->SetType(cetLED);
	CEngineArtist->CurrentCE->SetName("LED");
	CEngineArtist->CurrentCE->LegPlugAbility.CreateBitArray(2);
	CEngineArtist->CurrentCE->SetVoltageLegNo(1);
	CEngineArtist->CurrentCE->SetGroundLegNo(2);
	if(SelectedCE)
	{//born next to the selected, and prevent to born outside if the selected position is at the bottom edge of the BB.
		CEngineArtist->CurrentCE->MoveTo(SelectedCE->GetPositionX(), SelectedCE->GetPositionY() == CurrentBBrow - 1 ? SelectedCE->GetPositionY() - 1 : SelectedCE->GetPositionY());
	}

	CEngineArtist->SetLegPlugAbilityOfCurrentCE(); //set the leg plug ability of new born.
	CEngineArtist->AddCurrentCE();	
}
void AddSourceBar(bool SourceBit)
{
	CEngineArtist->NewCurrentCE();

	CEngineArtist->CurrentCE->SetType(cetSOURCE);
	CEngineArtist->CurrentCE->SetName("SOURCE");
	CEngineArtist->CurrentCE->V.output = SourceBit;//(source bar specific)
	CEngineArtist->CurrentCE->LegPlugAbility.CreateBitArray(1);//since it is a 1 leg CE.
	CEngineArtist->CurrentCE->V.SetType(gtSource);
	CEngineArtist->CurrentCE->V.Input[0] = SourceBit ? CEngineArtist->circuit->GetHead() : CEngineArtist->circuit->GetHead()->GetNext() ;
	//cetSOURCE's V.Input[0] points to the SourceBit'th index of the source level of the circuit in CECircuit
	//which is the corresponding source gate Voltage for SourceBit = true and Ground for SourceBit = false;

	//born next to the selected CE.
	CEngineArtist->CurrentCE->MoveTo(SelectedCE->GetPositionX(),SelectedCE->GetPositionY());

	CEngineArtist->AddCurrentCE();
}

void GiveUserMsg(bool top)
{
	GLfloat V[4];
	GLfloat decrement = 0.0f,temp;

	glBitmap(0,0, 0,0,0,top?(ViewPortHeight-(top?12:30)):0, NULL);
	for(int i=0; i<(int)strlen(top?MessageBufInfo:MessageBuf); i++)
	{
		glGetFloatv(GL_CURRENT_RASTER_POSITION,V);
		temp = V[0]; //before printing character.
		glutBitmapCharacter(top?GLUT_BITMAP_HELVETICA_12:GLUT_BITMAP_HELVETICA_18, (int)(top?MessageBufInfo:MessageBuf)[i]);
		glGetFloatv(GL_CURRENT_RASTER_POSITION,V);
		decrement += V[0] - temp; //after - before
	}
	glBitmap(0,0, 0,0, -decrement,top?(-ViewPortHeight+(top?12:30)):0, NULL);

}

void ClearInfo(int a)
{
	ShowInfo = false;
	IsInfoCleared = true;
}

bool CamConeVarPlaced[5]; //implies that the corresponding newcamcone variable setting is completed.

void FlyCam(void) //flies the cam the to the NewCamCone position
{
	if(!CamConeVarPlaced[0])
	if(camcone.pos_x >= NewCamCone.pos_x) {camcone.pos_x -= 0.05f; if(camcone.pos_x <= NewCamCone.pos_x) {CamConeVarPlaced[0] = true;}}
	else if(camcone.pos_x < NewCamCone.pos_x) {camcone.pos_x += 0.05f; if(camcone.pos_x >= NewCamCone.pos_x) {CamConeVarPlaced[0] = true;}}

	if(!CamConeVarPlaced[1])
	if(camcone.pos_y >= NewCamCone.pos_y) {camcone.pos_y -= 0.05f; if(camcone.pos_y <= NewCamCone.pos_y) {CamConeVarPlaced[1] = true;}}
	else if(camcone.pos_y < NewCamCone.pos_y) {camcone.pos_y += 0.05f; if(camcone.pos_y >= NewCamCone.pos_y) {CamConeVarPlaced[1] = true;}}

	if(!CamConeVarPlaced[2])
	if(camcone.radius >= NewCamCone.radius) {camcone.radius -= 0.06f; if(camcone.radius <= NewCamCone.radius) {CamConeVarPlaced[2] = true;}}
	else if(camcone.radius < NewCamCone.radius) {camcone.radius += 0.06f; if(camcone.radius >= NewCamCone.radius) {CamConeVarPlaced[2] = true;}}

	if(!CamConeVarPlaced[3])
	if(camcone.theta_xy >= NewCamCone.theta_xy) {camcone.theta_xy -= 0.4f; if(camcone.theta_xy <= NewCamCone.theta_xy) {CamConeVarPlaced[3] = true;}}
	else if(camcone.theta_xy < NewCamCone.theta_xy) {camcone.theta_xy += 0.4f; if(camcone.theta_xy >= NewCamCone.theta_xy) {CamConeVarPlaced[3] = true;}}

	if(!CamConeVarPlaced[4])
	if(camcone.theta_yz >= NewCamCone.theta_yz) {camcone.theta_yz -= 0.4f; if(camcone.theta_yz <= NewCamCone.theta_yz) {CamConeVarPlaced[4] = true;}}
	else if(camcone.theta_yz < NewCamCone.theta_yz) {camcone.theta_yz += 0.4f; if(camcone.theta_yz >= NewCamCone.theta_yz) {CamConeVarPlaced[4] = true;}}

	//there is still a NewCamCone variable which is not set yet.
	for(int i=0; i<5; i++)
		if(!CamConeVarPlaced[i])		
			return;

	SetCamCone = false;
}