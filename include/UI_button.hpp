#pragma once

#include <texture.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <shader.h>

#include <iostream>

namespace UI {


    class Rect {
    public:
        Rect() = default;
        Rect(glm::vec2 pos, glm::vec2 size, std::string iconPath, Shader& shader, GLenum drawType = GL_RGBA)
            : m_pos(pos), m_size(size), m_shader(shader), m_drawType(drawType) {
            LoadTexture(iconPath);
            SetupQuad();
            m_projection = glm::ortho(0.0f, (float)Settings::SCR_WIDTH, 0.0f, (float)Settings::SCR_HEIGHT);
        }

        ~Rect() {
            glDeleteTextures(1, &textureID);
            glDeleteVertexArrays(1, &vao);
            glDeleteBuffers(1, &vbo);
        }

        void Render() {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_BLEND);
            m_shader.use();
            m_shader.setMat4("projection", m_projection);
            //glActiveTexture(0);
            glBindTexture(GL_TEXTURE_2D, textureID);           
            //m_shader.setInt("texture1", 0);
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }

    protected:
        glm::vec2 m_pos;
        glm::vec2 m_size;

    private:
        GLuint textureID;
        GLuint vao, vbo;
        Shader& m_shader;
        GLenum m_drawType;
        glm::mat4 m_projection;

        void LoadTexture(const std::string& path) {
            stbi_set_flip_vertically_on_load(true);
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            int width, height, nrChannels;
            unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
            if (data) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, m_drawType, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
            }
            else {
                std::cerr << "Failed to load texture: " << path << std::endl;
            }
            stbi_image_free(data);
        }

        void SetupQuad() {
            float vertices[] = {
                m_pos.x, m_pos.y + m_size.y, 0.0f, 1.0f,
                m_pos.x + m_size.x, m_pos.y, 1.0f, 0.0f,
                m_pos.x, m_pos.y, 0.0f, 0.0f,

                m_pos.x, m_pos.y + m_size.y, 0.0f, 1.0f,
                m_pos.x + m_size.x, m_pos.y + m_size.y, 1.0f, 1.0f,
                m_pos.x + m_size.x, m_pos.y, 1.0f, 0.0f
            };

            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);
            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        
    };

    class Button : public Rect {
    public:
        Button() = default;
        Button(glm::vec2 pos, glm::vec2 size, std::string iconPath, Shader& shader, GLenum drawType = GL_RGBA)
            : Rect(pos, size, iconPath, shader, drawType)
        {

        }

        ~Button() {
            
        }

        void update(glm::vec2 mousePos) {
            if (Global::leftClick && m_pos.x < mousePos.x && mousePos.x < m_pos.x + m_size.x && m_pos.y < mousePos.y && mousePos.y < m_pos.y + m_size.y) {
                Global::leftClick = false;
                m_clicked = !m_clicked;
            }
        }

        bool getClicked() {
            return m_clicked;
        }

        void setClicked(bool clicked=true) {
            m_clicked = clicked;
        }

    private:
        bool m_clicked = false;
    };
}