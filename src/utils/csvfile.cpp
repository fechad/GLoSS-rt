#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include "csvfile.h"

// Read csv into a vector of vector of strings
csvFile::csvFile(char separator, std::string filename): 
        separator_(separator), filename_(filename)
{
	// Open file
	std::ifstream file ( filename );
	if (file.good()) {
		std::string line = "";
		size_t pos = 0;

        // Bool to check if file is empty
        bool emptyFile = true;
        // Read all lines in the csv
        while (std::getline(file, line)) {
            emptyFile = false;
            std::vector<std::string> lineVec;
            do {
                pos = line.find(separator); // Find separator
                // push_back string up till separator
                lineVec.push_back( line.substr(0, pos) );
                // erase pushed-backed string and the separator
                line.erase(0, pos + 1);
            } while ( pos != std::string::npos);
            // Push back line vector
            data_.push_back(lineVec);
        }
        if (emptyFile) {
            std::cout << filename << " is empty!\n";
            std::exit(EXIT_FAILURE);
        }
    }
	else {
		std::cout << "Error reading " << filename;
		std::exit(EXIT_FAILURE);
	}
}  

std::string csvFile::getFilename() const
{
    return filename_;
}

std::vector<std::vector<std::string>>* csvFile::getData()
{
    return &data_;
}