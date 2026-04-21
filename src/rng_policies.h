#ifndef RNG_POLICIES_H
#define RNG_POLICIES_H
#include <vector>
#include <random>
#include "xoshiro.h"
 
// Standard-library-based RNG
struct StandardMersenneTwisterGenerator {
 
    std::vector<double> generateNormals(int numberOfPaths) {

        std::vector<double> randomNumbers(numberOfPaths);

        std::random_device rd; // Generates random seed
        std::default_random_engine generator(rd()); // Generates random numbers
        std::normal_distribution<double> normal_dist(0.0,1); // Normal (Gaussian) distribution with mean 0 and std dev 1

        // Generates one random number per path
        for (int i=0;i<numberOfPaths;i++) {
            randomNumbers[i] = normal_dist(generator);
        }


        return randomNumbers;
    }
};
 
// Uses a high-performance RNG (xoshiro family)
struct XoshiroSingleThreadedGenerator {
 
    std::vector<double> generateNormals(int numberOfPaths) {
 
        std::random_device randomSeed; // Generates random seed
        xso::rng engine(randomSeed()); // Generates random numbers using xoshiro
        std::normal_distribution<double> normalDistribution(0.0, 1.0); // Normal (Gaussian) distribution with mean 0 and std dev 1
 
        std::vector<double> randomNumbers(numberOfPaths);
        // Generates one random number per path
        for (int i = 0; i < numberOfPaths; i++) {
            randomNumbers[i] = normalDistribution(engine);
        }
 
        return randomNumbers;
    }
};
 
// Designed for parallel engines where each thread has its own RNG.
struct XoshiroPerThreadGenerator {
 
    // Satisfies the generator interface for serial use
    std::vector<double> generateNormals(int numberOfPaths) {
        return XoshiroSingleThreadedGenerator{}.generateNormals(numberOfPaths);
    }
 
    // Called once per path inside an OpenMP loop
    // threadId ensures each thread gets a different RNG stream
    static double generateOneNormal(int threadId) {
 
        // thread_local: every thread gets its own independent copy
        // Seed is computed as: a * threadId + b which gives each thread a different deterministic seed
        thread_local xso::rng engine(
            static_cast<uint64_t>(threadId) * 6364136223846793005ULL
            + 1442695040888963407ULL
        ); // constants from linear congruential generators (LCG)

        // Each thread also has its own normal distribution object
        thread_local std::normal_distribution<double> dist(0.0, 1.0);
 
        // Generates one random variable
        return dist(engine);
    }
};

#endif