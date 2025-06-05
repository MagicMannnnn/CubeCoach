#pragma once

#include <GLFW/glfw3.h>
#include <stb_truetype/stb_truetype.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <Settings.hpp>
#include <shader.h>

// Helper function to flip bitmap (optional, if needed)
void FlipBitmap(unsigned char* bitmap, int width, int height) {
    for (int y = 0; y < height / 2; ++y) {
        int oppositeY = height - y - 1;
        for (int x = 0; x < width; ++x) {
            std::swap(bitmap[y * width + x], bitmap[oppositeY * width + x]);
        }
    }
}

class TextRenderer {
public:
    // Constructor with font path, size, and shader paths
    TextRenderer(std::string fontPath, int fontSize, std::string vertexShaderPath, std::string fragmentShaderPath)
        : fontSize(fontSize), shader(vertexShaderPath.c_str(), fragmentShaderPath.c_str()) {

        // Load the font and create texture atlas
        LoadFont(fontPath);
        CreateTextureAtlas();

        // Generate VAO and VBO for rendering quads
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // Set up projection matrix
        shader.use();
        glm::mat4 projection = glm::ortho(0.0f, (float)Settings::SCR_WIDTH, 0.0f, (float)Settings::SCR_HEIGHT);
        shader.setMat4("projection", projection);
    }

    // Destructor to clean up OpenGL resources
    ~TextRenderer() {
        glDeleteTextures(1, &textureID);
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
    }

    // Render the given text at specified (x, y) position with scale and color
    void RenderText(const std::string& text, float x, float y, float scale, const glm::vec3& color) {
        shader.use();
        shader.setVec3("textColor", color);
        shader.setInt("text", 0);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glBindVertexArray(vao);

        float origX = x;
        float baseY = y;
        float lineHeight = 24.0f * scale;  // Adjust depending on your font size

        for (char c : text) {
            if (c == '\n') {
                x = origX;
                y = baseY - lineHeight;
                baseY = y;
                continue;
            }

            stbtt_aligned_quad q;
            stbtt_GetBakedQuad(cdata, 512, 512, c - 32, &x, &y, &q, 1);

            // Fix vertical alignment (especially for apostrophes)
            float finalY = y;
            if (c == '\'') finalY = y + 15 * scale;

            float vertices[6][4] = {
                { q.x0, finalY + (q.y1 - q.y0), q.s0, q.t0 },
                { q.x1, finalY,                 q.s1, q.t1 },
                { q.x0, finalY,                 q.s0, q.t1 },

                { q.x0, finalY + (q.y1 - q.y0), q.s0, q.t0 },
                { q.x1, finalY + (q.y1 - q.y0), q.s1, q.t0 },
                { q.x1, finalY,                 q.s1, q.t1 }
            };

            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_BLEND);
    }


private:
    GLuint textureID;
    int fontSize;
    stbtt_bakedchar cdata[96]; // ASCII 32..126
    GLuint vao, vbo;
    Shader shader;

    // Load the font file and generate the baked font texture atlas
    void LoadFont(const std::string& fontPath) {
        // Open the font file
        FILE* fontFile = fopen(fontPath.c_str(), "rb");
        if (!fontFile) {
            std::cerr << "Failed to load font file: " << fontPath << std::endl;
            return;
        }

        // Read the font file into memory
        fseek(fontFile, 0, SEEK_END);
        long size = ftell(fontFile);
        fseek(fontFile, 0, SEEK_SET);

        std::vector<unsigned char> fontBuffer(size);
        fread(fontBuffer.data(), 1, size, fontFile);
        fclose(fontFile);

        // Create a bitmap for the baked font texture (512x512)
        unsigned char* bitmap = new unsigned char[512 * 512];
        stbtt_BakeFontBitmap(fontBuffer.data(), 0, fontSize, bitmap, 512, 512, 32, 96, cdata);

        // Generate the texture for the baked font bitmap
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Free the bitmap memory
        delete[] bitmap;
    }

    // Create texture atlas (handled in LoadFont)
    void CreateTextureAtlas() {
        // No additional atlas creation needed, as it's done in LoadFont
    }
};
