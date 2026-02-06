//TODO: 
// - plot histogram distribution of microsecond values ? what other graphs can we show? only show if nerd mode is on 
// - pin runSimulaiton funciton to a single thread : SetThreadAffinityMask(GetCurrentThread(), 1) ? do later , quite advanced
// - identify the reasons for varibaility and what we have done to mitigate this 
// design ui layout , what windows do we want ? Plots of convergance lines , accuracy bench marks , profiling results , speed stats?
// plot cache l1 l2 l3 against gflops 
// need to keep track of last model runs like a history tab - do after interview  


//TODO : Need to fix issue of model with higher number of paths not awlays being more accurate than model with lower numbe rof paths
//TODO : Add Standard Error metric (add show tip to determine how its calculated )
//TODO : Need to validate user inputs

#include <iostream>
#include "monte_carlo_engine.h"
#include "variance_reduction_module.h"
#include "cache_aware_module.h"
#include "black_scholes_model.h"
#include "simulation_helper.h"
#include "imgui_setup.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"



int main() {

    // ui setup


    // Setup window
	if (!glfwInit())
		return 1;

	// GL 3.0 + GLSL 130
	const char *glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	// Create window with graphics context
	GLFWwindow *window = glfwCreateWindow(1920,1080, "Monte Carlo Option Pricing Simulation", NULL, NULL);
	if (window == NULL)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))  // tie window context to glad's opengl funcs
	throw("Unable to context to OpenGL");

	int screen_width, screen_height;
	glfwGetFramebufferSize(window, &screen_width, &screen_height);
	glViewport(0, 0, screen_width, screen_height);

    UseImGui myimgui;
    myimgui.Init(window,glsl_version);

    SimulationHelper simHelper;
    
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


    //BlackScholes bs_model(spotPrice,strikePrice,timeToMaturity,riskFreeRate,volatility);

    // BlackScholes bs_model(100.0,100.0,1.0,0.05,0.2);
    // MonteCarloEngine vanilla_model;

    // VarianceReductionModule variance_reduction_model;
    // CacheAwareModule cache_aware_model;

    //double vanilla_estimated_value = vanilla_model.runSimulation(no_of_paths,spotPrice,strikePrice,timeToMaturity,riskFreeRate,volatility);
    //double variance_reduction_estimated_value = variance_reduction_model.runSimulation(no_of_paths,spotPrice,strikePrice,timeToMaturity,riskFreeRate,volatility);

    // double vanilla_estimated_value = vanilla_model.runSimulation(10000,100.0,100.0,1.0,0.05,0.2);
    // double variance_reduction_estimated_value = variance_reduction_model.runSimulation(10000,100.0,100.0,1.0,0.05,0.2);
    // double cache_aware_estimated_value = cache_aware_model.runSimulation(10000,100.0,100.0,1.0,0.05,0.2);
    // double black_scholes_estimated_value = bs_model.callPrice();

    // // TODO : add these outputs 
    // // speed of model in seconds
    // // plot error distributions 
    // // run multiple iterations changing the number of simulations and showing the different results

    // std::cout << "This is the black scholes value of the option: " << black_scholes_estimated_value << std::endl;
    // std::cout << "This is the vanilla model estimated value of the option: " << vanilla_estimated_value << std::endl;
    // vanilla_model.benchmark(10000,100.0,100.0,1.0,0.05,0.2);
    // std::cout << "This is the variance reduction model estimated value of the option: " << variance_reduction_estimated_value << std::endl;
    // variance_reduction_model.benchmark(10000,100.0,100.0,1.0,0.05,0.2);
    // std::cout << "This is the cache aware model estimated value of the option: " << cache_aware_estimated_value << std::endl;
    // cache_aware_model.benchmark(10000,100.0,100.0,1.0,0.05,0.2);


}