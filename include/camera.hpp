#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>
#include <iostream>

// Defines several possible options for camera movement.
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = -20.0f;
const float SPEED = 2.5f * 3.f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

const float MAX_HEIGHT = 20.f;
const float MIN_HEIGHT = -20.f;

// An abstract camera class
class Camera {
public:
    // Camera attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    float Yaw;
    float Pitch;
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = YAW, float pitch = PITCH)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        m_fast = false;
        updateCameraVectors();
    }

    // Returns the view matrix
    glm::mat4 GetViewMatrix() {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // Processes input
    void ProcessKeyboard(Camera_Movement direction, float deltaTime) {
        float velocity = MovementSpeed * deltaTime;
        if (m_fast) velocity *= m_fastMultiplier;

        if (direction == FORWARD) Position += glm::vec3(Front.x, 0.f, Front.z) * velocity;
        if (direction == BACKWARD) Position -= glm::vec3(Front.x, 0.f, Front.z) * velocity;
        if (direction == LEFT) Position -= glm::vec3(Right.x, 0.f, Right.z) * velocity;
        if (direction == RIGHT) Position += glm::vec3(Right.x, 0.f, Right.z) * velocity;
        if (direction == UP) Position += WorldUp * velocity;
        if (direction == DOWN) Position -= WorldUp * velocity;

        if (Position.y > MAX_HEIGHT) {
            Position.y = MAX_HEIGHT;
        }
        else if (Position.y < MIN_HEIGHT) {
            Position.y = MIN_HEIGHT;
        }
    }

    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        if (constrainPitch) {
            if (Pitch > 89.0f) Pitch = 89.0f;
            if (Pitch < -89.0f) Pitch = -89.0f;
        }
        updateCameraVectors();
    }

    void ProcessMouseScroll(float yoffset, float deltaTime) {
        /*
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f) Zoom = 1.0f;
        if (Zoom > 45.0f) Zoom = 45.0f;
        */

        if ((Position.y == MAX_HEIGHT && yoffset < 0) || (Position.y == MIN_HEIGHT && yoffset > 0)) return;

        Position += yoffset * Front * MovementSpeed * deltaTime * 10.f * (Position.y / (MAX_HEIGHT / 3.f));

        std::cout << Position.y << " ";

        if (Position.y > MAX_HEIGHT) {
            Position.y = MAX_HEIGHT;
        }
        else if (Position.y < MIN_HEIGHT) {
            Position.y = MIN_HEIGHT;
        }
    }

    void setFastVelocity(bool fast) { m_fast = fast; }


    void ExtractFrustumPlanes(const glm::mat4& projection, const glm::mat4& view) {
        // Calculate the combined view-projection matrix
        glm::mat4 viewProjection = projection * view;

        // Extract planes directly from the matrix
        // Plane equations are in the form Ax + By + Cz + D = 0
        frustumPlanes[0] = glm::vec4(viewProjection[0][3] + viewProjection[0][0], // Left
            viewProjection[1][3] + viewProjection[1][0],
            viewProjection[2][3] + viewProjection[2][0],
            viewProjection[3][3] + viewProjection[3][0]);

        frustumPlanes[1] = glm::vec4(viewProjection[0][3] - viewProjection[0][0], // Right
            viewProjection[1][3] - viewProjection[1][0],
            viewProjection[2][3] - viewProjection[2][0],
            viewProjection[3][3] - viewProjection[3][0]);

        frustumPlanes[2] = glm::vec4(viewProjection[0][3] + viewProjection[0][1], // Bottom
            viewProjection[1][3] + viewProjection[1][1],
            viewProjection[2][3] + viewProjection[2][1],
            viewProjection[3][3] + viewProjection[3][1]);

        frustumPlanes[3] = glm::vec4(viewProjection[0][3] - viewProjection[0][1], // Top
            viewProjection[1][3] - viewProjection[1][1],
            viewProjection[2][3] - viewProjection[2][1],
            viewProjection[3][3] - viewProjection[3][1]);

        frustumPlanes[4] = glm::vec4(viewProjection[0][3] + viewProjection[0][2], // Near
            viewProjection[1][3] + viewProjection[1][2],
            viewProjection[2][3] + viewProjection[2][2],
            viewProjection[3][3] + viewProjection[3][2]);

        frustumPlanes[5] = glm::vec4(viewProjection[0][3] - viewProjection[0][2], // Far
            viewProjection[1][3] - viewProjection[1][2],
            viewProjection[2][3] - viewProjection[2][2],
            viewProjection[3][3] - viewProjection[3][2]);

        // Normalize the planes (important for correct distance calculations)
        for (int i = 0; i < 6; i++) {
            glm::vec3 normal = glm::vec3(frustumPlanes[i]);
            float length = glm::length(normal);
            if (length > 0.0f) { // Avoid division by zero
                frustumPlanes[i] /= length;
            }
        }
    }

    bool IsSphereInFrustum(const glm::vec3& center, float radius) {
        for (int i = 0; i < 6; i++) {
            glm::vec3 normal = glm::vec3(frustumPlanes[i]);
            float distance = glm::dot(normal, center) + frustumPlanes[i].w;

            // If the distance is less than -radius, the sphere is outside the frustum
            if (distance < -radius) {
                return false;
            }
        }
        return true; // Sphere is inside or intersects the frustum
    }



private:
    void updateCameraVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }

    glm::vec4 frustumPlanes[6];
    bool m_fast;
    float m_fastMultiplier = 5.f;
};

#endif
