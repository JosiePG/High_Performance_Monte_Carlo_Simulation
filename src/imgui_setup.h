#ifndef FINAL_YEAR_PROJECT_IMGUI_SETUP_H
#define FINAL_YEAR_PROJECT_IMGUI_SETUP_H
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
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
        bool resultsWindowOpen = true; 
        SimulationResults resultsCache;
        double cachedProgress = 0.0;
        bool cachedIsRunning = false; // do we need this?


};

#endif