#include <Settings.hpp>

// Define the static variables
float Settings::MOVE_TIME = 0.1f;  // Default value for MOVE_TIME
glm::vec3 Settings::BG_COLOUR = glm::vec3(50.0f, 50.0f, 50.0f);  // Default value for BG_COLOUR
int Settings::SCR_WIDTH = 1920;
int Settings::SCR_HEIGHT = 1080;

void Settings::updateSettings() {
    std::ifstream file(RESOURCES_PATH"Settings.txt");
    Config config = { 0.1f, 1920, 1080, {50, 50, 50} }; // Default values

    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        applyConfigToSettings(config);
    }

    std::string line;
    while (std::getline(file, line)) {
        line.erase(remove(line.begin(), line.end(), ' '), line.end());
        size_t commentPos = line.find("***");
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos); // Strip out comments
        }

        // Process MOVETIME line
        if (line.find("MOVETIME=") == 0) {
            std::string value = line.substr(9);  // Get the part after 'MOVETIME='
            std::istringstream(value) >> config.moveTime;
        }
        // Process BG_COLOUR line
        else if (line.find("BG_COLOUR=") == 0) {
            std::string value = line.substr(10); // Get the part after 'BG_COLOUR='
            value = value.substr(1, value.size() - 2); // Remove parentheses
            std::istringstream ss(value);
            int r, g, b;
            char comma;
            ss >> r >> comma >> g >> comma >> b;
            config.bgColor = { r, g, b };
        }
        else if (line.find("WIDTH=") == 0) {
            std::string value = line.substr(6);  // Get the part after 'MOVETIME='
            std::istringstream(value) >> config.width;
        }
        else if (line.find("HEIGHT=") == 0) {
            std::string value = line.substr(6);  // Get the part after 'MOVETIME='
            std::istringstream(value) >> config.height;
        }
    }

    file.close();
    applyConfigToSettings(config);
}

void Settings::resetSettings() {
    std::ofstream file(RESOURCES_PATH"Settings.txt");

    if (!file.is_open()) {
        std::cerr << "Error opening file for writing!" << std::endl;
        return;
    }

    // Write content to the file
    file << "***time in seconds for each move, set to zero for instant moves***" << std::endl;
    file << "MOVETIME=0.1" << std::endl;
    file << std::endl; // Blank line between sections

    file << "***background colour in RGB***" << std::endl;
    file << "BG_COLOUR = (50, 50, 50)" << std::endl;

    /*
    file << "***Screen Dimensions - NOTE will only update on app restart***" << std::endl;
    file << "WIDTH=1920" << std::endl;
    file << "HEIGHT=1080" << std::endl;
    */
    // Close the file after writing
    file.close();
    std::cout << "Config file created successfully: " << std::endl;
}

void Settings::openInNotepad() {
    // Construct the command to open the file in Notepad
    std::string path = RESOURCES_PATH"Settings.txt";
    openInNotepad(path);
}

void Settings::openControlsInNotepad() {
    // Construct the command to open the file in Notepad
    std::string path = RESOURCES_PATH"Controls.txt";
    openInNotepad(path);
}

void Settings::openInNotepad(std::string& path) {
    std::string command = "start notepad " + path;

    // Execute the command using system()
    int result = system(command.c_str());

    if (result == 0) {
        std::cout << "Notepad opened successfully!" << std::endl;
    }
    else {
        std::cerr << "Failed to open Notepad." << std::endl;
    }
}

void Settings::applyConfigToSettings(Config config) {
    MOVE_TIME = config.moveTime;
    BG_COLOUR = config.bgColor;
    SCR_HEIGHT = config.height;
    SCR_WIDTH = config.width;
}
