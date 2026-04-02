
//TODO :
// in ui add call or put input
// implement code for call and put options
// store all profilling results in dedicated folders
// fix build config
// are there any more improvements we can make to cache aware model?
// clean up code
// bonuses
// determine what compiler to use - if time 


// #include <iostream>
// #include <iomanip>
// #include <vector>
// #include <memory>
// #include <chrono>
// #include <string>
 
// #include "monte_carlo_engine.h"
 
// // Runs one engine and returns elapsed time in milliseconds
// double runAndPrint(IMonteCarloEngine& engine,
//                    int numberOfPaths,
//                    const OptionParameters& params,
//                    double blackScholesReference) {
 
//     auto startTime = std::chrono::high_resolution_clock::now();
//     std::pair<double, double> result = engine.price(numberOfPaths, params);
//     auto endTime   = std::chrono::high_resolution_clock::now();
 
//     double elapsedMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
//     double mcPrice   = result.first;
//     double stdError  = result.second;
//     double errorVsBS = std::abs(mcPrice - blackScholesReference);
 
//     std::cout << std::left  << std::setw(45) << engine.getName()
//               << std::fixed << std::setprecision(4)
//               << std::setw(10) << mcPrice
//               << std::setw(10) << stdError
//               << std::setw(10) << errorVsBS
//               << std::setw(10) << elapsedMs << " ms"
//               << "\n";
 
//     return elapsedMs;
// }
 
// int main() {
 
//     // --- Option parameters ---------------------------------------------------
//     OptionParameters params;
//     params.spotPrice      = 100.0;
//     params.strikePrice    = 100.0;
//     params.timeToMaturity = 1.0;
//     params.riskFreeRate   = 0.05;
//     params.volatility     = 0.20;
 
//     const int NUMBER_OF_PATHS = 1000000;
 
//     // --- Analytical Black-Scholes reference price ----------------------------
//     BlackScholes analyticalModel(params.spotPrice, params.strikePrice,
//                                   params.timeToMaturity, params.riskFreeRate,
//                                   params.volatility);
//     double bsPrice = analyticalModel.callPrice();
 
//     // --- Build the enhancement ladder ----------------------------------------
//     // Each engine adds exactly one new feature so results are directly comparable
 
//     // Layer 1 — Baseline: standard RNG, plain sampling, serial loop
//     MonteCarloEngine<StandardMersenneTwisterGenerator,
//                      PlainMonteCarloSampling,
//                      SerialExecutionPolicy>
//         baselineEngine("1 - Baseline (MersenneTwister + Plain + Serial)");
 
//     // Layer 2 — Variance reduction only: antithetic variates on baseline
//     MonteCarloEngine<StandardMersenneTwisterGenerator,
//                      AntitheticVariateSampling,
//                      SerialExecutionPolicy>
//         antitheticEngine("2 - Antithetic variates only");
 
//         // need to fix this is should be faster than antithetic engine
//     // Layer 3 — Cache-aware + AVX2 vectorisation only (serial)
//     MonteCarloEngine<XoshiroSingleThreadedGenerator,
//                      PlainMonteCarloSampling,
//                      CacheAwareVectorizedExecutionPolicy>
//         vectorizedEngine("3 - Cache-aware + AVX2 vectorized (serial)");
 
//     // Layer 4 — OpenMP parallelism only (no AVX2)
//     MonteCarloEngine<XoshiroPerThreadGenerator,
//                      PlainMonteCarloSampling,
//                      OpenMPParallelExecutionPolicy>
//         parallelEngine("4 - OpenMP parallel only");
 
//     // Layer 5 — OpenMP + antithetic (parallelism and variance reduction combined)
//     MonteCarloEngine<XoshiroPerThreadGenerator,
//                      AntitheticVariateSampling,
//                      OpenMPParallelExecutionPolicy>
//         parallelAntitheticEngine("5 - OpenMP + Antithetic variates");
 
//     // Layer 6 — Ultimate: OpenMP + AVX2 (all performance enhancements)
//     MonteCarloEngine<XoshiroPerThreadGenerator,
//                      PlainMonteCarloSampling,
//                      OpenMPWithAVX2ExecutionPolicy>
//         ultimateEngine("6 - OpenMP + AVX2 (ultimate)");

//             // Layer 7 — Ultimate: OpenMP + AVX2 (all performance enhancements) + antithetic
//     MonteCarloEngine<XoshiroPerThreadGenerator,
//                      AntitheticVariateSampling,
//                      OpenMPWithAVX2ExecutionPolicy>
//         ultimateEngineAnti("7- OpenMP + AVX2 + antithetic");
 
//     // --- Print results table -------------------------------------------------
//     std::cout << "\n";
//     std::cout << "Black-Scholes analytical price: " << std::fixed << std::setprecision(4) << bsPrice << "\n";
//     std::cout << "Paths: " << NUMBER_OF_PATHS << "\n\n";
 
//     std::cout << std::left
//               << std::setw(45) << "Engine"
//               << std::setw(10) << "MC Price"
//               << std::setw(10) << "Std Err"
//               << std::setw(10) << "Time"
//               << "\n";
//     std::cout << std::string(85, '-') << "\n";
 
//     double baselineTime = runAndPrint(baselineEngine,         NUMBER_OF_PATHS, params, bsPrice);
//                           runAndPrint(antitheticEngine,        NUMBER_OF_PATHS, params, bsPrice);
//                           runAndPrint(vectorizedEngine,        NUMBER_OF_PATHS, params, bsPrice);
//                           runAndPrint(parallelEngine,          NUMBER_OF_PATHS, params, bsPrice);
//                           runAndPrint(parallelAntitheticEngine,NUMBER_OF_PATHS, params, bsPrice);
//                           runAndPrint(ultimateEngine,          NUMBER_OF_PATHS, params, bsPrice);
//                           runAndPrint(ultimateEngineAnti,          NUMBER_OF_PATHS, params, bsPrice);
 
//     std::cout << std::string(85, '-') << "\n";
//     std::cout << "Speedup is relative to Layer 0 baseline (" << std::fixed << std::setprecision(1) << baselineTime << " ms)\n\n";
 
//     return 0;
// }
 

#include <iostream>
#include "monte_carlo_engine.h"
#include "black_scholes_model.h"
#include "simulation_helper.h"
#include "imgui_setup.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"


int main() {

    // Ui config
	if (!glfwInit())
		return 1;
	const char *glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	GLFWwindow *window = glfwCreateWindow(1920,1080, "Monte Carlo Option Pricing Simulation", NULL, NULL);
	if (window == NULL)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enables vsync

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	throw("Unable to context to OpenGL");

	int screen_width, screen_height;
	glfwGetFramebufferSize(window, &screen_width, &screen_height);
	glViewport(0, 0, screen_width, screen_height);

    UseImGui myimgui;
    myimgui.Init(window,glsl_version);

    SimulationHelper simHelper; // creating the simulation helper object
    
    while(!glfwWindowShouldClose(window)){
        glfwPollEvents();
        myimgui.NewFrame();
        myimgui.Update(simHelper);
        myimgui.Render();
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
    simHelper.StopSimulation();
    myimgui.Shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();

    std::cout << "Monte Carlo Simulation For Option Pricing!" << std::endl;


}