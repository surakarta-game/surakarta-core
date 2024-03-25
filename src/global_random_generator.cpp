#include "global_random_generator.h"
#include <cstdlib>
#include <iostream>
#include <string>

std::mt19937& GlobalRandomGenerator::getInstance() {
    static std::mt19937 generator(getSeedFromEnvironment());
    return generator;
}

void GlobalRandomGenerator::setSeed(unsigned int seed) {
    getInstance().seed(seed);
}

unsigned int GlobalRandomGenerator::getSeedFromEnvironment() {
    // following code removed due to security warning C4996
    /*
    const char* seedStr = std::getenv("CUSTOM_SEED");
    if (seedStr != nullptr) {
        try {
            return static_cast<unsigned int>(std::stoi(seedStr));
        } catch (const std::exception& e) {
            std::cerr << "Failed to parse CUSTOM_SEED: " << e.what() << std::endl;
            std::cerr << "Using std::random_device{}() instead." << std::endl;
        }
    }
    */
    return std::random_device{}();
}
