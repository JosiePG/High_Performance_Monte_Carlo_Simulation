#ifndef IMGUI_SETUP_H
#define IMGUI_SETUP_H
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "implot.h"
#include "implot_internal.h"
#include "simulation_helper.h"

class UseImGui{
    public:
        void Init(GLFWwindow* window,const char* glsl_version);
        void NewFrame();
        virtual void Update(SimulationHelper & simHelper);
        void Render();
        void Shutdown();

    private:
        SimulationParams simParams;
        OptionParameters optParams;
        bool showResults = false;
        bool showPlot = true;
        bool resultsWindowOpen = true; 
        bool showCResults = false;
        bool showCPlot = true;
        bool showEstimatePlot = true;
        bool showErrorPlot = false;
        bool cResultsWindowOpen = true; 
        SimulationResults resultsCache;
        double cachedProgress = 0.0;
        bool cachedIsRunning = false; 
        SimulationData simData;
        std::deque<SimulationData> simHistory;
        bool historyAdded = false;
        int simHistoryCapacity = 20;


};

#endif