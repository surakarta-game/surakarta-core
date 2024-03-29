#pragma once

#include <random>

class GlobalRandomGenerator {
   public:
    static std::mt19937& getInstance();
    static void setSeed(unsigned int seed);

   private:
    static unsigned int getSeedFromEnvironment();
};
