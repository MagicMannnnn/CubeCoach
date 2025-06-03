#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Settings.hpp>
#include <camera.hpp>
#include <iostream>
//#include <textRenderer.hpp>

namespace Global {
    GLFWwindow* window;
    Camera camera(glm::vec3(0.0f, 5.f, 10.0f));
    float mouseX = Settings::SCR_WIDTH / 2.0f;
    float mouseY = Settings::SCR_HEIGHT / 2.0f;
    float mouseLastX = Settings::SCR_WIDTH / 2.0f;
    float mouseLastY = Settings::SCR_HEIGHT / 2.0f;
    float mouseDeltaX;
    float mouseDeltaY;
    bool firstMouse = true;

    bool leftClick = false;
    bool rightClick = false;
    bool firstLeftClick = true;

    // Move flags
    bool U = false, Up = false, U2 = false;
    bool D = false, Dp = false, D2 = false;
    bool L = false, Lp = false, L2 = false;
    bool R = false, Rp = false, R2 = false;
    bool F = false, Fp = false, F2 = false;
    bool B = false, Bp = false, B2 = false;
    bool M = false, Mp = false, M2 = false;
    bool UP = false, DOWN = false, LEFT = false, RIGHT = false;

    // Previous‐frame key states
    bool prevU = false, prevD = false, prevL = false,
        prevR = false, prevF = false, prevB = false, prevM = false, prevUP = false,
        prevDOWN = false, prevLEFT = false, prevRIGHT = false;

    bool solve = false;
    bool prevSolve = false;
    bool zero = false;
    bool prevZero = false;
    bool nine = false;
    bool prevNine = false;


    float deltaTime = 0.0f;

    void processInput(GLFWwindow* window)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Reset move flags each frame
        U = Up = U2 = false;
        D = Dp = D2 = false;
        L = Lp = L2 = false;
        R = Rp = R2 = false;
        F = Fp = F2 = false;
        B = Bp = B2 = false;
        M = Mp = M2 = false;
        UP = DOWN = LEFT = RIGHT = false;
        solve = false;
        zero = false;
        nine = false;

        struct MoveDef {
            int key;
            bool& prev, & single, & prime, & dbl;
        };

        struct RotateDef {
            int key;
            bool& prev, & move;
        };

        static MoveDef moves[] = {
            { GLFW_KEY_U, prevU, U, Up, U2 },
            { GLFW_KEY_D, prevD, D, Dp, D2 },
            { GLFW_KEY_L, prevL, L, Lp, L2 },
            { GLFW_KEY_R, prevR, R, Rp, R2 },
            { GLFW_KEY_F, prevF, F, Fp, F2 },
            { GLFW_KEY_B, prevB, B, Bp, B2 },
            {GLFW_KEY_M, prevM, M, Mp, M2}
        };

        static RotateDef rotations[] = {
            {GLFW_KEY_LEFT, prevLEFT, LEFT},
            {GLFW_KEY_RIGHT, prevRIGHT, RIGHT},
            {GLFW_KEY_DOWN, prevDOWN, DOWN},
            {GLFW_KEY_UP, prevUP, UP},
            {GLFW_KEY_S, prevSolve, solve},
            {GLFW_KEY_0, prevZero, zero},
            {GLFW_KEY_9, prevNine, nine}
        };

        for (auto& m : moves) {
            bool curr = (glfwGetKey(window, m.key) == GLFW_PRESS);
            // if just pressed this frame:
            if (curr && !m.prev) {
                bool prime = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
                bool dbl = (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS);
                if (dbl)   m.dbl = true;
                else if (prime) m.prime = true;
                else            m.single = true;
            }
            m.prev = curr;
        }

        for (auto& r : rotations) {
            bool curr = (glfwGetKey(window, r.key) == GLFW_PRESS);
            // if just pressed this frame:
            if (curr && !r.prev) {
                r.move = true;
            }
            r.prev = curr;
        }




    }

    // glfw: whenever the window size changed (by OS or user resize) this callback function executes
    // ---------------------------------------------------------------------------------------------
    void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        // make sure the viewport matches the new window dimensions; note that width and 
        // height will be significantly larger than specified on retina displays.
        Settings::SCR_HEIGHT = height;
        Settings::SCR_WIDTH = width;
        glViewport(0, 0, width, height);
    }


    // glfw: whenever the mouse moves, this callback is called
    // -------------------------------------------------------
    void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
    {
        mouseLastX = mouseX;
        mouseLastY = mouseY;
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);
        mouseX = xpos;
        mouseY = ypos;


        float xoffset = mouseX - mouseLastX;
        float yoffset = mouseLastY - mouseY; // reversed since y-coordinates go from bottom to top
        mouseDeltaX = xoffset;
        mouseDeltaY = yoffset;


        if (rightClick) {
            //camera.ProcessMouseMovement(xoffset, yoffset);
        }
        

    }

    // glfw: whenever the mouse scroll wheel scrolls, this callback is called
    // ----------------------------------------------------------------------
    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {

        camera.ProcessMouseScroll(static_cast<float>(yoffset), deltaTime);
    }


    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (action == GLFW_PRESS) {
                if (firstMouse) {
                    leftClick = true;
                    glfwSetInputMode(Global::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                }
                firstMouse = false;
                
            }
            if (action == GLFW_RELEASE) {
                firstMouse = true;
                leftClick = false;
                glfwSetInputMode(Global::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
        }

        if (button == GLFW_MOUSE_BUTTON_RIGHT && !leftClick) {
            if (action == GLFW_PRESS) {
                rightClick = true;
                glfwSetInputMode(Global::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            if (action == GLFW_RELEASE) {
                rightClick = false;
                glfwSetInputMode(Global::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
        }
    }
}

