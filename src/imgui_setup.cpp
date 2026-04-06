#include "imgui_setup.h"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <deque>



void UseImGui::Init(GLFWwindow* window,const char* glsl_version){
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    
    io.Fonts->AddFontFromFileTTF(
    "fonts/SplineSansMono[wght].ttf",
    22.0f   
);
    
 
    ImGui_ImplGlfw_InitForOpenGL(window,true); // creates the main window
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsClassic();

}

void UseImGui::NewFrame(){
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
    

}

void UseImGui::Update(SimulationHelper & simHelper){

// stores the real time plot data
    struct RollingBuffer {
        ImVector<ImVec2> Data;
        RollingBuffer() {
            Data.reserve(2000);
        }
        void AddPoint(float x, float y) {
            Data.push_back(ImVec2(x, y));
        }
    };

    static RollingBuffer   rdata;
    static RollingBuffer   edata;

    resultsCache = simHelper.GetResults();
    cachedProgress = simHelper.GetProgress();
    cachedIsRunning = simHelper.IsRunning();

    ImGui::SetNextWindowSizeConstraints(
    ImVec2(300.0f, 500.0f),          // min size
    ImVec2(800.0f, 600.0f)      // max size
);

    ImGui::Begin("Parameter Settings");
    ImGui::Text("(CTRL + Click) to enter input manually");
    ImGui::SeparatorText("Option Inputs");

    static float spotPrice = 100.0;
    static float strikePrice = 100.0;
    static float timeToMaturity = 1.0;
    static float riskFreeRate = 0.03;
    static float volatility = 0.2;
    static int no_of_paths = 10000;
    static int optionType = 0;  // 0 = call, 1 = put
    static int current_model = 0;
    static int iterations = 1000;
    

if (ImGui::BeginTable("OptionInputsTable", 2, ImGuiTableFlags_SizingStretchProp))
{
    ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthStretch, 0.6f);
    ImGui::TableSetupColumn("Input", ImGuiTableColumnFlags_WidthStretch, 0.6f);

    auto RowDragFloat = [](const char* label, float* value,
                       float speed,
                       float minVal,
                       float maxVal,
                       const char* fmt)
{
    ImGui::TableNextRow();

    ImGui::TableSetColumnIndex(0);
    ImGui::TextUnformatted(label);

    ImGui::TableSetColumnIndex(1);
    ImGui::SetNextItemWidth(-FLT_MIN);

    std::string id = std::string("##") + label;

    ImGui::DragFloat(id.c_str(),value,speed,minVal,maxVal,fmt,ImGuiSliderFlags_AlwaysClamp); // clamp ensures user cant go below min or max values
};

    RowDragFloat("spot price", &spotPrice, 1.0f, 0.0f, FLT_MAX, "%.2f");
    RowDragFloat("strike price", &strikePrice, 1.0f, 0.0f, FLT_MAX, "%.2f");
    RowDragFloat("time to maturity (years)", &timeToMaturity, 0.05f, 0.0f, FLT_MAX, "%.3f");
    RowDragFloat("volatility", &volatility, 0.01f, 0.0f, 3.0f, "%.3f");
    RowDragFloat("risk free rate", &riskFreeRate, 0.01f, -1.0f, 1.0f, "%.3f");

   

    const char* optionTypes[] = { "Call", "Put" };
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("option type");
    ImGui::TableSetColumnIndex(1);
    ImGui::SetNextItemWidth(-FLT_MIN);
    ImGui::Combo("##optiontype", &optionType, optionTypes, IM_ARRAYSIZE(optionTypes));

    


    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("number of paths");

    ImGui::TableSetColumnIndex(1);
    ImGui::SetNextItemWidth(-FLT_MIN);
    ImGui::SliderInt("##paths", &no_of_paths, 30, 1000000,"%d",ImGuiSliderFlags_AlwaysClamp); // minimum is 30 to meet central limit theorm requirements 

    const char* models[] =
    {
        "vanilla",
        "variance reduction",
        "cache-aware + SIMD",
        "OpenMP parallel",
        "OpenMP + variance reduction"
    };

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("monte carlo engine");

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
    ImGui::SliderInt("##iterations", &iterations, 1, 1000,"%d",ImGuiSliderFlags_AlwaysClamp);
    ImGui::EndTable();

}

    
    if (cachedIsRunning) {
        ImGui::BeginDisabled();
    }

    ImGui::Dummy(ImVec2(0.0f, 10.0f)); // adds spacing
    
    if (ImGui::Button("Benchmark Simulation", ImVec2(250, 40))) {

        optParams.spotPrice       = spotPrice;
        optParams.strikePrice     = strikePrice;
        optParams.timeToMaturity  = timeToMaturity;
        optParams.riskFreeRate    = riskFreeRate;
        optParams.volatility      = volatility;
        optParams.optionType      = static_cast<OptionType>(optionType);
        simParams.numPaths        = no_of_paths;
        simParams.modelType       = static_cast<ModelType>(current_model);
        simParams.iterations      = iterations;

        simHelper.StartSimulation(optParams,simParams,false);
        showPlot = (iterations >= 50 &&no_of_paths>=10000);
        showResults = true;
        resultsWindowOpen = true;
        showCResults = false;
        cResultsWindowOpen = false;
        showEstimatePlot = true;
        showErrorPlot = false;

        historyAdded = false;
        rdata.Data.clear();
        edata.Data.clear();

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
        optParams.spotPrice       = 100.0;
        optParams.strikePrice     = 100.0;
        optParams.timeToMaturity  = 1.0;
        optParams.riskFreeRate    = 0.03;
        optParams.volatility      = 0.2;
        simParams.numPaths        = 0; // dummy number that will be overwritten in SimulationHelper::RunConvergancePlot
        simParams.modelType       = static_cast<ModelType>(current_model);
        simParams.iterations      = 1; 
        simHelper.StartSimulation(optParams,simParams,true);
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


    ImGui::SetNextWindowSizeConstraints(ImVec2(1000.0f, 800.0f),ImVec2(1200.0f, 900.0f));


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

            ImGui::BeginDisabled((iterations < 50 || no_of_paths<10000));
            ImGui::Checkbox("Show Plot", &showPlot);
            ImGui::EndDisabled();

            if(!simHelper.IsRunning() && !historyAdded){
                simData.model = resultsCache.modelName;
                simData.optionType     = (optParams.optionType == OptionType::CALL) ? "Call" : "Put";
                simData.numPaths       = simParams.numPaths;        
                simData.iterations     = simParams.iterations;     
                simData.std_error = resultsCache.standardError;
                simData.mean_time = resultsCache.meanTime;

                if(simHistory.size() <simHistoryCapacity){
                    simHistory.push_front(simData);
                }else{
                    simHistory.pop_back();
                    simHistory.push_front(simData);
                }
                historyAdded = true;
            }

            if ((iterations < 50 || no_of_paths<10000) && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
            {
            ImGui::SetTooltip("Warning : iterations to low (<50) or number of paths to low (<10000), unable to load plot");
            }

            if (showPlot){

                if (resultsCache.iterationsCompleted != lastIteration&& resultsCache.iterationsCompleted > 0)
                {
                    rdata.AddPoint(
                        (float)resultsCache.iterationsCompleted,
                        (float)resultsCache.estimatedValue
                    );

                    edata.AddPoint(
                        (float)resultsCache.iterationsCompleted,
                        (float)resultsCache.error
                    );


                    lastIteration = resultsCache.iterationsCompleted;
                }

                float ymin = FLT_MAX;
                float ymax = -FLT_MAX;

                for (auto& p : rdata.Data) {
                    ymin = std::min(ymin, p.y);
                    ymax = std::max(ymax, p.y);
                }

                float ymin_e = FLT_MAX;
                float ymax_e = -FLT_MAX;

                for (auto& p : edata.Data) {
                    ymin_e = std::min(ymin_e, p.y);
                    ymax_e = std::max(ymax_e, p.y);
                }
                
    
                double tdata1[20], tdata2[20];
                for (int i = 0; i < 20; ++i)  {
                    tdata1[i] = (double)i*((double)simParams.iterations/10.0);
                    tdata2[i] = resultsCache.bsValue;
                }

                ImGui::SeparatorText("Plot Controls");

                ImGui::Checkbox("Show Estimate vs Theoretical", &showEstimatePlot);
                ImGui::Checkbox("Show Absolute Error", &showErrorPlot);


                ImGui::SeparatorText("Simulation Plots");
                


                if (showEstimatePlot) {
                if (ImPlot::BeginPlot("Theoretical Values vs Estimate Value", ImVec2(-1,400))) {
                    ImPlot::SetupAxes("Benchmark Iterations", "Theoretical Value");
                    ImPlot::SetupAxisLimits(ImAxis_X1,0,simParams.iterations, ImGuiCond_Always);
                    ImPlot::SetupAxisLimits(ImAxis_Y1,ymin,ymax ,ImGuiCond_Always);
                    ImPlot::PlotLine("Theoretical Value", tdata1,tdata2,20);
                    ImPlot::SetNextFillStyle(ImVec4(0.2f,0.5f,1.0f,1.0f),0.25f);
                    if (rdata.Data.size() > 0){
                    ImPlot::PlotLine("Estimate Value", &rdata.Data[0].x, &rdata.Data[0].y, rdata.Data.size(), 0, 0, 2 * sizeof(float));
                    }
                    ImPlot::EndPlot();
                }}

                ImGui::Dummy(ImVec2(0,10));

                if (showErrorPlot) {
                if (ImPlot::BeginPlot("Absolute Error of Estimate Value", ImVec2(-1,400))) {
                    ImPlot::SetupAxes("Benchmark Iterations", "Absolute Error");
                    ImPlot::SetupAxisLimits(ImAxis_X1,0,simParams.iterations, ImGuiCond_Always);
                    ImPlot::SetupAxisLimits(ImAxis_Y1,ymin_e,ymax_e ,ImGuiCond_Always);
                    if (rdata.Data.size() > 0){
                    ImPlot::PlotLine("Absolute Error", &edata.Data[0].x, &edata.Data[0].y, edata.Data.size(), 0, 0, 2 * sizeof(float));
                    }
                    ImPlot::EndPlot();
                }}
        }


            
        } else {
            ImGui::Text("No results yet. Click 'Run Simulation' to start.");
        }

    if (ImGui::CollapsingHeader("Simulation History"))
            {
                if (ImGui::BeginChild("HistoryChild", ImVec2(0, 200), true))
                {
                    if (ImGui::BeginTable("HistoryTable", 6, 
                        ImGuiTableFlags_Borders | 
                        ImGuiTableFlags_ScrollY |
                        ImGuiTableFlags_RowBg))
                    {

                        ImGui::TableSetupColumn("Model", ImGuiTableColumnFlags_WidthStretch, 3.0f);
                        ImGui::TableSetupColumn("Type",      ImGuiTableColumnFlags_WidthFixed, 50.0f);
                        ImGui::TableSetupColumn("Paths",     ImGuiTableColumnFlags_WidthFixed, 80.0f);
                        ImGui::TableSetupColumn("Iters",     ImGuiTableColumnFlags_WidthFixed, 60.0f);
                        ImGui::TableSetupColumn("Std Error", ImGuiTableColumnFlags_WidthStretch, 1.0f);
                        ImGui::TableSetupColumn("Mean Time (ms)", ImGuiTableColumnFlags_WidthStretch, 1.0f);
                        ImGui::TableHeadersRow();

                        for (auto& entry : simHistory)
                        {
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn(); ImGui::Text("%s",   entry.model.c_str());
                            ImGui::TableNextColumn(); ImGui::Text("%s",   entry.optionType.c_str());
                            ImGui::TableNextColumn(); ImGui::Text("%d",   entry.numPaths);
                            ImGui::TableNextColumn(); ImGui::Text("%d",   entry.iterations);
                            ImGui::TableNextColumn(); ImGui::Text("%.4f", entry.std_error);
                            ImGui::TableNextColumn(); ImGui::Text("%.2f", entry.mean_time);
                        }
                        ImGui::EndTable();
                    }
                }
                ImGui::EndChild();
            }

        ImGui::End(); 
        
    }

    

            

    ImGui::SetNextWindowSizeConstraints(ImVec2(1000.0f, 400.0f),ImVec2(1200.0f, 600.0f));

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