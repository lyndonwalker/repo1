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

			for (int i=0; i < size; i++)
			{
				fileOut << (int)raw[i];
				if (i < size - 1)
				{
					fileOut << ",";
				}
				fileOut << endl;
			}
			fileOut.close();

			// Generate RLE output
			ofstream rleOut;
			rleOut.open("rle.out", fstream::out);
			int pos=0;
			char current=0;
			int count=0;
			while(pos < size)
			{
				current= raw[pos];
				while(raw[pos] == current)
				{
					count++;
					pos++;
				}
				rleOut << (int)current << "," << count;
				if (pos < size -1)
				{
					rleOut << ",";
				}
				rleOut << endl;
				count = 0;
			}
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

