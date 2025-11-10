#define FMT_HEADER_ONLY
#include <thread>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <mutex>
#include <map>
#include <fstream>
#include <fmt/core.h>

#include "utils/json.hpp"
#include "../include/gloss.hpp"
#include "gridpaths.cpp"

std::string antennaFilename = "";
std::string output_path = "los_datasets/";

namespace gloss {

    void setAntennaFilename(std::string filename) {
        antennaFilename = filename;
    }

    void setOutputPath(std::string path) {
        output_path = path;
    }

    // Function to be executed by each thread
    void ThreadFunc(Antenna antenna, AntennaDict& antennaDict, std::mutex& dictMutex) {
        Grid paths = GetPathLoS(antenna);

        std::lock_guard<std::mutex> lock(dictMutex);
        antennaDict[antenna.id] = paths;
    }

    // Initialize all readers and settings
    void initialize(const std::string& antennaFile, const std::string& tiffFile, const std::string& groundTiffFile) {
        std::cout << "Initializing with:" << std::endl;
        std::cout << "  Antenna file: " << antennaFile << std::endl;
        std::cout << "  TIFF file: " << tiffFile << std::endl;
        std::cout << "  Ground TIFF file: " << groundTiffFile << std::endl;

        setAntennaFilename(antennaFile);
        setTiffFile(tiffFile.c_str());
        setGroundTiffFile(groundTiffFile.c_str());
        initializeReaders();
    }

    // Core computation function
    AntennaDict compute() {
        if (antennaFilename.empty()) {
            throw std::runtime_error("Antenna filename not set. Call initialize() first.");
        }

        std::vector<Antenna> antennas = getAntennas(antennaFilename);
        int numAntennas = antennas.size();

        AntennaDict antennaDict;
        std::vector<std::thread> threads;
        std::mutex dictMutex;

        for (int i = 0; i < numAntennas; ++i) {
            threads.emplace_back(ThreadFunc, antennas[i], std::ref(antennaDict), std::ref(dictMutex));
            // Temporarily removed true multithreading, which had race conditions causing seg fault crashes.
            threads[i].join();
        }

        return antennaDict;
    }

    // Save results to JSON files
    void saveResults(const AntennaDict& antennaDict) {
        for (const auto& [key, value] : antennaDict) {
            std::string filename = fmt::format("{}/los_dataset_{}.json", output_path, key);
            
            // Check if output_path directory exists, if not create it
            struct stat info;
            if (stat(output_path.c_str(), &info) != 0) {
                std::cout << "Output directory does not exist. Creating directory: " << output_path << std::endl;
                #ifdef _WIN32
                    _mkdir(output_path.c_str());
                #else 
                    mkdir(output_path.c_str(), 0777);
                #endif
            }

            json valueJson = json::array();
            for (const auto& vec : value) {
                valueJson.push_back(vec);
            }

            std::ofstream jsonFile(filename);
            if (jsonFile.is_open()) {
                jsonFile << valueJson.dump(4);
                jsonFile.close();
                std::cout << "Successfully wrote JSON to " << filename << std::endl;
            } else {
                std::cerr << "Error: Could not open file " << filename << " for writing." << std::endl;
            }
        }
    }
}

// using namespace gloss;

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <antenna_filename> <tiff_file> <ground_tiff_file>" << std::endl;
        return 1;
    }

    try {
        // Initialize with command line arguments
        gloss::initialize(argv[1], argv[2], argv[3]);

        // Run computation
        AntennaDict results = gloss::compute();

        // Save results
        gloss::saveResults(results);

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}