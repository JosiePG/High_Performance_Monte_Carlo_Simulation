#ifndef RNG_POLICIES_H
#define RNG_POLICIES_H
#include <vector>
#include <random>
#include "xoshiro.h"
 
// =============================================================================
// POLICY: StandardMersenneTwisterGenerator
//
// Uses std::mt19937_64 from the C++ standard library.
// This is the BASELINE generator — correct and portable.
//
// Drawback: mt19937 holds 2.5 KB of internal state. At high path counts this
// spills out of the CPU's L1 cache, causing slowdowns.
// =============================================================================
struct StandardMersenneTwisterGenerator {
 
    std::vector<double> generateNormals(int numberOfPaths) {

        std::vector<double> randomNumbers(numberOfPaths);

        std::random_device rd; // generates random seed
        std::default_random_engine generator(rd()); // generates random numbers
        std::normal_distribution<double> normal_dist(0.0,1);

        for (int i=0;i<numberOfPaths;i++) {
            randomNumbers[i] = normal_dist(generator);
        }


        return randomNumbers;
    }
};
 
 
// =============================================================================
// POLICY: XoshiroSingleThreadedGenerator
//
// Uses xoshiro256++ (via your existing xoshiro.h).
// xoshiro holds only 32 bytes of state vs 2.5 KB for mt19937 — it almost
// always lives in CPU registers, making it significantly cache-friendlier.
//
// Used by: SerialExecutionPolicy, CacheAwareVectorizedExecutionPolicy
// =============================================================================
struct XoshiroSingleThreadedGenerator {
 
    std::vector<double> generateNormals(int numberOfPaths) {
 
        std::random_device randomSeed;
        xso::rng engine(randomSeed());
        std::normal_distribution<double> normalDistribution(0.0, 1.0);
 
        std::vector<double> randomNumbers(numberOfPaths);
        for (int i = 0; i < numberOfPaths; i++) {
            randomNumbers[i] = normalDistribution(engine);
        }
 
        return randomNumbers;
    }
};
 
 
// =============================================================================
// POLICY: XoshiroPerThreadGenerator
//
// Designed specifically for OpenMP parallel regions.
// Each CPU thread gets its own independent xoshiro generator so threads
// never share state — avoiding data races and cache line contention.
//
// How it works:
//   thread_local means each thread owns its own copy of the generator.
//   The thread ID is used as a unique seed offset so each thread produces
//   a different stream of random numbers.
//
// Used by: OpenMPParallelExecutionPolicy, OpenMPWithAVX2ExecutionPolicy
// =============================================================================
struct XoshiroPerThreadGenerator {
 
    // Satisfies the generator interface for serial use
    std::vector<double> generateNormals(int numberOfPaths) {
        return XoshiroSingleThreadedGenerator{}.generateNormals(numberOfPaths);
    }
 
    // Called once per path inside an OpenMP parallel loop.
    // Each thread has its own generator — no locking required.
    static double generateOneNormal(int threadId) {
 
        // thread_local: every thread gets its own independent copy
        thread_local xso::rng engine(
            static_cast<uint64_t>(threadId) * 6364136223846793005ULL
            + 1442695040888963407ULL
        );
        thread_local std::normal_distribution<double> dist(0.0, 1.0);
 
        return dist(engine);
    }
};

#endif