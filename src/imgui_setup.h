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
        bool showResults = false;
        bool showPlot = true;
        bool resultsWindowOpen = true; 
        bool showCResults = false;
        bool showCPlot = true;
        bool cResultsWindowOpen = true; 
        SimulationResults resultsCache;
        double cachedProgress = 0.0;
        bool cachedIsRunning = false; 
        struct SimulationData{
        std::string model;
        double std_error;
        double mean_time;
        double min_time;
        };

        std::deque<SimulationData> simHistory;
        bool historyAdded = false;


};

#endif