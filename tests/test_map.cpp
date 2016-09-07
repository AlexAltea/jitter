/**
 * (c) 2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "test_map.h"
#include "jitter/map.h"

#include <map>

bool test_map() {
    jitter::map<int, int> map1({
        {0,  1000},
        {1,  1001},
        {2,  1002},
        {3,  1003},
        {4,  1004},
        {5,  1005},
        {6,  1006},
        {7,  1007},
        {8,  1008},
        {20, 1009},
        {21, 1010},
        {22, 1011},
        {23, 1012},
        {24, 1013},
    });

    map1.compile();
    int t = map1.at(7);
    return t == 1007;
}
