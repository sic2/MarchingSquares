#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

class DataAcquisition
{
public:
	/**
	* @param fileName containing the data to be displayed
	* @param rows [OUT] 
	* @param columns [OUT]
	* @return data to be displayed as int[columns][rows]
	*/
	inline static int** getData(std::string fileName, int* rows, int* columns)
	{
		int** retval;

		// File input routine as in: http://www.cplusplus.com/doc/tutorial/files/
		std::ifstream file (fileName.c_str());
		std::string line;
		if (file.is_open())
		{
			while (getline(file,line))
			{ 
				std::stringstream lineStream(line);
			    std::string token;
			    // Get array dimension
			    lineStream >> token; // columns
			    *columns = atoi(token.c_str());
			    lineStream >> token; // rows
			    *rows = atoi(token.c_str());

			    // Initialise data array
			    retval = (int**) malloc(*columns * sizeof(int*));  
				for (int i = 0; i < *columns; i++) retval[i] = (int*) malloc(*rows * sizeof(int));  

			    int row = 0;
			    int column = 0;
			    while(lineStream >> token)
			    {
			        retval[column][row] = atoi(token.c_str());
			        column++;
			        if ((column % *columns) == 0)
			        {
			        	column = 0;
			        	row++;
			        }
			    }
			} // end outer while loop
			file.close();
		} 
		else
		{
			std::cout << "Unable to open file " << fileName;
		}  

		return retval;
	}

	/**
	* Free data
	*/
	inline static void freeData(int** data, int columns)
	{
		for (int i = 0; i < columns; i++) free(data[i]);
		free(data);
	}
};