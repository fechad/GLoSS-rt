#include <vector>
#include <iostream>
#include <cmath>
#include <utility>
#include <random>

#include "elevation.cpp"
#include "azimuth_and_sec.cpp"


float DEFAULT_LOS_ELEVATION = 100.0;
float DEFAULT_LOS_IN_BUILDING = 50.0;
float DEFAULT_LOS_OUTSIDE_REGION = 25.0; // Assumes that the points outside the upper and lower region bounds of the antenna are null.
float DEFAULT_NLOS_ELEVATION = 0.0;

double UE_HEIGHT = 1.5;
double BUILDING_MIN_HEIGHT = 5.0;

double MAX_HORIZON_DISTANCE = 5.0; //TODO: determine the real value using propagation calculation, as done in GGTool.
const double RADIUS_STEP = 0.00001; // Granularity (step size), double check with that of the GGTool
// const double RADIUS_STEP = 0.00111; // in km, will be needed if we change how
                                        // number of steps are calculated
const int ANGLE_STEP = 1; // TODO: change to double and adapt code
const int MINIMAL_DISTANCE = 12; // All points under 12m are considered LoS

using namespace std;

Coordinate GetAntennaCoordinates(Antenna antenna) {
    //Add logic to get coordinate
    return {antenna.lat, antenna.lon};
}

Coordinate CalculateDestination(double lat, double lng, double bearing, double distanceKm){
    const double earthRadiusKm = 6371.0;
    double latRad = lat * M_PI / 180.0;
    double lngRad = lng * M_PI / 180.0;
    double bearingRad = bearing * M_PI / 180.0;
    double distanceRad = distanceKm / earthRadiusKm;

    // TODO: check if this function takes into account the lattitude when calculating the longitude
    // degrees distance to traverse, because 1 degree of longitude does NOT translate
    // to the same distance depending on the position on earth.

    double destLatRad = asin(sin(latRad) * cos(distanceRad) +
                                  cos(latRad) * sin(distanceRad) * cos(bearingRad));
    double destLngRad = lngRad + atan2(sin(bearingRad) * sin(distanceRad) * cos(latRad),
                                            cos(distanceRad) - sin(latRad) * sin(destLatRad));

    double destLat = destLatRad * 180.0 / M_PI;
    double destLng = destLngRad * 180.0 / M_PI;

    return {destLat, destLng};                                 
}

vector<Coordinate> GeneratePath(double startLat, double startLng, double endLat, double endLng) {
    // Once sure if the calculations are correct in CalculateDestination...
    // TODO: (?) change path subdivision to, instead of dividing by radius (degree) step, divides
    // by the same number of steps in all directions (or not necessary?)

    vector<Coordinate> path;
    
    double deltaLat = endLat - startLat;
    double deltaLng = endLng - startLng;
    double distance = sqrt(deltaLat * deltaLat + deltaLng * deltaLng);
    int numSteps = static_cast<int>(distance / RADIUS_STEP);

    // TODO: recheck calculation, I believe they're incorrect because
    // 1 lat VS 1 long degree don't represent the same "ground" distance here. 

    // possible improvement to be tested:    
    // int numSteps = static_cast<int>(MAX_HORIZON_DISTANCE / RADIUS_STEP);

    // TODO: should start at i = 1 to exclude antenna? probably better, but would need
    // to add antenna to the first pathLos manually as a LOS just in case a UE is under antenna (theoretically)
    for (int i = 0; i <= numSteps; ++i) {
        // TODO: fix this logic to not "grow" lng linearly, because it is not the same value depending on lattitude
        // OR switch to cartesian coordinates (ex. CRS like original data), would need to change read_tiff.cpp
        double t = static_cast<double>(i) / numSteps;
        double lat = startLat + t * deltaLat;
        double lng = startLng + t * deltaLng;
        path.emplace_back(lat, lng);
    }

    return path;
}

vector<vector<Coordinate>> GetGridPaths(Antenna antenna) {
    Coordinate antCoord = GetAntennaCoordinates(antenna);
    int totalAngle = 360;
    int angleIncrease = ANGLE_STEP;
    int numPaths = totalAngle / angleIncrease;
    
    vector<vector<Coordinate>> paths;

    // cout << "[";
    // TODO: implement "progressive" raytracing, to fill the gaps between rays at far distances from the antenna.
    for (int i=0; i<numPaths; ++i) {
        double angle = i * angleIncrease;
        Coordinate endCoord = CalculateDestination(antCoord.first, antCoord.second, angle, MAX_HORIZON_DISTANCE);
        // cout << "(" << antCoord.first << ", " << antCoord.second << "),";
        vector<Coordinate> path = GeneratePath(antCoord.first, antCoord.second, endCoord.first, endCoord.second);
        paths.push_back(path);
    }
    // cout << "]" << endl;

    return paths;
}

Grid GetPathLoS(Antenna antenna) {
    double antElevation = GetAntennaElevation(antenna);
    cout << "elevation : " << antElevation << endl;
    cout << "Azimut: " << antenna.azimuth << endl;
    cout << "dt: " << antenna.dt << endl;

    auto [lowerBound, upperBound] = calculateBounds(antenna);

    vector<vector<Coordinate>> paths = GetGridPaths(antenna);
    Grid LoSPaths;

    int pathId = 0;
    for (const auto& path : paths) {
        vector<CoordinateElevationPair> losPath;
        double lastPeakElevation = GetElevation(path[MINIMAL_DISTANCE -1].first, path[MINIMAL_DISTANCE -1].second, UE_HEIGHT); //distr(gen);
        
        // cout << "last peak elevation: " << lastPeakElevation << endl;

        double lastPeakLat = path[MINIMAL_DISTANCE -1].first;
        double lastPeakLng = path[MINIMAL_DISTANCE -1].second;

        int index = 0;
        bool reachedLOSLimit = false;
        for (const auto& point : path) {
            
            if (index < MINIMAL_DISTANCE ) { // for the first 12m everything is considered LoS
                losPath.push_back({{point.first, point.second}, DEFAULT_LOS_ELEVATION});
            } else if (pathId > upperBound || pathId < lowerBound) { // If outside working regions for directional antenna
                losPath.push_back({{point.first, point.second}, DEFAULT_LOS_OUTSIDE_REGION});
            } else {

                if (reachedLOSLimit) {
                    losPath.push_back({{point.first, point.second}, DEFAULT_NLOS_ELEVATION});
                    continue;
                    cout << "after continue" << endl;
                }
            
                double UEElevation = GetElevation(point.first, point.second, UE_HEIGHT);
               
                double newAngle = 0.0;
                if (UEElevation > antElevation) {

                    newAngle = calculateNewAngle(antElevation, antenna.lat, antenna.lon, UEElevation, point.first, point.second);
                    
                    if (newAngle > antenna.dt || antenna.dt <= 0.0) {
                        reachedLOSLimit = true;
                    }
                }

                double rayElevation = GetRayHeight(antElevation, antenna.lat, antenna.lon, UEElevation, point.first, point.second, lastPeakLat, lastPeakLng);

                if (lastPeakElevation >= rayElevation) {
                    losPath.push_back({{point.first, point.second}, DEFAULT_NLOS_ELEVATION});
                    
                } else {

                    // Compare with ground elevation to check if on a building,
                    double structureHeight = UEElevation - UE_HEIGHT - GetGroundElevation(point.first, point.second);
                    // cout << "new angle : " << newAngle << endl;
                    if (structureHeight > BUILDING_MIN_HEIGHT) {
                        losPath.push_back({{point.first, point.second}, DEFAULT_LOS_IN_BUILDING});
                    }
                    else {
                        losPath.push_back({{point.first, point.second}, DEFAULT_LOS_ELEVATION});
                    }

                    lastPeakElevation = UEElevation - UE_HEIGHT;
                    lastPeakLat = point.first;
                    lastPeakLng = point.second;
                }
                
            }

            index += 1;
        }
        LoSPaths.push_back(losPath);
        pathId += 1;
    }

    cout << "success for antenna id : " << antenna.id << endl;
    
    return LoSPaths;
}