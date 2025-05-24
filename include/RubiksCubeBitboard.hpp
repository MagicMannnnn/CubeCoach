#pragma once


#include <array>
#include <cstdint>
#include <iostream>
#include <bitset>
#include <cassert>

class RubiksCubeBitboard {
public:
    enum Face { FACE_U, FACE_D, FACE_L, FACE_R, FACE_F, FACE_B };

    // Each face is stored as a 64-bit integer; 8 stickers, 8 bits each
    std::array<uint64_t, 6> faces;
    std::array<uint64_t, 6> solvedFaces;

    RubiksCubeBitboard() {
        // Initialize each face with a different color for visualization
        for (int i = 0; i < 6; ++i) {
            faces[i] = 0;
            for (int j = 0; j < 8; ++j) {
                faces[i] |= (uint64_t(i) & 0xFF) << (j * 8);
            }
        }

        solvedFaces = faces;
    }

    bool isSolved() {
        return solvedFaces == faces;
    }

    void rotate_face_cw(Face f) {
        faces[f] = rolq(faces[f], 16);  // 90° CW rotation
    }

    void rotate_face_ccw(Face f) {
        faces[f] = rorq(faces[f], 16);  // 90° CCW rotation
    }

    void rotate_face_180(Face f) {
        faces[f] = rolq(faces[f], 32);  // 180° rotation
    }

    void U() {
        rotate_face_cw(FACE_U);
        cycle_edges({ FACE_F, FACE_R, FACE_B, FACE_L }, { 0, 1, 2 });
        cycle_corners({ FACE_F, FACE_R, FACE_B, FACE_L }, { 0, 1, 2 });
    }

    void Up() {
        rotate_face_ccw(FACE_U);
        cycle_edges({ FACE_F, FACE_L, FACE_B, FACE_R }, { 0, 1, 2 });
        cycle_corners({ FACE_F, FACE_L, FACE_B, FACE_R }, { 0, 1, 2 });
    }

    void U2() {
        rotate_face_180(FACE_U);
        cycle_edges({ FACE_F, FACE_B }, { 0, 1, 2 }, true);
        cycle_corners({ FACE_F, FACE_B }, { 0, 1, 2 }, true);
    }

    void D() {
        rotate_face_cw(D);
        cycle_edges({ F, L, B, R }, { 5, 6, 7 });
        cycle_corners({ F, L, B, R }, { 5, 6, 7 });
    }

    void Dp() {
        rotate_face_ccw(D);
        cycle_edges({ F, R, B, L }, { 5, 6, 7 });
        cycle_corners({ F, R, B, L }, { 5, 6, 7 });
    }

    void D2() {
        rotate_face_180(D);
        cycle_edges({ F, B }, { 5, 6, 7 }, true);
        cycle_corners({ F, B }, { 5, 6, 7 }, true);
    }

    void L() {
        rotate_face_cw(L);
        cycle_edges({ U, F, D, B }, { 0, 3, 5 }, false, true);
        cycle_corners({ U, F, D, B }, { 0, 3, 5 }, false, true);
    }

    void Lp() {
        rotate_face_ccw(L);
        cycle_edges({ U, B, D, F }, { 0, 3, 5 }, false, true);
        cycle_corners({ U, B, D, F }, { 0, 3, 5 }, false, true);
    }

    void L2() {
        rotate_face_180(L);
        cycle_edges({ U, D }, { 0, 3, 5 }, true, true);
        cycle_corners({ U, D }, { 0, 3, 5 }, true, true);
    }

    void R() {
        rotate_face_cw(R);
        cycle_edges({ U, B, D, F }, { 2, 4, 7 }, false, true);
        cycle_corners({ U, B, D, F }, { 2, 4, 7 }, false, true);
    }

    void Rp() {
        rotate_face_ccw(R);
        cycle_edges({ U, F, D, B }, { 2, 4, 7 }, false, true);
        cycle_corners({ U, F, D, B }, { 2, 4, 7 }, false, true);
    }

    void R2() {
        rotate_face_180(R);
        cycle_edges({ U, D }, { 2, 4, 7 }, true, true);
        cycle_corners({ U, D }, { 2, 4, 7 }, true, true);
    }

    void F() {
        rotate_face_cw(F);
        cycle_edges({ U, R, D, L }, { 5, 0, 3 }, false);
        cycle_corners({ U, R, D, L }, { 5, 0, 3 }, false);
    }

    void Fp() {
        rotate_face_ccw(F);
        cycle_edges({ U, L, D, R }, { 5, 0, 3 }, false);
        cycle_corners({ U, L, D, R }, { 5, 0, 3 }, false);
    }

    void F2() {
        rotate_face_180(F);
        cycle_edges({ U, D }, { 5, 3 }, true);
        cycle_corners({ U, D }, { 5, 3 }, true);
    }

    void B() {
        rotate_face_cw(B);
        cycle_edges({ U, L, D, R }, { 1, 2, 4 }, false);
        cycle_corners({ U, L, D, R }, { 1, 2, 4 }, false);
    }

    void Bp() {
        rotate_face_ccw(B);
        cycle_edges({ U, R, D, L }, { 1, 2, 4 }, false);
        cycle_corners({ U, R, D, L }, { 1, 2, 4 }, false);
    }

    void B2() {
        rotate_face_180(B);
        cycle_edges({ U, D }, { 1, 4 }, true);
        cycle_corners({ U, D }, { 1, 4 }, true);
    }

    void print_face(Face f) {
        uint64_t face = faces[f];
        for (int i = 0; i < 8; ++i) {
            std::cout << (int)((face >> (i * 8)) & 0xFF) << " ";
            if ((i + 1) % 3 == 0) std::cout << "\n";
        }
        std::cout << std::endl;
    }

private:
    // Rotate left
    static uint64_t rolq(uint64_t x, int r) {
        return (x << r) | (x >> (64 - r));
    }

    // Rotate right
    static uint64_t rorq(uint64_t x, int r) {
        return (x >> r) | (x << (64 - r));
    }

    // Cycle edge pieces between adjacent faces
    void cycle_edges(std::vector<Face> faces_list, std::vector<int> positions, bool is_double = false, bool reverse = false) {
        std::array<std::array<uint8_t, 8>, 6> stickers{};
        for (int i = 0; i < 6; ++i) {
            uint64_t f = faces[i];
            for (int j = 0; j < 8; ++j) {
                stickers[i][j] = (f >> (j * 8)) & 0xFF;
            }
        }

        // Rotate stickers based on faces_list
        auto tmp = stickers[faces_list[0]];
        if (is_double) {
            for (size_t i = 0; i < positions.size(); ++i) {
                std::swap(stickers[faces_list[0]][positions[i]], stickers[faces_list[1]][positions[i]]);
            }
        }
        else {
            int n = faces_list.size();
            for (int i = 0; i < n; ++i) {
                int from = reverse ? (i + 1) % n : (i + n - 1) % n;
                for (int pos : positions) {
                    stickers[faces_list[i]][pos] = tmp[pos];
                }
                tmp = stickers[faces_list[i]];
            }
        }

        // Rebuild the faces after cycling the stickers
        for (int i = 0; i < 6; ++i) {
            faces[i] = 0;
            for (int j = 0; j < 8; ++j) {
                faces[i] |= (uint64_t(stickers[i][j]) & 0xFF) << (j * 8);
            }
        }
    }

    // Cycle corners logic
    void cycle_corners(std::vector<Face> faces_list, std::vector<int> positions, bool is_double = false, bool reverse = false) {
        std::array<std::array<uint8_t, 8>, 6> stickers{};
        for (int i = 0; i < 6; ++i) {
            uint64_t f = faces[i];
            for (int j = 0; j < 8; ++j) {
                stickers[i][j] = (f >> (j * 8)) & 0xFF;
            }
        }

        // Implement similar logic for corner stickers
        auto tmp = stickers[faces_list[0]];
        if (is_double) {
            for (size_t i = 0; i < positions.size(); ++i) {
                std::swap(stickers[faces_list[0]][positions[i]], stickers[faces_list[1]][positions[i]]);
            }
        }
        else {
            int n = faces_list.size();
            for (int i = 0; i < n; ++i) {
                int from = reverse ? (i + 1) % n : (i + n - 1) % n;
                for (int pos : positions) {
                    stickers[faces_list[i]][pos] = tmp[pos];
                }
                tmp = stickers[faces_list[i]];
            }
        }

        // Rebuild the faces after cycling the corner stickers
        for (int i = 0; i < 6; ++i) {
            faces[i] = 0;
            for (int j = 0; j < 8; ++j) {
                faces[i] |= (uint64_t(stickers[i][j]) & 0xFF) << (j * 8);
            }
        }
    }
};