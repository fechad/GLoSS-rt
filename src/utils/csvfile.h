#ifndef CSVFILE_H
#define CSVFILE_H

#include <string>
#include <vector>

// Implementaion of a csv reader into an object
class csvFile
{
public:
    // Read csv file with separator and a given filename
    csvFile(char separator, std::string filename);

    // Returns the csv file data in a vector of a vector of strings
    std::vector<std::vector<std::string>>* getData();

    std::string getFilename() const;

private:
    char separator_;
    std::string filename_;
    std::vector<std::vector<std::string>> data_;
};

#endif