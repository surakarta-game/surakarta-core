#pragma once

#include <random>

class GlobalRandomGenerator {
   public:
    static std::mt19937& getInstance();
    static void setSeed(int seed);

   private:
    static int getSeedFromEnvironment();
};
