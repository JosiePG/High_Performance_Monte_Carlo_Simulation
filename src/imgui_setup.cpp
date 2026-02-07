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
    ImVec2(800.0f, 600.0f)      // max width = 600 px, unlimited height
);
    ImGui::Begin("Parameter Settings");
    ImGui::Text("(CTRL + Click) to enter input manually");
    ImGui::SeparatorText("Option Inputs");

    static double spotPrice = 100.0;
    static double strikePrice = 100.0;
    static double timeToMaturity = 1.0;
    static double riskFreeRate = 0.03;
    static double volatility = 0.2;
    static int no_of_paths = 10000;
    static int current_model = 0;
    static int iterations = 1000;
    

if (ImGui::BeginTable("OptionInputsTable", 2, ImGuiTableFlags_SizingStretchProp))
{
    ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthStretch, 0.6f);
    ImGui::TableSetupColumn("Input", ImGuiTableColumnFlags_WidthStretch, 0.6f);

    auto RowInputDouble = [](const char* label, double* value,
                            double step, double step_fast, const char* fmt)
    {
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted(label);

        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(-FLT_MIN);

        std::string id = std::string("##") + label;
        ImGui::InputDouble(id.c_str(), value, step, step_fast, fmt);
    };

    RowInputDouble("spot price", &spotPrice, 1.0, 10.0, "%.2f");
    RowInputDouble("strike price", &strikePrice, 1.0, 10.0, "%.2f");
    RowInputDouble("time to maturity (years)", &timeToMaturity, 0.05, 0.25, "%.3f");
    RowInputDouble("risk free rate", &riskFreeRate, 0.01, 0.05, "%.3f");
    RowInputDouble("volatility", &volatility, 0.01, 0.05, "%.3f");

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("number of paths");

    ImGui::TableSetColumnIndex(1);
    ImGui::SetNextItemWidth(-FLT_MIN);
    ImGui::SliderInt("##paths", &no_of_paths, 1, 1000000);

    const char* models[] =
    {
        "vanilla mc model",
        "variance reduction mc model",
        "cache aware mc model",
    };

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("model");

    ImGui::TableSetColumnIndex(1);
    ImGui::SetNextItemWidth(-FLT_MIN);

    if (ImGui::Combo("##model", &current_model, models, IM_ARRAYSIZE(models)))
    {
        simParams.modelType = static_cast<ModelType>(current_model);
    }

    ImGui::EndTable();


}

ImGui::SeparatorText("Simulation Inputs");

if (ImGui::BeginTable("Simulation Inputs Table", 2, ImGuiTableFlags_SizingStretchProp))
{
    ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthStretch, 0.6f);
    ImGui::TableSetupColumn("Input", ImGuiTableColumnFlags_WidthStretch, 0.6f);
    
    
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("iterations");
    ImGui::TableSetColumnIndex(1);
    ImGui::SetNextItemWidth(-FLT_MIN);
    ImGui::SliderInt("##iterations", &iterations, 1, 1000);
    ImGui::EndTable();

}

    




    
    if (cachedIsRunning) {
        ImGui::BeginDisabled();
    }

    ImGui::Dummy(ImVec2(0.0f, 10.0f)); // adds spacing
    
    if (ImGui::Button("Benchmark Simulation", ImVec2(250, 40))) {

        simParams.spotPrice       = spotPrice;
        simParams.strikePrice     = strikePrice;
        simParams.timeToMaturity  = timeToMaturity;
        simParams.riskFreeRate    = riskFreeRate;
        simParams.volatility      = volatility;
        simParams.numPaths        = no_of_paths;
        simParams.modelType       = static_cast<ModelType>(current_model);
        simParams.iterations      = iterations;
        simHelper.StartSimulation(simParams,false);
        showPlot = (iterations >= 50 &&no_of_paths>=10000);
        showResults = true;
        resultsWindowOpen = true;
        showCResults = false;
        cResultsWindowOpen = false;
        rdata2.Data.clear();

    }

    if (cachedIsRunning) {
        ImGui::EndDisabled();
        ImGui::SameLine();
        ImGui::Text("Running...");
    }
    
    if (cachedIsRunning && ImGui::Button("Stop Simulation", ImVec2(200, 40))) {
        simHelper.StopSimulation();
    }

    ImGui::Dummy(ImVec2(0.0f, 10.0f)); 

    ImGui::SeparatorText("Standard Error vs Number Of Paths");

    ImGui::Dummy(ImVec2(0.0f, 10.0f));

        if (ImGui::Button("Run Convergance Plot", ImVec2(250, 40))) {
        simParams.spotPrice       = 100.0;
        simParams.strikePrice     = 100.0;
        simParams.timeToMaturity  = 1.0;
        simParams.riskFreeRate    = 0.03;
        simParams.volatility      = 0.2;
        simParams.numPaths        = 0; // dummy number that will be overwritten in SimulationHelper::RunConvergancePlot
        simParams.modelType       = static_cast<ModelType>(current_model);
        simParams.iterations      = 1; 
        simHelper.StartSimulation(simParams,true);
        showCResults = true;
        cResultsWindowOpen = true;
        showResults = false;
        resultsWindowOpen = false;

    }

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
    {
    ImGui::SetTooltip("Note : default option inputs will be used instead of user input");
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
            ImGui::Text("Model: %s", resultsCache.modelName.c_str());
            ImGui::Text("Estimated Option Value: %.6f", resultsCache.estimatedValue);
            ImGui::Text("Theoretical Option Value: %.6f", resultsCache.bsValue);
            ImGui::SeparatorText("Model Error");
            ImGui::Text("Absolute Error of Estimated Value: %.6f", resultsCache.error);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
            {
            ImGui::SetTooltip("|Model Estimate Value - Theoretical Value (Black Scholes Model)|");
            }
            ImGui::Text("Standard Error of Estimated Value: %.6f", resultsCache.standardError);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
            {
            ImGui::SetTooltip("Measures statistical uncertainty of the model");
            }
            ImGui::Text("95%% confidence interval of estimated value: [%.6f, %.6f]", resultsCache.ci_lo,resultsCache.ci_hi);
            ImGui::SeparatorText("Timings");
            ImGui::Text("Mean Time in Microseconds:: %.6f", resultsCache.meanTime);
            ImGui::Text("Min Time in Microseconds:: %.6f", resultsCache.minTime);
            ImGui::BeginDisabled(!showPlot);
            ImGui::Checkbox("Show Plot", &showPlot);
            ImGui::EndDisabled();

            if (!showPlot && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
            {
            ImGui::SetTooltip("Warning : iterations to low (<50) or number of paths to low (<10000), unable to load plot");
            }
                


            if (showPlot){



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

    if (showCResults)
{
    ImGui::Begin("Convergence Results", &showCResults);
    if (showCPlot){
    if (ImPlot::BeginPlot("Standard Error Convergence", ImVec2(-1,400)))
    {
     ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Log10);
        ImPlot::SetupAxisScale(ImAxis_Y1, ImPlotScale_Linear);
        

        ImPlot::SetupAxisLimits(ImAxis_X1, 900, 1010000, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0.0, 0.5, ImGuiCond_Always);
        
        ImPlot::SetupAxes("Number of Paths (log scale)", "Standard Error");

        ImPlot::PlotLine(
            "SE vs Paths",
            resultsCache.convergencePaths.data(),
            resultsCache.convergenceSE.data(),
            resultsCache.convergencePaths.size()
        );

        ImPlot::EndPlot();
    }}
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