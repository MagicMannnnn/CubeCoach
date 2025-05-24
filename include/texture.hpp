#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>


struct Texture {
    unsigned int id;       // OpenGL texture ID (0 for color)
    std::string type;      // Type of texture (diffuse, specular, etc. or color name)
    std::string path;      // File path (for caching purposes)

    // Additional fields for storing color data
    float r, g, b, a;      // Color values (used if id is 0)

    // Constructor for a regular texture (loaded from file)
    Texture(unsigned int textureID, const std::string& textureType, const std::string& texturePath)
        : id(textureID), type(textureType), path(texturePath), r(0.0f), g(0.0f), b(0.0f), a(1.0f) {}

    // Constructor for color textures (when there's no actual image)
    Texture(const std::string& colorType, float red, float green, float blue, float alpha)
        : id(0), type(colorType), path(""), r(red), g(green), b(blue), a(alpha) {}
};