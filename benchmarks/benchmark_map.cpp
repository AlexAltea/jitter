/**
 * (c) 2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "benchmark_map.h"
#include "jitter/map.h"
#include "benchmark.h"

#include <map>
#include <unordered_map>
#include <random>

#define LOOKUPS_WARMUP 100000
#define LOOKUPS_BENCHMARK 1000000

#define BENCHMARK_ALL(table) \
    benchmark_table(table, BENCHMARK_JITTER_MAP); \
    benchmark_table(table, BENCHMARK_JITTER_MAP_OPTIMIZED); \
    benchmark_table(table, BENCHMARK_STD_MAP); \
    benchmark_table(table, BENCHMARK_STD_UNORDERED_MAP);

#define BENCHMARK() \
    TIMER_INIT();  \
    volatile int s = 0;  \
    for (int i = 0; i < LOOKUPS_WARMUP; i++) \
        s ^= target.at(lookups_warmup[i]); \
    TIMER_START(); \
    for (int i = 0; i < LOOKUPS_BENCHMARK; i++) \
        s ^= target.at(lookups_benchmark[i]); \
    TIMER_STOP();

    
enum LookupType {
    LOOKUP_UNIFORM_DIST,
    LOOKUP_GEOMETRIC_DIST,
};

enum BenchmarkType {
    BENCHMARK_JITTER_MAP,
    BENCHMARK_JITTER_MAP_OPTIMIZED,
    BENCHMARK_STD_MAP,
    BENCHMARK_STD_UNORDERED_MAP,
};

double benchmark_table(const std::map<int, int>& table, BenchmarkType btype, LookupType ltype = LOOKUP_UNIFORM_DIST) {
    // Store and shuffle keys randomly
    std::vector<int> indices;
    for (const auto& entry : table) {
        indices.push_back(entry.first);
    }
    std::random_device rd;
    std::mt19937 rng(rd());
    std::shuffle(indices.begin(), indices.end(), rng);

    // Pre-load the keys to avoid biasing benchmarks due to RNG delays
    const char* ltext;
    std::vector<int> lookups_warmup;
    std::vector<int> lookups_benchmark;
    if (ltype == LOOKUP_UNIFORM_DIST) {
        ltext = "Uniform";
        std::uniform_int_distribution<std::mt19937::result_type> dist(0, indices.size() - 1);
        for (int i = 0; i < LOOKUPS_WARMUP; i++)
            lookups_warmup.push_back(indices[dist(rng)]);
        for (int i = 0; i < LOOKUPS_BENCHMARK; i++)
            lookups_benchmark.push_back(indices[dist(rng)]);
    } else {
        ltext = "Geometric";
        std::geometric_distribution<> dist(0.5);
        for (int i = 0; i < LOOKUPS_WARMUP; i++) {
            int index = dist(rng);
            if (index >= indices.size()) { i--; continue; }
            lookups_warmup.push_back(indices[dist(rng)]);
        }
        for (int i = 0; i < LOOKUPS_BENCHMARK; i++) {
            int index = dist(rng);
            if (index >= indices.size()) { i--; continue; }
            lookups_warmup.push_back(indices[dist(rng)]);
        }
    }

    const char* btext;
    double elapsedTime = 0.0;
    if (btype == BENCHMARK_JITTER_MAP) {
        btext = "jitter::map";
        jitter::map<int, int> target(table);
        target.compile();
        BENCHMARK();
    }
    else if (btype == BENCHMARK_JITTER_MAP_OPTIMIZED) {
        btext = "jitter::map (opt.)";
        jitter::map<int, int> target(table);
        target.compile();
        BENCHMARK();
    }
    else if (btype == BENCHMARK_STD_MAP) {
        btext = "std::map";
        std::map<int, int> target(table);
        BENCHMARK();
    }
    else if (btype == BENCHMARK_STD_UNORDERED_MAP) {
        btext = "std::unordered_map";
        std::unordered_map<int, int> target;
        for (const auto& entry : table)
            target[entry.first] = entry.second;
        BENCHMARK();
    }
    std::cout << "[*] ";
    printElement(btext, 20);
    printElement(ltext, 10);
    printElement(indices.size(), 8);
    printElement(elapsedTime, 8);
    std::cout << std::endl;
    return elapsedTime;
}

void benchmark_map() {
    std::mt19937 rng;
    std::random_device rdev;
    rng.seed(rdev());

    // Distribution
    std::uniform_int_distribution<std::mt19937::result_type> dist_int(0, INT_MAX);

    // Table
    std::map<int, int> table;

    /**
     * Benchmark 1
     * ===========
     * Consecutive numbers
     */
    for (int elements : {100, 200, 400, 800, 1600, 3200, 6400}) {
        std::uniform_int_distribution<std::mt19937::result_type> dist(0, INT_MAX - elements);
        table.clear();
        int begin = dist(rng);
        for (int i = 0; i < elements; i++) {
            table[begin + i] = dist_int(rng);
        }
        BENCHMARK_ALL(table);
        std::cout << std::endl;
    }
}
