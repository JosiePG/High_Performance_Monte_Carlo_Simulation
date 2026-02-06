#include "imgui_setup.h"
#include <algorithm>
#include <filesystem>
#include <iostream>

//TODO add statistical error metrics 


void UseImGui::Init(GLFWwindow* window,const char* glsl_version){
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    
    io.Fonts->AddFontFromFileTTF(
        
    "fonts/SplineSansMono[wght].ttf",
    22.0f   
);
    
 
    ImGui_ImplGlfw_InitForOpenGL(window,true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsClassic();

}

void UseImGui::NewFrame(){
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
    

}

void UseImGui::Update(SimulationHelper & simHelper){

    struct RollingBuffer {
        ImVector<ImVec2> Data;
        RollingBuffer() {
            Data.reserve(2000);
        }
        void AddPoint(float x, float y) {
            Data.push_back(ImVec2(x, y));
        }
    };



    static RollingBuffer   rdata2;
    static int lastIteration = -1; // why does this have to be static

    resultsCache = simHelper.GetResults();
    cachedProgress = simHelper.GetProgress();
    cachedIsRunning = simHelper.IsRunning();

    ImGui::SetNextWindowSizeConstraints(
    ImVec2(250.0f, 250.0f),          // min size
    ImVec2(600.0f, FLT_MAX)      // max width = 600 px, unlimited height
);
    ImGui::Begin("Parameter Settings");
    ImGui::Text("(CTRL + Click) to enter input manually");
    ImGui::SeparatorText("Option Inputs");
        static double spotPrice = 50.0;
    ImGui::InputDouble("spot price", &spotPrice, 1.0f, 2.0f, "%.3f");
        static double strikePrice = 100.0;
    ImGui::InputDouble("strike price", &strikePrice, 1.0f, 2.0f, "%.3f");
        static double timeToMaturity = 2.0;
    ImGui::InputDouble("time to maturity", &timeToMaturity, 1.0f, 2.0f, "%.3f");
        static double riskFreeRate = 10.0;
    ImGui::InputDouble("risk free rate", &riskFreeRate, 1.0f, 2.0f, "%.3f");
        static double volatility = 0.5;
    ImGui::InputDouble("volatility", &volatility, 0.1f, 1.0f, "%.3f");
        static int no_of_paths = 0;
    ImGui::SliderInt("number of paths", &no_of_paths, 1, 1000000);
    
    const char* models[] = { "vanilla mc model", "variance reduction mc model", "cache aware mc model", "black scholes model"};
    static int current_model = 0;
    if (ImGui::Combo("pick model", &current_model, models, IM_ARRAYSIZE(models))){
        simParams.modelType = static_cast<ModelType>(current_model); 
    }

    ImGui::Separator();

    ImGui::SeparatorText("Simulation Inputs");
    static int iterations = 1000.0;
    ImGui::InputInt("Iterations", &iterations, 1, 100);

    
    if (cachedIsRunning) {
        ImGui::BeginDisabled();
    }
    
    if (ImGui::Button("Benchmark Simulation", ImVec2(250, 40))) {

        simParams.spotPrice       = spotPrice;
        simParams.strikePrice     = strikePrice;
        simParams.timeToMaturity  = timeToMaturity;
        simParams.riskFreeRate    = riskFreeRate;
        simParams.volatility      = volatility;
        simParams.numPaths        = no_of_paths;
        simParams.modelType       = static_cast<ModelType>(current_model);
        simParams.iterations      = iterations;
        simHelper.StartSimulation(simParams);
        showResults = true;
        resultsWindowOpen = true;
        rdata2.Data.clear();

    }
    
    if (cachedIsRunning) { // dont really get this
        ImGui::EndDisabled();
        ImGui::SameLine();
        ImGui::Text("Running...");
    }
    
    if (cachedIsRunning && ImGui::Button("Stop Simulation", ImVec2(200, 40))) {
        simHelper.StopSimulation();
    }
    
    ImGui::End();

    if (showResults) {
        ImGui::Begin("Simulation Results", &showResults);

        if (!resultsWindowOpen) {
            showResults = false;
        }
        
        if (cachedIsRunning) {
            ImGui::SeparatorText("Simulation in Progress");
            ImGui::ProgressBar(cachedProgress, ImVec2(-1.0f, 0.0f));
            ImGui::Text("Iterations: %llu /%llu", resultsCache.iterationsCompleted,simParams.iterations);
        }
        
        if (resultsCache.isComplete || resultsCache.iterationsCompleted > 0) {
            ImGui::SeparatorText("Results");
            // ImGui::Text("Theoretical Option Value: %.6f", resultsCache.bsValue);
            ImGui::Text("Model: %s", resultsCache.modelName.c_str());
            ImGui::Text("Estimated Option Value: %.6f", resultsCache.estimatedValue);
            ImGui::SeparatorText("Model Error");
            ImGui::Text("Absolute Error of Estimated Value: %.6f", resultsCache.error);
            //ImGui::Text("Absolute Minimum Error of Estimated Value: %.6f", resultsCache.minError);
            ImGui::SeparatorText("Timings");
            ImGui::Text("Mean Time in Microseconds:: %.6f", resultsCache.meanTime);
            ImGui::Text("Min Time in Microseconds:: %.6f", resultsCache.minTime);


            static bool showPlot = true;
            ImGui::Checkbox("Show Plot", &showPlot);

            if (showPlot){
                //Add points to the buffers every 0.001 seconds
                // if(cachedIsRunning){
                
                //     if (t == 0 || t - last_t >= 0.001f) {
                //         rdata2.AddPoint(t,resultsCache.estimatedValue);
                //         last_t = t;
                //     }
                //     t += ImGui::GetIO().DeltaTime;
                // }

                //could plot error convergance 



                static int lastIteration = -1;

                if (resultsCache.iterationsCompleted != lastIteration)
                {
                    rdata2.AddPoint(
                        (float)resultsCache.iterationsCompleted,
                        (float)resultsCache.estimatedValue
                    );

 

                    lastIteration = resultsCache.iterationsCompleted;
                }
                


                

    
                double tdata1[20], tdata2[20];
                for (int i = 0; i < 20; ++i)  {
                    tdata1[i] = (double)i*((double)simParams.iterations/10.0);
                    tdata2[i] = resultsCache.bsValue;
                }

                



                if (ImPlot::BeginPlot("Theoretical Values vs Estimate Value", ImVec2(-1,400))) {
                    ImPlot::SetupAxes("Iterations", "Theoretical Value");
                    ImPlot::SetupAxisLimits(ImAxis_X1,0,simParams.iterations, ImGuiCond_Always);
                    ImPlot::SetupAxisLimits(ImAxis_Y1,resultsCache.minEstimatedValue,resultsCache.maxEstimatedValue, ImGuiCond_Always);
                    ImPlot::PlotLine("Theoretical Value", tdata1,tdata2,20);
                    ImPlot::PlotLine("Estimate Value", &rdata2.Data[0].x, &rdata2.Data[0].y, rdata2.Data.size(), 0, 0, 2 * sizeof(float));
                    ImPlot::EndPlot();
                }
        }





            
        } else {
            ImGui::Text("No results yet. Click 'Run Simulation' to start.");
        }
        
        ImGui::End();
    }





}

void UseImGui::Render(){
    ImGui::Render();

}

void UseImGui::Shutdown(){
    ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
    ImPlot::DestroyContext();

}