#include <vector>
#include <iostream>
#include <string>
#include <cstdio>
#include <memory>
#include <map>
#include <random>

#include "classes/antennas.cpp"
#include "classes/read_tiff.cpp"


using Coordinate = std::pair<double, double>;
using Elevation = float;
using CoordinateElevationPair = std::pair<Coordinate, Elevation>;
using Grid = std::vector<std::vector<CoordinateElevationPair>>;
using AntennaDict = std::map<int, Grid>;


// For testing purposes

std::random_device rd; // obtain a random number from hardware
std::mt19937 gen(rd()); // seed the generator
std::uniform_int_distribution<> distr(10, 60);

//
// const char* tiffFile = "data/montreal/montreal.tif";
// const char* groundTiffFile = "data/montreal/montreal_MNT.tif";

// ElevationReader reader(tiffFile);
// ElevationReader groundReader(groundTiffFile);

const char* tiffFile;
const char* groundTiffFile;

ElevationReader reader;
ElevationReader groundReader;

void setTiffFile(const char* filename) {
    tiffFile = filename;
}

void setGroundTiffFile(const char* filename) {
    groundTiffFile = filename;
}

void initializeReaders() {
    std::cout << "Initializing readers with files: " << tiffFile << " and " << groundTiffFile << std::endl;
    reader = ElevationReader(tiffFile);
    groundReader = ElevationReader(groundTiffFile);
}

// const char* groundTiffFile = "data/montreal/montreal_MNT.tif";
// ElevationReader groundReader(groundTiffFile);

const double FEET_METERS = 0.3048; // Convert from feet to meters

float exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    
    try
    {
        float elevation = std::stof(result);
        return elevation;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return 0.0;
}

double GetAntennaElevation(Antenna antenna){
    return (antenna.height + antenna.gndElevation) * FEET_METERS;
}

double GetUEHeight(){
    // TODO: Should be 1.5m over GROUND elevation! (need to use the second tif, data/montreal_MNT.tif)
    return 1.5;
}

// Placeholder function to calculate distance between two points (Haversine formula)
double CalculateDistance(double lat1, double lon1, double lat2, double lon2) {
    // TODO: validate that function is correct, and always returns positive numbers etc.
    const double R = 6371e3; // Earth's radius in meters
    double phi1 = lat1 * M_PI / 180.0;
    double phi2 = lat2 * M_PI / 180.0;
    double deltaPhi = (lat2 - lat1) * M_PI / 180.0;
    double deltaLambda = (lon2 - lon1) * M_PI / 180.0;

    double a = std::sin(deltaPhi / 2) * std::sin(deltaPhi / 2) +
               std::cos(phi1) * std::cos(phi2) *
               std::sin(deltaLambda / 2) * std::sin(deltaLambda / 2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));

    double distance = R * c; // in meters
    return distance;
}

double GetRayHeight(double antElevation, double antLat, double antLng, double UEElevation, double UELat, double UELng, double lastPeakLat, double lastPeakLng) { //update lastPeaks type
    double opp = antElevation - UEElevation;
    double adj = CalculateDistance(antLat, antLng, UELat, UELng);
    double newAdj = CalculateDistance(lastPeakLat, lastPeakLng, UELat, UELng);

    double newOpp = opp * newAdj / adj;

    return UEElevation + newOpp;
}

double calculateNewAngle(double antElevation, double antLat, double antLng, double UEElevation, double UELat, double UELng) {
    double opp = UEElevation - antElevation; // Vertical difference (opposite side)
    double adj = CalculateDistance(antLat, antLng, UELat, UELng); // Horizontal distance (adjacent side)

    double newAngle = atan2(opp, adj);

    // Convert angle to degrees
    newAngle = newAngle * (180.0 / M_PI);

    return newAngle;
}


double GetElevation(double latitude, double longitude, double height) {
    //Call GDAL to get elevation at coord
    // std::string lat = std::to_string(latitude);
    // std::string lng = std::to_string(longitude);
    // std::string command = "python get_elevation.py " + lat + " " + lng;

    // // std::cout << "Cmd to run: " << command << std::endl;
    
    // int result = std::system(command.c_str());
    
    // Execute the command and capture the output
    // float elevation = exec(command.c_str()); //distr(gen);
    
    // Print the output
    // std::cout << "lat and lon used is : " << latitude << ", " << longitude << std::endl;


    double elevation = reader.getElevation(latitude, longitude);

    if (elevation == -1) {
        // TODO: check nodata value in code, because -1 should be possible in city "valleys" unless it is nodata.

        std::cout << "elevation is -1" << std::endl;
        return -1000;
    }
    return elevation + height;
}

double GetGroundElevation(double latitude, double longitude) {
    double gndElevation = groundReader.getElevation(latitude, longitude);
    if (gndElevation == -1) {
        // TODO: check nodata value in code, because -1 should be possible in city "valleys" unless it is nodata.
        std::cout << "elevation is -1" << std::endl;
        return -1000;
    }
    return gndElevation;
}
