#include "gl/glew.h"
#include "glm/glm.hpp"

#include "GLFW/glfw3.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"
#include <chrono>
#include <filesystem>
#include <iostream>
#include <queue>

import Component;
import Ecs;
import System;
import Graphics;

#ifdef _WIN32
extern "C" { // Use High Performance GPU
__declspec(dllexport) bool AmdPowerXpressRequestHighPerformance = true;
__declspec(dllexport) bool NvOptimusEnablement = true;
}
#endif

void error_callback(int error, const char* description) { std::cerr << "Error: " << description << std::endl; }

int main()
{
    {
        glfwSetErrorCallback(error_callback);

        // Start GLFW Library
        if (!glfwInit())
            return -1;

        char const* glslVersion{"#version 330"};
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Create window and ensure creation
        GLFWwindow* window{glfwCreateWindow(640, 480, "GameEngine", NULL, NULL)};

        if (!window)
        {
            glfwTerminate();
            std::cerr << "Window not made!" << std::endl;
            return -1;
        }
        // Make window current
        glfwMakeContextCurrent(window);
        glfwSwapInterval(0);

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if (glfwRawMouseMotionSupported())
            glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

        // Test GLEW is working and output GL Version
        if (glewInit() != GLEW_OK)
        {
            std::cerr << "Could not start GLEW" << std::endl;
            return -1;
        }
        std::cout << glGetString(GL_VERSION) << std::endl;

        // Set clear color
        glClearColor(0.5f, 1.0f, 1.0f, 1.0f);

        // Wireframe
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // Cull back faces
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        // Enable depth testing
        glEnable(GL_DEPTH_TEST);

        // Setup debug UI
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io{ImGui::GetIO()};
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glslVersion);

        std::shared_ptr<Ecs::EntityManager> manager{std::make_shared<Ecs::EntityManager>()};
        Ecs::SystemManager sysManager(manager);
        Component::registerComponents(*manager);
        System::registerSystems(sysManager);
        Graphics::ResourceManager resourceManager;

        Ecs::Entity player{manager->createEntity()};
        manager->addComponent<Component::CameraComponent>(player, {{}});
        manager->addComponent<Component::TransformComponent>(player, {{0.0f, 4.0f, 0.0f}});
        manager->addComponent<Component::InputComponent>(player, {window});
        manager->addComponent<Component::MovementComponent>(player, {true, true, 5.0F});
        manager->addComponent<Component::PlayerComponent>(player, {});
        manager->addComponent<Component::BoxCollision>(player, {1.0f, 2.0f, 1.0f});

        Ecs::Entity ambientLight{manager->createEntity()};
        manager->addComponent<Component::AmbientLightComponent>(ambientLight, {});
        Ecs::Entity diffuseLight{manager->createEntity()};
        manager->addComponent<Component::PointLightComponent>(diffuseLight, {});
        manager->addComponent<Component::TransformComponent>(diffuseLight, {{5.0, 10.0, 0.0}});
        Ecs::Entity model{manager->createEntity()};
        manager->addComponent<Component::ModelComponent>(
            model, {resourceManager.getResourceId<Graphics::Model>(std::filesystem::path("assets/Pencil.gltf")),
                    &resourceManager});
        manager->addComponent<Component::TransformComponent>(model, {});

        std::chrono::time_point lastTime{std::chrono::steady_clock::now()};

        // Queue to average frame times
        std::queue<float> frameTimes;
        constexpr int frameTimesCount{400};
        float frameTimeSum{};

        while (!glfwWindowShouldClose(window))
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            int winWidth, winHeight;
            glfwGetFramebufferSize(window, &winWidth, &winHeight);
            glViewport(0, 0, winWidth, winHeight);

            // Start new imgui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            std::chrono::time_point updateTime{std::chrono::steady_clock::now()};
            float deltaTime_s{
                static_cast<float>(
                    std::chrono::duration_cast<std::chrono::microseconds>(updateTime - lastTime).count()) /
                1000000.0F};
            sysManager.updateSystems(deltaTime_s);
            lastTime = updateTime;

            {
                // Get and display the average FPS
                frameTimeSum += deltaTime_s;
                if (frameTimes.size() == frameTimesCount)
                {
                    frameTimeSum -= frameTimes.front();
                    frameTimes.pop();
                }
                frameTimes.push(deltaTime_s);

                ImGui::Begin("Stats:");
                ImGui::Text(std::string("FPS: " + std::to_string(1.0F / (frameTimeSum / frameTimes.size()))).c_str());
                ImGui::End();
            }

            // Render imgui frame
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
        }

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    glfwTerminate();

    return 0;
}