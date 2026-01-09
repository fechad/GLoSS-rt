/**
 * @file utils.cpp
 * @brief Utility functions and class implementations
 */

#include "utils.hpp"
#include "gloss.hpp"
#include <iostream>

namespace gloss {

    std::string getVersion() {
        return Version::getString();
    }
} // namespace gloss

