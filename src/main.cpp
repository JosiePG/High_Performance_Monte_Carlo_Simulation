
//TODO :


// add to project log and clean it up
// make gui window sizes look nice
// set up readme 
// store all profilling results in dedicated folders
// clean up code
// bonuses
// fix mean and min time bug not appearing sometimes in sim history

 

#include <iostream>
#include "monte_carlo_engine.h"
#include "option_parameters.h"
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

// profiling code

// int main() {
// OptionParameters optParams;

// optParams.spotPrice      = 100.0;   // S0
// optParams.strikePrice    = 100.0;   // K (ATM option)
// optParams.timeToMaturity = 1.0;     // 1 year
// optParams.riskFreeRate   = 0.05;    // 5%
// optParams.volatility     = 0.2;     // 20%
// optParams.optionType     = OptionType::CALL;    // assume 0 = call

// int no_of_paths = 1000000000;

// MonteCarloEngine<XoshiroPerThreadGenerator,
//                  EfficentMonteCarloSampling,
//                  OpenMPParallelExecutionPolicy>
//                      parallelEngine;



//     auto result = parallelEngine.price(no_of_paths, optParams);
//     return 0;
// }
    // MonteCarloEngine<StandardMersenneTwisterGenerator,
    //              PlainMonteCarloSampling,
    //              SerialExecutionPolicy>
    //                  vEngine;
// //     MonteCarloEngine<StandardMersenneTwisterGenerator,
// //                      AntitheticVariateSampling,
// //                      SerialExecutionPolicy>
// //                      varianceEngine;
// //     MonteCarloEngine<XoshiroSingleThreadedGenerator,
// //                  EfficentMonteCarloSampling,
// //                  CacheAwareVectorizedExecutionPolicy>
// //                      cacheEngine;
// // MonteCarloEngine<XoshiroPerThreadGenerator,
// //                  EfficentMonteCarloSampling,
// //                  OpenMPParallelExecutionPolicy>
// //                      parallelEngine;
// //      MonteCarloEngine<XoshiroPerThreadGenerator,
// //                      AntitheticVariateSampling,
// //                      OpenMPParallelExecutionPolicy>
// //                      ultimateEngine;