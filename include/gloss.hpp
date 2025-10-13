#ifndef GLOSS_HPP
#define GLOSS_HPP

#include <string>

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
} // namespace gloss

#endif // GLOSS_HPP