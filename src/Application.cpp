#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <GL/glew.h>

#include "Header.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_glfw.h"
#include "ImGUI/imgui_impl_opengl3.h"

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include "Shader.h"

#include "Scenes/Scene.h"

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT)
    {
        /*glfwGetCursorPos(window, &HEADER::MOUSE_INIT_X, &HEADER::MOUSE_INIT_Y);
        HEADER::MOUSE_X = HEADER::MOUSE_INIT_X;
        HEADER::MOUSE_Y = HEADER::MOUSE_INIT_Y;*/
        HEADER::IS_LEFT_MOUSE_DOWN = true;
        HEADER::IS_RIGHT_MOUSE_DOWN = false;
    }
    else if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        glfwGetCursorPos(window, &HEADER::MOUSE_INIT_X, &HEADER::MOUSE_INIT_Y);
        HEADER::MOUSE_X = HEADER::MOUSE_INIT_X;
        HEADER::MOUSE_Y = HEADER::MOUSE_INIT_Y;
        HEADER::IS_RIGHT_MOUSE_DOWN = true;
        HEADER::IS_LEFT_MOUSE_DOWN = false;
    }
    else
    {
        HEADER::IS_RIGHT_MOUSE_DOWN = false;
        HEADER::IS_LEFT_MOUSE_DOWN = false;
    }
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    //CORE PROFILE
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */

    GLFWmonitor* MONITOR = glfwGetPrimaryMonitor();
    const GLFWvidmode* MODE = glfwGetVideoMode(MONITOR);
    HEADER::WINDOW_HEIGHT = MODE->height;
    HEADER::WINDOW_WIDTH = MODE->width;

    window = glfwCreateWindow(HEADER::WINDOW_WIDTH, HEADER::WINDOW_WIDTH, "Bezier surfaces", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    //init glew
    if (glewInit() != GLEW_OK)
    {
        std::cout << "glew init error" << std::endl;
        return -1;
    }

    {
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        Scenes::Scene mScene;
        bool renderScene = true;

        Renderer renderer;

        //imgui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui::StyleColorsLight();
        ImGui_ImplOpenGL3_Init((char*)glGetString(330));

        bool show_demo_window = false;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
            renderer.Clear();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            
            if (renderScene)
            {
                glfwSetMouseButtonCallback(window, mouse_button_callback);
                if (HEADER::IS_RIGHT_MOUSE_DOWN)
                {
                    HEADER::MOUSE_PREV_X = HEADER::MOUSE_X;
                    HEADER::MOUSE_PREV_Y = HEADER::MOUSE_Y;
                    glfwGetCursorPos(window, &HEADER::MOUSE_X, &HEADER::MOUSE_Y);
                }

                //Configure imgui design in here
                ImGuiStyle* style = &ImGui::GetStyle();
                ImVec4* colors = style->Colors;
                colors[ImGuiCol_Border] = ImVec4(0.51f, 0.36f, 0.15f, 0.00f);
                colors[ImGuiCol_MenuBarBg] = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
                colors[ImGuiCol_PopupBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);

                style->WindowRounding = 4;
                style->FrameRounding = 4;
                style->PopupRounding = 4;
                style->ScrollbarRounding = 6;
                style->GrabRounding = 4;
                style->TabRounding = 4;
                style->ItemSpacing = ImVec2(10, 10);

                //Scene stuff in here
                mScene.OnUpdate(0.0f);
                mScene.OnRender();
                ImGui::Begin("LOL");
                mScene.OnImGuiRender();
                ImGui::End();
            }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    glfwTerminate();
    return 0;
}
