# Project Log

Please regularly update this file to record your project progress. You should be updating the project log _at least_ once a fortnight.
- detail your actions here
- you can include details of commits if necessary
- alternatively, details of any reading you have completed in support of your project
## Week 1 [6/10 to 12/10 2025]

- Read Monte Carlo Simulation for European Option Pricing: Methodology, Error Analysis, and Algorithmic Enhancements
    This provided me with the background knwoledge of implementing a monte carlo simulation for option pricing 
    .And provided me with pitfalls of monte carlo simulations and how I can improve them through variance reduction.

    Control Variate : Incorporate the black scholes price to reduce variance of simulations 
    Antithetic Variates : For each estimated random variable Z use its antithetic counterpart -Z to simulate an additonal path



## Week 2 [13/10 to 17/10]
- Read Monte Carlo Financal Engineering Book 
- Explored Low Latency C++ Techniques 

## Week 3 to 4 [20/10 to 31/10]
- Implemented Vanilla Monte Carlo Simulation 
- Create basic command line interface for testing

## Week 5 to 6 [3/11 to 14/11]
- Started writing interim report
- Started implementing VarianceReduction logic for monte carlo simulation

## Week 7 to 8 [17/11 to 28/11]
- Finished writing interim report

## Week 8 to 10 [1/12 to 12/12]
- Refactored code into classes
- Created MonteCarloEngine Class
- Created VarianceReductionModule Class
- Created CacheAwareModule Class

## Week 12 to 14 [15/12 to 26/12]
- Christmas Break

## Week 14 to 16 [29/12 to 9/01]
- Complemted Variance Reduction Module 
- Started working on Cache Aware Module

## Week 16 to 18 [12/01 to 23/01]
- Added Imgui library to project
- Implemented Input Paramters UI

## Week 18 to 20 [25/01 to 06/02]
- Added Ui Simulation Manager
- Added Benchmark plots


## Week 20 to 22 [08/02 to 20/02]
- Added Convergance Plot
- Replaced merseene twister with xoshiro rng
- Added vectorization

## Week 22–24 [22/02 to 06/03]

- Refactored architecture into policy-based design:
  - Execution policies (serial, vectorised, parallel)
  - RNG policies (mt19937, xoshiro variants)
  - Sampling policies (plain, antithetic)
- Implemented efficient Monte Carlo sampling using precomputed constants  

---

## Week 24–26 [08/03 to 20/03]

- Implemented cache-aware vectorised execution policy  
  - Added aligned memory access patterns  
  - Integrated SIMD-friendly loop structures  
- Integrated SLEEF library for vectorised exponential computation  
- Reduced branching in inner loops to improve SIMD utilisation  

---

## Week 26–28 [22/03 to 03/04]

- Implemented OpenMP-based parallel execution policy  
  - Distributed paths across CPU cores using `#pragma omp parallel for`  
  - Introduced reduction variables for thread-safe accumulation  
- Designed per-thread RNG (xoshiro) to eliminate contention  
- Addressed false sharing and ensured thread-local independence  

---

## Week 28–Current [04/04 to 05/04]

- Integrated Intel VTune profiling workflow:
  - Identified hotspots in simulation loop  
  - Analysed CPU utilisation and memory access patterns  


