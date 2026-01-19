#include "imgui_setup.h"

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

void UseImGui::Update(){
    ImGui::SetNextWindowSizeConstraints(
    ImVec2(250.0f, 250.0f),          // min size (no minimum)
    ImVec2(600.0f, FLT_MAX)      // max width = 600 px, unlimited height
);
    ImGui::Begin("Parameter Settings");
    ImGui::SeparatorText("Option Inputs");
        static double spotPrice = 50.0;
    ImGui::InputDouble("Enter spot price", &spotPrice, 1.0f, 2.0f, "%.3f");
        static double strikePrice = 100.0;
    ImGui::InputDouble("Enter strike price", &strikePrice, 1.0f, 2.0f, "%.3f");
        static double timeToMaturity = 2.0;
    ImGui::InputDouble("Enter time to maturity", &timeToMaturity, 1.0f, 2.0f, "%.3f");
        static double riskFreeRate = 10.0;
    ImGui::InputDouble("Enter risk free rate", &riskFreeRate, 1.0f, 2.0f, "%.3f");
        static double volatility = 0.5;
    ImGui::InputDouble("Enter Spot Price", &volatility, 0.1f, 1.0f, "%.3f");
        static int no_of_paths = 0;
    ImGui::SliderInt("Enter number of simulations", &no_of_paths, 1, 100000);
    
    const char* items[] = { "vanilla mc model", "variance reduction mc model", "cache aware mc model", "black scholes model"};
    static int item_current = 0;
    ImGui::Combo("pick model", &item_current, items, IM_ARRAYSIZE(items));
    
    static int clicked = 0;
        if (ImGui::Button("Run Simulation"))
            clicked++;
        if (clicked & 1)
        {
            ImGui::Begin("Simulation Results");
            ImGui::SeparatorText("Execution time statistics");
            ImGui::End();

        }
    ImGui::End();



}

void UseImGui::Render(){
    ImGui::Render();

}

void UseImGui::Shutdown(){
    ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

}