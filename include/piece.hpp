#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class Piece {
public:
    glm::ivec3 gridPosition;
    glm::vec3 offset;

    // Default constructor (deleted to enforce parameters)
    Piece() = delete;

    // Main constructor: colors + offset + grid position
    Piece(const glm::vec3& topColor,
        const glm::vec3& leftColor,
        const glm::vec3& backColor,
        const glm::vec3& rightColor,
        const glm::vec3& frontColor,
        const glm::vec3& downColor,
        const glm::vec3& _offset,
        glm::ivec3 _gridPosition)
        : gridPosition(_gridPosition),
        offset(_offset),
        model(glm::translate(glm::mat4(1.0f), _offset))
    {
        // Define cube vertices centered at origin (no offset here)
        vertices = {
            // Top face
            {{-0.5f,  0.5f, -0.5f}, topColor},
            {{ 0.5f,  0.5f, -0.5f}, topColor},
            {{ 0.5f,  0.5f,  0.5f}, topColor},
            {{-0.5f,  0.5f,  0.5f}, topColor},
            // Left face
            {{-0.5f,  0.5f,  0.5f}, leftColor},
            {{-0.5f, -0.5f,  0.5f}, leftColor},
            {{-0.5f, -0.5f, -0.5f}, leftColor},
            {{-0.5f,  0.5f, -0.5f}, leftColor},
            // Back face
            {{ 0.5f,  0.5f, -0.5f}, backColor},
            {{-0.5f,  0.5f, -0.5f}, backColor},
            {{-0.5f, -0.5f, -0.5f}, backColor},
            {{ 0.5f, -0.5f, -0.5f}, backColor},
            // Right face
            {{ 0.5f,  0.5f,  0.5f}, rightColor},
            {{ 0.5f, -0.5f,  0.5f}, rightColor},
            {{ 0.5f, -0.5f, -0.5f}, rightColor},
            {{ 0.5f,  0.5f, -0.5f}, rightColor},
            // Front face
            {{-0.5f,  0.5f,  0.5f}, frontColor},
            {{ 0.5f,  0.5f,  0.5f}, frontColor},
            {{ 0.5f, -0.5f,  0.5f}, frontColor},
            {{-0.5f, -0.5f,  0.5f}, frontColor},
            // Down face
            {{-0.5f, -0.5f, -0.5f}, downColor},
            {{ 0.5f, -0.5f, -0.5f}, downColor},
            {{ 0.5f, -0.5f,  0.5f}, downColor},
            {{-0.5f, -0.5f,  0.5f}, downColor}
        };

        setupBuffers();
    }

    // Copy constructor for deep copy (re-creates buffers)
    Piece(const Piece& other)
        : gridPosition(other.gridPosition),
        offset(other.offset),
        vertices(other.vertices),
        model(other.model)
    {
        setupBuffers();
    }

    // Accessor for offset
    glm::vec3 getOffset() const {
        return offset;
    }

    /*** Drawing and Transform Methods ***/

    void draw(Shader& shader, Camera& camera) {
        shader.setMat4("model", model);
        // view and projection are set externally
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void rotate(const glm::vec3& center, float degrees, const glm::vec3& axis, bool clockwise) {
        float radians = glm::radians(degrees * (clockwise ? 1.0f : -1.0f));
        model = glm::translate(model, -center);
        model = glm::rotate(model, radians, axis);
        model = glm::translate(model, center);
    }

    void rotateWorldAxis(const glm::vec3& center, float degrees, const glm::vec3& worldAxis, bool clockwise) {
        float radians = glm::radians(degrees * (clockwise ? 1.0f : -1.0f));
        model = glm::translate(model, -center);
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), radians, worldAxis);
        model = rotation * model;
        model = glm::translate(model, center);
    }

private:
    struct Vertex {
        glm::vec3 position;
        glm::vec3 color;
    };

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        8, 9,10,10,11, 8,
       12,13,14,14,15,12,
       16,17,18,18,19,16,
       20,21,22,22,23,20
    };

    unsigned int VAO = 0, VBO = 0, EBO = 0;
    glm::mat4 model;

    void setupBuffers() {
        if (VAO) { glDeleteVertexArrays(1, &VAO); glDeleteBuffers(1, &VBO); glDeleteBuffers(1, &EBO); }
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }
};
