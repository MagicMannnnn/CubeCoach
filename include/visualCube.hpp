#pragma once

#include <piece.hpp>
#include <queue>
#include <vector>



struct Move {
    glm::ivec3 axis = glm::ivec3(0, 0, 0);
    int layerValue;
    bool clockwise;
    float degrees;
    bool rotationNext = false;

    Move(glm::ivec3 axis, int layerValue, bool clockwise, float degrees)
        : axis(axis), layerValue(layerValue), clockwise(clockwise), degrees(degrees) {}
    Move(bool rotationNext) : rotationNext(rotationNext) {}

    // Copy constructor
    Move(const Move& other)
        : axis(other.axis), layerValue(other.layerValue), clockwise(other.clockwise),
        degrees(other.degrees), rotationNext(other.rotationNext) {}

    // Copy assignment operator
    Move& operator=(const Move& other) {
        if (this != &other) {  // Check for self-assignment
            axis = other.axis;
            layerValue = other.layerValue;
            clockwise = other.clockwise;
            degrees = other.degrees;
            rotationNext = other.rotationNext;
        }
        return *this;
    }

};



class VisualCube {
public:
    std::vector<Piece> pieces;
    std::vector<Piece> visualPieces;
    glm::mat4 cubeRotation = glm::mat4(1.0f);  // Tracks world rotation
    glm::mat3 logicalOrientation = glm::mat3(1.0f);  // Discrete face remapping

    // Store initial cube state before drag starts
    glm::mat4 initialCubeRotation = glm::mat4(1.0f);
    std::vector<Piece> initialPieces;
    bool rotating = false;
    std::queue<Move> moves;
    std::queue<Move> visualMoves;

    float rotationTime = 0.1f;
    float rotationProgress = 0.f;
    bool fast = false;

    VisualCube() {
        const float spacing = 1.05f;

        for (int x = -1; x <= 1; ++x) {
            for (int y = -1; y <= 1; ++y) {
                for (int z = -1; z <= 1; ++z) {
                    // Skip center piece
                    if (x == 0 && y == 0 && z == 0) continue;

                    glm::vec3 offset(x * spacing, y * spacing, z * spacing);

                    glm::vec3 top = y == 1 ? glm::vec3(1.0f, 1.0f, 1.0f) : glm::vec3(0.0f);
                    glm::vec3 down = y == -1 ? glm::vec3(1.0f, 1.0f, 0.0f) : glm::vec3(0.0f);
                    glm::vec3 left = x == -1 ? glm::vec3(1.0f, 0.5f, 0.0f) : glm::vec3(0.0f);
                    glm::vec3 right = x == 1 ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.0f);
                    glm::vec3 front = z == 1 ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(0.0f);
                    glm::vec3 back = z == -1 ? glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(0.0f);

                    pieces.emplace_back(top, left, back, right, front, down, offset, glm::ivec3(x, y, z));
                }
            }
        }


        initialCubeRotation = cubeRotation;  // Store the initial cube rotation
        initialPieces = pieces; // Store the initial pieces' state
        visualPieces = pieces;

    }


    void applyMove(const std::string& move) {
        if (move == "U") U();
        else if (move == "Up") Up();
        else if (move == "U2") U2();
        else if (move == "D") D();
        else if (move == "Dp") Dp();
        else if (move == "D2") D2();
        else if (move == "L") L();
        else if (move == "Lp") Lp();
        else if (move == "L2") L2();
        else if (move == "R") R();
        else if (move == "Rp") Rp();
        else if (move == "R2") R2();
        else if (move == "F") F();
        else if (move == "Fp") Fp();
        else if (move == "F2") F2();
        else if (move == "B") B();
        else if (move == "Bp") Bp();
        else if (move == "B2") B2();
        else if (move == "Y") Y();
        else if (move == "Yp") Yp();
    }

    void applyMoves(const std::string& moves) {
        std::string currentMove = "";
        for (char c : moves) {
            if (c == ' ') {
                applyMove(currentMove);
                currentMove = "";
            }
            else {
                currentMove += c;
            }

        }
        applyMove(currentMove);
    }


    void update(float dt) {

    keepGoingIfFast:;

        if (!visualMoves.empty()) {

            Move& move = visualMoves.front();

            float multiplier = move.degrees == 180.f ? 0.5f : 1.f;

            float oldRotationProgress = rotationProgress;
            rotationProgress += (dt / rotationTime) * multiplier;
            rotationProgress = std::min(rotationProgress, 1.f);

            if (fast) rotationProgress = 1.f;
            

            if (move.rotationNext) {
                if (rotationProgress < 1.f) {
                    visualRotation(rotationProgress - oldRotationProgress);
                }
                else {
                    visualRotation(rotationProgress - oldRotationProgress, true);
                    rotationProgress = 0.f;
                }
                

            }           
            else {
                partialRotateFace(move.axis, move.layerValue, move.clockwise, move.degrees * (rotationProgress - oldRotationProgress));
                if (rotationProgress == 1.f) {

                    rotationProgress = 0.f;
                    for (auto& p : visualPieces) {
                        if (getAxisValue(p.gridPosition, move.axis) == move.layerValue) {
                            p.gridPosition = rotatedGridPosition(p.gridPosition, move.axis, move.clockwise, int(move.degrees / 90.0f + 0.5f));
                        }
                    }

                    visualMoves.pop();

                }
            }
            

            



            if (visualMoves.empty()) {
                visualPieces = pieces; //prevent rounding errors during rotation
            }
            else if (fast){
                //goto keepGoingIfFast;
            }
            
        }

    }



    // Start drag operation and store initial state
    void startDrag() {
        if (!rotating) {
            initialCubeRotation = cubeRotation;  // Store the initial cube rotation
            initialPieces = pieces; // Store the initial pieces' state
        }
        rotating = true;
        
    }

    // Update the cube rotation and logically update the piece positions
    void rotate(float degrees, glm::vec3& axis, bool clockwise, bool updateLogic = true) {
        float radians = glm::radians(clockwise ? degrees : -degrees);
        cubeRotation = glm::rotate(glm::mat4(1.0f), radians, axis) * cubeRotation;

        /*
        // Update logical face orientation if needed
        if (updateLogic && (int(degrees) % 90 == 0)) {
            // Update the logical orientation matrix
            glm::mat3 step = glm::mat3(glm::rotate(glm::mat4(1.0f), radians, axis));
            logicalOrientation = step * logicalOrientation;

            // Update each piece's grid position logically (this ensures pieces are still correctly mapped)
            for (auto& piece : pieces) {
                piece.gridPosition = rotatedGridPosition(piece.gridPosition, glm::ivec3(axis), clockwise, int(degrees / 90));
            }
        }
        */
        // Rotate the pieces visually based on the world rotation matrix
        for (auto& piece : pieces) {
            piece.rotateWorldAxis(glm::vec3(0), degrees, axis, clockwise);
        }
        for (auto& piece : visualPieces) {
            piece.rotateWorldAxis(glm::vec3(0), degrees, axis, clockwise);
        }
    }

    // Reset cube to initial orientation
    void resetOrientation() {
        if (!rotating || !visualMoves.empty()) return;
        pieces = initialPieces;            // Reset to the initial pieces' positions
        cubeRotation = initialCubeRotation;  // Reset to the initial visual rotation
        rotating = false;

        while (!moves.empty()) {
            Move& move = moves.front();
            rotateFace(move.axis, move.layerValue, move.clockwise, move.degrees, false);
            moves.pop();
        }
        visualPieces = pieces;
    }

    // Draw all pieces of the cube
    void draw(Shader& shader, Camera& camera) {
        for (auto& piece : visualPieces) {
            piece.draw(shader, camera);
        }
    }

private:


    void visualRotation(float rotateAmount, bool lastRotation = false) {

        std::queue<Move> tempQ = visualMoves;
        std::vector<Move>  queueAsVector;

        while (!tempQ.empty() && queueAsVector.size() < 3) {
            Move& move = tempQ.front();
            if (!move.rotationNext) queueAsVector.insert(queueAsVector.begin(), move);
            tempQ.pop();
        }

        for (int i = 0; i < 3; i++) {
            Move& move = queueAsVector[i];
            partialRotateFace(move.axis, move.layerValue, move.clockwise, move.degrees * rotateAmount);
            if (lastRotation) {

            
                for (auto& p : visualPieces) {
                    if (getAxisValue(p.gridPosition, move.axis) == move.layerValue) {
                        p.gridPosition = rotatedGridPosition(p.gridPosition, move.axis, move.clockwise, int(move.degrees / 90.0f + 0.5f));
                    }
                }

                

            }
        }

        if (lastRotation) {
            for (int i = 0; i < 4; i++) {
                visualMoves.pop();
            }
        }

        queueAsVector.clear();
        
        //delete &tempQ;
       
    }

    void partialRotateFace(const glm::ivec3& axis, int layerValue, bool clockwise, float degrees) {



        glm::vec3 faceNormal = glm::vec3(axis);
        glm::vec3 worldAxis = glm::mat3(cubeRotation) * faceNormal;  // Apply cube rotation to world space axis

        // Apply the rotation to the pieces' positions and rotations
        for (auto& p : visualPieces) {
            if (getAxisValue(p.gridPosition, axis) == layerValue) {
                // Rotate the piece visually
                p.rotateWorldAxis(glm::vec3(0), degrees, worldAxis, clockwise);

            }
        }

    }


    // Helper method to check if a piece is on a given face
    bool isOnFace(const glm::vec3& pos, const glm::vec3& faceNormal) {
        return glm::abs(glm::dot(pos, faceNormal) - 1.0f) < 0.01f;
    }

    void rotateFace(const glm::ivec3& axis, int layerValue, bool clockwise, float degrees = 90.f, bool doVisualMoves = true) {

        bool updateMoves = rotating;
        if (updateMoves) {
            moves.push(Move(axis, layerValue, clockwise, degrees));  // Record the move
        }
        if (doVisualMoves) {
            visualMoves.push(Move(axis, layerValue, clockwise, degrees));
        }
        

        int steps = int(degrees / 90.0f + 0.5f);  // Always 1 or 2 steps (90° or 180°)

        glm::vec3 faceNormal = glm::vec3(axis);
        glm::vec3 worldAxis = glm::mat3(cubeRotation) * faceNormal;  // Apply cube rotation to world space axis

        // Apply the rotation to the pieces' positions and rotations
        for (auto& p : pieces) {
            if (getAxisValue(p.gridPosition, axis) == layerValue) {
                // Rotate the piece visually
                p.rotateWorldAxis(glm::vec3(0), degrees, worldAxis, clockwise);

                // Update the piece's logical grid position
                p.gridPosition = rotatedGridPosition(p.gridPosition, axis, clockwise, steps);
            }
        }

    }





    // Returns the component of pos selected by axis (1,0,0)->x, etc.
    int getAxisValue(const glm::ivec3& pos, const glm::ivec3& axis) {
        if (axis.x) return pos.x;
        if (axis.y) return pos.y;
        return pos.z;
    }

    // Rotate `pos` around the given grid‐axis by `steps`×90°, updating pos
    glm::ivec3 rotatedGridPosition(glm::ivec3 pos, const glm::ivec3& axis, bool clockwise, int steps) {
        for (int i = 0; i < steps; ++i) {
            if (axis.x) {
                int y = pos.y, z = pos.z;
                if (clockwise) { pos.y = -z; pos.z = y; }
                else { pos.y = z; pos.z = -y; }
            }
            else if (axis.y) {
                int x = pos.x, z = pos.z;
                if (clockwise) { pos.x = z; pos.z = -x; }
                else { pos.x = -z; pos.z = x; }
            }
            else { // axis.z
                int x = pos.x, y = pos.y;
                if (clockwise) { pos.x = -y; pos.y = x; }
                else { pos.x = y; pos.y = -x; }
            }
        }
        return pos;
    }

public:
    // U face = +Y in logical space
    void U() { rotateFace(logicalOrientation * glm::vec3(0, 1, 0), +1, false); }
    void Up() { rotateFace(logicalOrientation * glm::vec3(0, 1, 0), +1, true); }
    void U2() { rotateFace(logicalOrientation * glm::vec3(0, 1, 0), +1, false, 180.0f); }

    // D = -Y
    void Dp() { rotateFace(logicalOrientation * glm::vec3(0, 1, 0), -1, false); }
    void D() { rotateFace(logicalOrientation * glm::vec3(0, 1, 0), -1, true); }
    void D2() { rotateFace(logicalOrientation * glm::vec3(0, 1, 0), -1, false, 180.0f); }

    // F = +Z
    void F() { rotateFace(logicalOrientation * glm::vec3(0, 0, 1), +1, false); }
    void Fp() { rotateFace(logicalOrientation * glm::vec3(0, 0, 1), +1, true); }
    void F2() { rotateFace(logicalOrientation * glm::vec3(0, 0, 1), +1, false, 180.0f); }

    // B = -Z
    void Bp() { rotateFace(logicalOrientation * glm::vec3(0, 0, 1), -1, false); }
    void B() { rotateFace(logicalOrientation * glm::vec3(0, 0, 1), -1, true); }
    void B2() { rotateFace(logicalOrientation * glm::vec3(0, 0, 1), -1, false, 180.0f); }

    // L = -X
    void Lp() { rotateFace(logicalOrientation * glm::vec3(1, 0, 0), -1, false); }
    void L() { rotateFace(logicalOrientation * glm::vec3(1, 0, 0), -1, true); }
    void L2() { rotateFace(logicalOrientation * glm::vec3(1, 0, 0), -1, false, 180.0f); }

    // R = +X
    void R() { rotateFace(logicalOrientation * glm::vec3(1, 0, 0), +1, false); }
    void Rp() { rotateFace(logicalOrientation * glm::vec3(1, 0, 0), +1, true); }
    void R2() { rotateFace(logicalOrientation * glm::vec3(1, 0, 0), +1, false, 180.0f); }

    // Define slice moves
    void M() { rotateFace(logicalOrientation * glm::vec3(1, 0, 0), 0, true); }
    void Mp() { rotateFace(logicalOrientation * glm::vec3(1, 0, 0), 0, false); }
    void M2() { rotateFace(logicalOrientation * glm::vec3(1, 0, 0), 0, false, 180.f); }


    void E() { rotateFace(logicalOrientation * glm::vec3(0, 1, 0), 0, false); }
    void Ep() { rotateFace(logicalOrientation * glm::vec3(0, 1, 0), 0, true); }
    void E2() { rotateFace(logicalOrientation * glm::vec3(0, 1, 0), 0, true, 180.f); }

    void S() { rotateFace(logicalOrientation * glm::vec3(0, 0, 1), 0, true); }
    void Sp() { rotateFace(logicalOrientation * glm::vec3(0, 0, 1), 0, false); }

    //rotations
    void Y(){
        visualMoves.push(Move(true)); // queue rotation
        U();
        Dp();
        E();
    }
    void Yp() {
        visualMoves.push(Move(true)); // queue rotation
        Up();
        D();
        Ep();
    }

    void X() {
        visualMoves.push(Move(true)); // queue rotation
        R();
        Lp();
        Mp();
    }
    void Xp() {
        visualMoves.push(Move(true)); // queue rotation
        Rp();
        L();
        M();
    }


};
