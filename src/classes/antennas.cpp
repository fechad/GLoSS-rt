#include <iostream>
#include "../utils/json.hpp"
#include "../utils/csvfile.cpp"

using json = nlohmann::json;

class Antenna {
public:
    int id;
    double lat;
    double lon;
    double height; // in ft.
    double frequency;
    double erp;
    std::string name;
    double azimuth;
    double dt;
    double bandwidth;
    double ba;
    double gndElevation; // in ft.

    Antenna(int id, double lat, double lon, double height, double frequency, double erp,
            std::string name, double azimuth, double dt, double bandwidth, double ba, double gndElevation ) :
                id(id), lat(lat), lon(lon), height(height), frequency(frequency), erp(erp),
                name(name), azimuth(azimuth), dt(dt), bandwidth(bandwidth), ba(ba), gndElevation(gndElevation) {}

    void print() const {
        std::cout << "ID: " << id << ", Lat: " << lat << ", Lon: " << lon << std::endl;
    }
};

// Data position in the csv file
enum fileFormat {LAT, LON, HGT, FRQ, ERP, NAME, AZ, DT, BW, BA, GNDELV};

std::vector<Antenna> getAntennas(std::string filePath) {
    // Read csv
    csvFile* antFile = new csvFile(';', filePath);
    
    std::vector<Antenna> AntennaList;
    int i = 1;
    for (const auto& csvLine : (*antFile->getData())) {
        Antenna ant(i++, std::stof(csvLine[LAT]), std::stof(csvLine[LON]), std::stof(csvLine[HGT]),
                    std::stof(csvLine[FRQ]), std::stof(csvLine[ERP]), csvLine[NAME], std::stof(csvLine[AZ]),
                    std::stof(csvLine[DT]), std::stof(csvLine[BW]), std::stof(csvLine[BA]), std::stof(csvLine[GNDELV]));
        AntennaList.push_back(ant);
    }

    return AntennaList;
}