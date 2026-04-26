# Project Log

## Week 1 [6/10 to 12/10 2025]

- Read Monte Carlo Simulation for European Option Pricing: Methodology, Error Analysis, and Algorithmic Enhancements
    This provided me with the background knowledge of implementing a monte carlo simulation for option pricing 
    .And provided me with pitfalls of monte carlo simulations and how I can improve them through variance reduction.

    Control Variate : Incorporate the black scholes price to reduce variance of simulations 
    Antithetic Variates : For each estimated random variable Z use its antithetic counterpart -Z to simulate an additional path

- Reflection : 
  Monte Carlo methods converge slowly (O(1/√N)), highlighting the need for:
  * variance reduction
  * computational optimisation

  Outcome:
  Planned to implement antithetic variates and use Black-Scholes as a benchmark.

## Week 2 [13/10 to 17/10]
- Read Monte Carlo Financial Engineering Book 
- Explored Low Latency C++ Techniques such as cache aware algorithms and SIMD vectorization

  Reflection:
  From this i identified that performance through hardware is going to be a key focus area in the project.

## Week 3 [20/10 to 24/10]
- Implemented Vanilla Monte Carlo Simulation
- Focused on correctness of stochastic modelling

## Week 4 [27/10 to 31/10]
- Created basic command line interface for testing
- Validated results against expected option pricing behaviour

  Reflection:
  This work ensured correctness early on which reduced the need for debugging later as the system became more complex.

## Week 5 [3/11 to 07/11]
- Started writing interim report
- Started implementing Variance Reduction logic for monte carlo simulation


## Week 6 [10/11 to 14/11]
- Continued development of variance reduction logic
- Which then formed a part of the Variance Reduction Techniques section within the interim report.

  Reflection:
  The insight in using antithetic variates to improve accuracy was successful as antithetic variates reduced variance with minimal computational overhead.

## Week 7 [17/11 to 21/11]
- Continued writing interim report
- And began reviewing early design decisions in terms of class diagrams and data structures

## Week 8 [24/11 to 28/11]
- Finished writing interim report


## Week 9 [1/12 to 05/12]
- Refactored code into classes
- Created MonteCarloEngine Class
- Created VarianceReductionModule Class

  Reflection:
  This refactor improved flexibility and maintainability within the entire codebase.

## Week 10 [1/12 to 12/12]
- Created CacheAwareModule Class

## Week 11 to 12 [15/12 to 26/12]
- Christmas Break

## Week 13 [29/12 to 02/01]
- Completed Variance Reduction Module 
- Started working on Cache Aware Module

## Week 14 [05/01 to 9/01]
- Completed Variance Reduction Module 
- Started working on Cache Aware Module

## Week 15 [12/01 to 16/01]
- Added Imgui library to project
- Implemented Input Parameters UI

  Reflection:
  Switching from a basic command line interface to the graphical user interface ImGui , dramatically changed the entire look and feel of the project. It enhanced the user experience as well as making it more suitable for professional purposes.

## Week 16 [19/01 to 23/01]
- Expanded UI functionality
- Improved layout and usability

## Week 17  [25/01 to 06/02]
- Added Ui Simulation Manager
- Added Benchmark plots

  Reflection:
  Implementing the simulation manager allowed for thread safe interaction between the simulation logic and UI logic. This enabled the system to produce accurate and deterministic results. 

## Week 18  [25/01 to 06/02]
 - Added confidence interval metrics
 - Added standard error metrics 
 - Enabled comparison between models through summary statistics

## Week 19 [08/02 to 13/02]
- Added Convergence Plot

  Reflection:
  Implementing the convergence plot , was imperative to the academic and research based aspects of my project. As this plot provides insight towards the convergence behavior of Monte Carlo simulations. Additionally it provides a clear visualization of the impact of the variance reduction engine.

## Week 20 [15/02 to 20/02]
- Replaced Mersenne twister with xoshiro rng
- Introduced SIMD vectorization

  Reflection:
  Implementing the convergence plot , was imperative to the academic and research based aspects of my project. As this plot provides insight towards the convergence behavior of Monte Carlo simulations. Additionally it provides a clear visualization of the impact of the variance reduction engine.



## Week 21 [22/02 to 27/02]

- Refactored architecture into policy-based design:
  - Execution policies (serial, vectorised, parallel)
  - RNG policies (mt19937, xoshiro variants)
  - Sampling policies (plain, antithetic)

## Week 22 [01/03 to 06/03]
- Implemented efficient Monte Carlo sampling using precomputed constants  

  Reflection:
  In order to accurately measure the performance enhancements from the optimized Monte Carlo engines , it was more beneficial to implement the baseline Monte Carlo sampling policy with the obvious and naive enhancements such as using precomputed constants. Overall, this added to the legitimacy of the performance comparisons between engines.


## Week 23 [08/03 to 13/03]

- Refactored cache-aware vectorised execution policy  
  - Improved aligned memory access patterns  
  - Integrated more SIMD-friendly loop structures   
- Reduced branching in inner loops to improve SIMD utilisation  


## Week 24 [15/03 to 20/03]

- Profiled cache-aware vectorised execution policy against the serial execution policy.
- Noticed that enhancements were in fact working as the cache-aware vectorised execution policy was faster than the serial execution policy. 


## Week 25 [22/03 to 27/03]

- Implemented OpenMP-based parallel execution policy  
  - Distributed paths across CPU cores using `#pragma omp parallel for`  
  - Introduced reduction variables for thread-safe accumulation  
 
  Reflection:
  Through intermediary profiling of the cache-aware vectorized execution policy it became apparent that the primary bottleneck of the simulation was the lack of CPU core utilization. Therefore the OpenMP-based parallel execution policy was a clear and natural addition to the optimization engines.  


## Week 26 [29/03 to 03/04]

- Designed per-thread RNG (xoshiro) to eliminate contention  
- Addressed false sharing and ensured thread-local independence  

  Reflection:
  In order to ensure thread safety within the parallel execution policy , a per thread random number generator was created. This meant that the parallel engine was not only faster but also statistically accurate and robust.

## Week 27 [04/04 to 10/04]

- Integrated Intel VTune profiling workflow:
  - Identified hotspots in simulation loop  
  - Analysed CPU utilisation and memory access patterns  

## Week 28 [04/04 to 10/04]

- Finished writing first draft of dissertation 
- Re adjusted GUI window sizings , to make the UI feel more professional


## Week 29 [11/04 to 19/04]

- Continued writing dissertation 
- Updated Readme.md file 
- Cleaned up overall code structure


## Week 30 [20/04 to 27/04]
- Finished writing dissertation


