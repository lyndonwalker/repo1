// Raw2Ascii.cpp : Converts an input file representing sound in 8-bit format to 
// ASCII code for inclusion in software for playback. Also compresses the file using
// Run Length Encoding to reduce the memory needed to store the data.
//
// Copyright Lyndon Walker 2014
// 
//

#include "stdafx.h"
#include <iostream>
#include <fstream>

using namespace std;
int _tmain(int argc, _TCHAR* argv[])
{
	_TCHAR *file= argv[1];
	if (!file)
	{
		cout << "No filename given";
	}
	else
	{
		try
		{
			// Get size
			ifstream fileData( file, ios::binary | ios::ate);
			int size= (int)fileData.tellg();
			fileData.close();

			// Get data
			ifstream fileIn;
			fileIn.open(file, fstream::in|fstream::binary);
			
			// Read it in
			char *raw= new char[size];
			fileIn.read(raw, size);
			fileIn.close();

			// spit data out
			ofstream fileOut;
			fileOut.open("raw.out", fstream::out);
			unsigned char max=0;
			unsigned char min= 255;
			// Write header
			fileOut << "// Generated by Raw2Ascii.exe" << endl;
			fileOut << "// Raw audio: " << size << " bytes" << endl;
			fileOut << "static char audio[] PROGMEM = {" << endl;
			int fileWidth= 6;
			for (int i=0; i < size; i++)
			{
				// Write value
				fileOut << (int)raw[i];
				// Gather statistics
				if (max < raw[i])
				{
					max = raw[i];
				}
				if (min > raw[i])
				{
					min = raw[i];
				}

				if (i < size - 1)
				{
					fileOut << ",";
				}
				fileWidth += 4;
				if (fileWidth > 80)
				{
					fileWidth =0;
					fileOut << endl;
				}
			}
			fileOut << "};" << endl;
			fileOut << "static const int length = " << size << ";" << endl;
			fileOut << "static const char maxVal = " << (int)max << ";" << endl;
			fileOut << "static const char minVal = " << (int)min << ";" << endl;
			fileOut.close();

			// Generate RLE output
			ofstream rleOut;
			rleOut.open("rle.out", fstream::out);
			int pos=0;
			char current=0;
			int count=0;
			// Write header
			rleOut << "audioRle[";
			fileWidth= 9;
			int rleSize=0;
			while(pos < size)
			{
				current= raw[pos];
				while(raw[pos] == current)
				{
					count++;
					pos++;
				}
				rleOut << (int)current << "," << count;
				rleSize++;
				if (pos < size -1)
				{
					rleOut << ",";
				}
				fileWidth += 4;
				if (fileWidth > 80)
				{
					fileWidth =0;
					rleOut << endl;
				}
				count = 0;
			}
			rleOut << "];" << endl;
			rleOut << "// RLE audio: " << rleSize << " bytes" << endl;
			rleOut.close();

			delete[] raw;
			raw=0;
		}
		catch (exception e)
		{
		}
	}

	return 0;
}

