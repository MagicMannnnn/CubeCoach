#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <glm/glm.hpp>




static struct Settings {
    static int SCR_WIDTH;
    static int SCR_HEIGHT;

    static float MOVE_TIME;  // Declaration
    static glm::vec3 BG_COLOUR;  // Declaration

    static void updateSettings();
    static void resetSettings();
    static void openInNotepad();
    static void openControlsInNotepad();

private:
    struct Config {
        float moveTime;
        float width;
        float height;
        glm::vec3 bgColor;
    };

    static void applyConfigToSettings(Config config);
    static void openInNotepad(std::string& path);
};
