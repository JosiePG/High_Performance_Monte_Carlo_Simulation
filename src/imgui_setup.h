#ifndef IMGUI_SETUP_H
#define IMGUI_SETUP_H
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "implot.h"
#include "implot_internal.h"
#include "simulation_helper.h"

// Helper class for Imgui and Implot logic
class UseImGui{
    public:
        void Init(GLFWwindow* window,const char* glsl_version); 
        void NewFrame();
        virtual void Update(SimulationHelper & simHelper); // updates the UI with simulation inputs and plot values
        void Render(); // draws the ImGui data
        void Shutdown();

    private:
        SimulationParams simParams; //  Simulation inputs chosed by the user
        OptionParameters optParams; // Option pricing inputs chosen by the user

        // Boolean values to control if a window should be open
        bool showResults = false;
        bool showPlot = true;
        bool resultsWindowOpen = true; 
        bool showCResults = false;
        bool showCPlot = true;
        bool showEstimatePlot = true;
        bool showErrorPlot = false;
        bool cResultsWindowOpen = true; 
        SimulationResults resultsCache; // Captures the final state of the simulation
        double cachedProgress = 0.0; // Cached simulation progress for the UI.
        bool cachedIsRunning = false; 
        SimulationData simData; // Simulation data for plots
        std::deque<SimulationData> simHistory; // History of recent simulation runs.
        bool historyAdded = false; // Tracks whether the current run has already been added to history.
        int simHistoryCapacity = 20; // Maximum number of history entries
        int lastIteration = -1; // Used to detect when a new simulation iteration has completed.


};

#endif