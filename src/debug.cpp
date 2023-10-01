#include "Debug.h"
#include <iostream>

namespace Debug {
    bool Log(const char* message) {
        std::cout << message << std::endl;
        return true;
    }

    bool Log(int message) {
        std::cout << message << std::endl;
        return true;
    }
}