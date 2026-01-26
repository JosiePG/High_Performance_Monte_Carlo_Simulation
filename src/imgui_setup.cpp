#include "imgui_setup.h"
#include <algorithm>


void UseImGui::Init(GLFWwindow* window,const char* glsl_version){
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    
 
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
    
    if (cachedIsRunning && ImGui::Button("Stop Simulation")) {
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
            ImGui::Text("Theoretical Option Value: %.6f", resultsCache.bsValue);
            
            if (resultsCache.isComplete) {
                ImGui::SeparatorText("Execution Time Statistics");
                ImGui::Text("Mean Time: %.2f μs", resultsCache.meanTime);
                ImGui::Text("Min Time: %.2f μs", static_cast<double>(resultsCache.minTime));
                ImGui::Text("Total Runs: %llu", resultsCache.iterationsCompleted);
                
            
                static bool showHistogram = false;
                ImGui::Checkbox("Show Timing Histogram", &showHistogram);
                
                if (showHistogram && !resultsCache.timings.empty()) {
                    std::vector<float> histData;
                    for (auto t : resultsCache.timings) {
                        histData.push_back(static_cast<float>(t));
                    }
                    
                    float minVal = *std::min_element(histData.begin(), histData.end());
                    float maxVal = *std::max_element(histData.begin(), histData.end());
                    
                    ImGui::PlotHistogram("Timing Distribution", 
                        histData.data(), histData.size(), 
                        0, nullptr, minVal, maxVal, 
                        ImVec2(0, 150));
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

}