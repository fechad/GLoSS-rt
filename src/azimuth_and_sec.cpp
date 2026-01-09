#include <iostream>
#include <cmath>
#include <tuple>
#include <string>

//#include "antennas.cpp"


double extractNumberFromName(const std::string& input) {
    // Find the position of the first non-digit character
    std::size_t pos = input.find_first_not_of("0123456789.");
    
    // Extract the substring from the start up to the first non-digit character
    std::string numberPart = input.substr(0, pos);

    // Convert the extracted string to double
    return std::stod(numberPart);
}


// Function to calculate the upper and lower angle bounds for an antenna
std::tuple<double, double> calculateBounds(Antenna antenna) {

    if (antenna.name == "OMNI") return std::make_tuple(0.0, 360.0);

    double azimuth = antenna.azimuth;
    double sectorDegree = extractNumberFromName(antenna.name);
    double lowerBound = azimuth - sectorDegree / 2.0;
    double upperBound = azimuth + sectorDegree / 2.0;

    // Normalize the angles to be within [0, 360] degrees
    if (lowerBound < 0.0) {
        lowerBound += 360.0;
    }
    if (upperBound >= 360.0) {
        upperBound -= 360.0;
    }

    std::cout << "lower: " << lowerBound << std::endl;
    std::cout << "upper: " << upperBound << std::endl;

    return std::make_tuple(lowerBound, upperBound);
}

// int main() {
//     double azimuth = 210.0;     
//     double sectorDegree = 65.0; 

//     auto [lowerBound, upperBound] = calculateBounds(azimuth, sectorDegree);

//     std::cout << "Lower Bound: " << lowerBound << " degrees\n";
//     std::cout << "Upper Bound: " << upperBound << " degrees\n";

//     return 0;
// }
