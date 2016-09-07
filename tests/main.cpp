/**
 * (c) 2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "test_map.h"

#include <iostream>

int main(int argc, char** argv) {
    if (!test_map()) {
        std::cout << "Tests: jitter::map failed!" << std::endl;
    }
    return 0;
}
