#ifndef GLOSS_HPP
#define GLOSS_HPP

#include <string>
#include <map>
#include <vector>
#include <utility>

using Coordinate = std::pair<double, double>;
using Elevation = float;
using CoordinateElevationPair = std::pair<Coordinate, Elevation>;
using Grid = std::vector<std::vector<CoordinateElevationPair>>;
using AntennaDict = std::map<int, Grid>;


namespace gloss {

    /**
     * @brief Main library version information
     */
    struct Version {
        static constexpr int MAJOR = 0;
        static constexpr int MINOR = 0;
        static constexpr int PATCH = 1;

        static std::string getString() {
            return  std::to_string(MAJOR) + "." +
                    std::to_string(MINOR) + "." +
                    std::to_string(PATCH);
        }
    };

    void initialize(const std::string& antennaFile, const std::string& tiffFile, const std::string& groundTiffFile);
    AntennaDict compute();
    void saveResults(const AntennaDict& antennaDict);
} // namespace gloss

#endif // GLOSS_HPP