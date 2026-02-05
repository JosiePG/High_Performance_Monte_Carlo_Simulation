#include "imgui_setup.h"
#include <algorithm>
#include <filesystem>
#include <iostream>




void UseImGui::Init(GLFWwindow* window,const char* glsl_version){
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    
    io.Fonts->AddFontFromFileTTF(
        
    "fonts/SplineSansMono[wght].ttf",
    22.0f   // font size
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

    resultsCache = simHelper.GetResults();
    cachedProgress = simHelper.GetProgress();
    cachedIsRunning = simHelper.IsRunning();

    ImGui::SetNextWindowSizeConstraints(
    ImVec2(250.0f, 250.0f),          // min size
    ImVec2(600.0f, FLT_MAX)      // max width = 600 px, unlimited height
);
    ImGui::Begin("Parameter Settings");
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
    ImGui::SliderInt("number of paths", &no_of_paths, 1, 100000);
    
    const char* models[] = { "vanilla mc model", "variance reduction mc model", "cache aware mc model", "black scholes model"};
    static int current_model = 0;
    if (ImGui::Combo("pick model", &current_model, models, IM_ARRAYSIZE(models))){
        simParams.modelType = static_cast<ModelType>(current_model); 
    }

    ImGui::Separator();

    
    if (cachedIsRunning) {
        ImGui::BeginDisabled();
    }
    
    if (ImGui::Button("Run Simulation", ImVec2(200, 40))) {
        simParams.spotPrice       = spotPrice;
        simParams.strikePrice     = strikePrice;
        simParams.timeToMaturity  = timeToMaturity;
        simParams.riskFreeRate    = riskFreeRate;
        simParams.volatility      = volatility;
        simParams.numPaths        = no_of_paths;
        simParams.modelType       = static_cast<ModelType>(current_model);
        simHelper.StartSimulation(simParams);
        showResults = true;
        resultsWindowOpen = true;
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
            ImGui::Text("Iterations: %llu / 1000", resultsCache.iterationsCompleted);
        }
        
        if (resultsCache.isComplete || resultsCache.iterationsCompleted > 0) {
            ImGui::SeparatorText("Results");
            ImGui::Text("Model: %s", resultsCache.modelName.c_str());
            ImGui::Text("Estimated Option Value: %.6f", resultsCache.estimatedValue);
            ImGui::Text("Error of Estimated Value: %.6f", resultsCache.error);
            ImGui::Text("Theoretical Option Value: %.6f", resultsCache.bsValue);
            ImGui::SeparatorText("Timings");
            ImGui::Text("Mean Time in Microseconds:: %.6f", resultsCache.meanTime);
            ImGui::Text("Min Time in Microseconds:: %.6f", resultsCache.minTime);

            //TODO : need to find a way to reset the plot data 

            static bool showPlot = true;
            ImGui::Checkbox("Show Plot", &showPlot);

            if (showPlot){
            struct RollingBuffer {
            float Span;
            ImVector<ImVec2> Data;
            RollingBuffer() {
                Span = 10.0f;
                Data.reserve(2000);
            }
            void AddPoint(float x, float y) {
                float xmod = fmodf(x, Span);
                if (!Data.empty() && xmod < Data.back().x)
                    Data.shrink(0);
                Data.push_back(ImVec2(xmod, y));
            }
        };

            static RollingBuffer   rdata2;

             //rdata2.AddPoint( resultsCache.iterationsCompleted,resultsCache.estimatedValue); doesnt work not continous

        //Add points to the buffers every 0.02 seconds
        if(cachedIsRunning){
        static float t = 0, last_t = 0.0f;
            if (t == 0 || t - last_t >= 0.001f) {
                rdata2.AddPoint(t,resultsCache.estimatedValue);
                last_t = t;
            }
            t += ImGui::GetIO().DeltaTime;}
        

            static double tdata1[20], tdata2[20];
            for (int i = 0; i < 20; ++i)  {
                tdata1[i] = i;
                tdata2[i] = resultsCache.bsValue;
            }


            static ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels;

            if (ImPlot::BeginPlot("Theoretical Values vs Estimate Value", ImVec2(-1,400))) {
                ImPlot::SetupAxes("Time (ms)", "Theoretical Value");
                ImPlot::SetupAxisLimits(ImAxis_X1,0,3, ImGuiCond_Always);
                ImPlot::SetupAxisLimits(ImAxis_Y1,resultsCache.bsValue-0.5,resultsCache.bsValue+0.5);
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