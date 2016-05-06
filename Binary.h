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
	Binary.h :	a primitive data str. for CEngine to handle individual bits
				within a char array as a bit array which
				Requires 8 times less memory space than standard 
				boolean arrays with a threshold of 8-bits.
*/

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <string.h> //for CE.

class Binary
{
	unsigned char *b_arr;
	unsigned int nof_bits;
	unsigned int nof_bytes;
	bool has_bit_array;

public:

	void Clear(void);
	~Binary();
	Binary();
	Binary(unsigned int size);
	void CreateBitArray(unsigned int size);
	int GiveNofOnes(void);
	int GiveIndexOfFirstZero(void);
	bool operator [](unsigned int index);
	void operator ++(void);
	bool SetIndex(unsigned int index);
	bool ResetIndex(unsigned int index);
	bool IsZero(void);
	void GiveMsg(char *msg, bool iserror = false);
	void ResetArray(void);
	unsigned char GetByte(unsigned int byte_index);
	void SetByte(unsigned int byte_index, unsigned char info);


	void debugprint(void);
};

 void Binary::GiveMsg(char *msg, bool iserror)
 {	 
	 if(iserror) printf("\n(e)");
	 else printf("\n(i)");

	 printf("(Binary) : %s",msg);
	 //if(iserror) exit(1);
 }

 void Binary::Clear(void)
 {
  for(unsigned int i=0 ; i < nof_bytes ; i++)
     b_arr[i] = 0x00;  
 }

 Binary::~Binary() 
 { 
	 free(b_arr); 
 }
 Binary::Binary()
 {
  nof_bits = 0;
  nof_bytes = 0;
  b_arr = NULL;
 }

 Binary::Binary(unsigned int size)
 {
  nof_bits = size;
  nof_bytes = size/8+1;
  b_arr = (unsigned char*) malloc (sizeof(unsigned char) * nof_bytes);

  Clear();  
 }

 void Binary::CreateBitArray(unsigned int size)
 {
  if(b_arr != NULL)
    free(b_arr);

  nof_bits = size;
  nof_bytes = (int)(size/8) + 1;
  b_arr = (unsigned char*) malloc (sizeof(unsigned char) * nof_bytes);

  Clear();  
 }

 int Binary::GiveNofOnes(void)
 {
  if(b_arr == NULL)
  {
   GiveMsg("GiveNofOnes : has no bit array.");
   return -1;
  }

  unsigned int nof_ones = 0;

  for(unsigned int i=0; i<nof_bytes; i++)
   for(unsigned int j=0; j<8; j++)
    if(((int)b_arr[i] & (int)pow((double)2,(int)j)) != 0) //(true)
      nof_ones++;

  return nof_ones;
 } 

 bool Binary::operator[](unsigned int index)
 {

  if(b_arr == NULL)
  {
	  GiveMsg("Binary::operator[] : has no bit array.");
	  return false;
  }

  if(index >= nof_bits) 
  {
	  //GiveMsg("Binary::operator[] : index exceeds the bit field size!",true);
	  return false; //the exceeding bits requested are always zero.
  }

  return ((int)b_arr[(int)(index/8)] & (int)pow((double)2,(int)index%8)) != 0 ? true : false;
 }

 void Binary::operator ++(void)
 {
  if(b_arr == NULL)
  {
   GiveMsg("operator++ : no bit array.");
   return;
  }
  for(unsigned int i=0; i<nof_bytes; i++)
  {
   ++b_arr[i];
   if(b_arr[i] != 0x00) break;
  }
  return;
 }

 bool Binary::SetIndex(unsigned int index)
 {
  if(b_arr == NULL)
  {
   GiveMsg("SetIndex : has no bit array.");
   return false;
  }
  if(index > nof_bits-1) return false;
  if((b_arr[index/8] & (int)pow((double)2,(int)index%8)) != 0) return false;
   b_arr[index/8] |= (int)pow((double)2,(int)index%8);
  return true;
 }

 bool Binary::ResetIndex(unsigned int index)
 {
  if(b_arr == NULL)
  {
   GiveMsg("ResetIndex : has no bit array.");
   return false;
  }
  if(index > nof_bits-1) return false;
  if((b_arr[index/8] & (int)pow((double)2,(int)index%8)) == 0) return false;
   b_arr[index/8] &= (0xFF - (int)pow((double)2,(int)index%8));
  return true;
 }
 bool Binary::IsZero(void)
 {
  if(b_arr == NULL)
  {
   GiveMsg("ResetIndex : has no bit array.");
   return false;
  }
  for(unsigned int i=0; i<nof_bytes; i++)
   if(b_arr[i])
    return false;
  return true;
 }

 void Binary::debugprint(void)
 {
    if(this->b_arr == NULL) 
	{
		printf("empty");
		return;
	}
    
	for(unsigned int i = 0 ; i<nof_bits ; i++)
        printf("%d",this->operator[](i));    
 }
 int Binary::GiveIndexOfFirstZero(void)
 {	
	 unsigned int index;
	 for(index = 0; index < nof_bits; index++)
		 if( ((int)b_arr[(int)(index/8)] & (int)pow((double)2,(int)index%8)) == 0) break;
	 return index == nof_bits ? (-1) : index;
 }
 void Binary::ResetArray(void)
 {
	 for(unsigned int i=0; i<nof_bytes; i++)
		 b_arr[i] = 0x00;
 } 
 /*void Binary::SetArray(void)
 {
	 for(unsigned int i=0; i<nof_bytes; i++)
		 b_arr[i] = 0xFF;
 }*/
 unsigned char Binary::GetByte(unsigned int byte_index)
 {
     if(b_arr == NULL) GiveMsg("GetByte() without creation of b_arr",true);
	 if(byte_index>=nof_bytes) GiveMsg("index bigger than the byte size. cannot GetByte()",true);

	 return b_arr[byte_index];
 }

 void Binary::SetByte(unsigned int byte_index, unsigned char info)
 {
     if(b_arr == NULL) GiveMsg("SetByte() without creation of b_arr",true);
	 if(byte_index>=nof_bytes) GiveMsg("index bigger than the byte size. cannot SetByte()",true);

	 b_arr[byte_index] = info;     
 }