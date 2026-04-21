
//TODO :
// add to project log and clean it up
// clean up code ( make sure namings make sense eg model / engine )
// bonuses
// fix mean and min time bug not appearing sometimes in sim history

#include "black_scholes_model.h"
#include "monte_carlo_engine.h"
#include "option_parameters.h"
#include "simulation_helper.h"
#include "imgui_setup.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>




int main() {

    // Ui config

	if (!glfwInit()) return 1; //Initializes GLFW
	const char *glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Requests OpenGL 3.0 context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	GLFWwindow *window = glfwCreateWindow(1920,1080, "Monte Carlo Option Pricing Simulation", NULL, NULL); // creates main application window
	if (window == NULL)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	throw("Unable to context to OpenGL");

	int screen_width, screen_height;
	glfwGetFramebufferSize(window, &screen_width, &screen_height);
	glViewport(0, 0, screen_width, screen_height);

    // Initializes the UseImGui helper object
    UseImGui myimgui;
    myimgui.Init(window,glsl_version);

    SimulationHelper simHelper; // Creating the simulation helper object
    
    // Main application loop
    while(!glfwWindowShouldClose(window)){
        glfwPollEvents(); // Handles input events 
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

//      MonteCarloEngine<XoshiroPerThreadGenerator,
//                      AntitheticVariateSampling,
//                      OpenMPParallelExecutionPolicy>
//                      ultimateEngine;


//     auto result = ultimateEngine.price(no_of_paths, optParams);
//     return 0;
// }
    // MonteCarloEngine<StandardMersenneTwisterGenerator,
    //              PlainMonteCarloSampling,
    //              SerialExecutionPolicy>
    //                  vanillaEngine;
//     MonteCarloEngine<StandardMersenneTwisterGenerator,
//                      AntitheticVariateSampling,
//                      SerialExecutionPolicy>
//                      varianceEngine;
//     MonteCarloEngine<XoshiroSingleThreadedGenerator,
//                  PlainMonteCarloSampling,
//                  CacheAwareVectorizedExecutionPolicy>
//                      cacheEngine;
// MonteCarloEngine<XoshiroPerThreadGenerator,
//                  PlainMonteCarloSampling,
//                  OpenMPParallelExecutionPolicy>
//                      parallelEngine;
//      MonteCarloEngine<XoshiroPerThreadGenerator,
//                      AntitheticVariateSampling,
//                      OpenMPParallelExecutionPolicy>
//                      parallel_and_varianceEngine;