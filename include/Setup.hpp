#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <app_input.hpp>
#include <iostream>
#include <texture.hpp>


namespace Setup {

    int setupApplication() {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


        // glfw window creation
        // --------------------
        Global::window = glfwCreateWindow(Settings::SCR_WIDTH, Settings::SCR_HEIGHT, "CubeCoach", NULL, NULL);
        if (Global::window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(Global::window);
        glfwSetFramebufferSizeCallback(Global::window, Global::framebuffer_size_callback);
        glfwSetCursorPosCallback(Global::window, Global::mouse_callback);
        glfwSetScrollCallback(Global::window, Global::scroll_callback);
        glfwSetMouseButtonCallback(Global::window, Global::mouse_button_callback);
        glfwSetCharCallback(Global::window, Global::characterCallback);

        setupIcon();


        // glad: load all OpenGL function pointers
        // ---------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -1;
        }

        // configure global opengl state
        // -----------------------------
        glEnable(GL_DEPTH_TEST);
        //glEnable(GL_CULL_FACE);       // Enable culling
        //glCullFace(GL_BACK);          // Cull back faces
       // glFrontFace(GL_CCW);          // Counter-clockwise is the front face

        return 0;
    }
}
