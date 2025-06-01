#pragma once

#include <iostream>
#include <array>

class fastCube {
public:
    // Corner positions (0-7) and orientations (0-2)
    uint8_t corner_pos[8];
    uint8_t corner_ori[8];

    // Edge positions (0-11) and orientations (0-1)
    uint8_t edge_pos[12];
    uint8_t edge_ori[12];

    // Constructor: Initializes to the solved cube state
    fastCube() {
        // Initialize solved state for corner positions and orientations
        for (int i = 0; i < 8; i++) {
            corner_pos[i] = i;  // Solved: corners in the solved position
            corner_ori[i] = 0;  // Solved: all corners oriented correctly
        }
        // Initialize solved state for edge positions and orientations
        for (int i = 0; i < 12; i++) {
            edge_pos[i] = i;  // Solved: edges in the solved position
            edge_ori[i] = 0;  // Solved: all edges oriented correctly
        }
    }

    // Check if the cube is solved
    bool is_solved() const {
        for (int i = 0; i < 8; i++) {
            if (corner_pos[i] != i || corner_ori[i] != 0) {
                return false;  // Found a corner not in the solved state
            }
        }
        for (int i = 0; i < 12; i++) {
            if (edge_pos[i] != i || edge_ori[i] != 0) {
                return false;  // Found an edge not in the solved state
            }
        }
        return true;  // All corners and edges are in the solved state
    }

    // Rotate 4 items (used for 4-cycles in moves)
    static void rotate4(uint8_t* arr, int a, int b, int c, int d) {
        uint8_t tmp = arr[a];
        arr[a] = arr[b];
        arr[b] = arr[c];
        arr[c] = arr[d];
        arr[d] = tmp;
    }

    // Rotate orientations for 4-cycle (corner/edge orientation)
    static void rotate4_ori(uint8_t* ori, int a, int b, int c, int d, int o1, int o2, int o3, int o4) {
        uint8_t tmp = ori[a];
        ori[a] = (ori[b] + o1) % 3;
        ori[b] = (ori[c] + o2) % 3;
        ori[c] = (ori[d] + o3) % 3;
        ori[d] = (tmp + o4) % 3;
    }

    // Apply a move on the cube (Right move)
    void R() {
        // Corner positions (4-cycle)
        rotate4(corner_pos, 0, 1, 2, 3);
        rotate4(corner_pos, 4, 5, 6, 7);

        // Edge positions (4-cycle)
        rotate4(edge_pos, 0, 1, 2, 3);
        rotate4(edge_pos, 4, 5, 6, 7);

        // Corner orientations (clockwise 3-cycle)
        rotate4_ori(corner_ori, 0, 1, 2, 3, +1, +1, -1, -1);
        rotate4_ori(corner_ori, 4, 5, 6, 7, +1, +1, -1, -1);

        // Edge orientations (flip)
        std::swap(edge_ori[0], edge_ori[1]);
        std::swap(edge_ori[2], edge_ori[3]);
        std::swap(edge_ori[4], edge_ori[5]);
        std::swap(edge_ori[6], edge_ori[7]);
    }

    // Apply a counterclockwise right move (R')
    void Rp() {
        // Reverse of R: Rotate positions back and orientations counter-clockwise
        rotate4(corner_pos, 0, 3, 2, 1);
        rotate4(corner_pos, 4, 7, 6, 5);

        rotate4(edge_pos, 0, 3, 2, 1);
        rotate4(edge_pos, 4, 7, 6, 5);

        rotate4_ori(corner_ori, 0, 3, 2, 1, -1, -1, +1, +1);
        rotate4_ori(corner_ori, 4, 7, 6, 5, -1, -1, +1, +1);

        std::swap(edge_ori[0], edge_ori[1]);
        std::swap(edge_ori[2], edge_ori[3]);
        std::swap(edge_ori[4], edge_ori[5]);
        std::swap(edge_ori[6], edge_ori[7]);
    }

    // Apply a 180° Right move (R2)
    void R2() {
        R();
        R();
    }

    // Apply an Up move (U)
    void U() {
        // Corner positions (4-cycle)
        rotate4(corner_pos, 0, 1, 4, 5);
        rotate4(corner_pos, 2, 3, 6, 7);

        // Edge positions (4-cycle)
        rotate4(edge_pos, 0, 1, 4, 5);
        rotate4(edge_pos, 2, 3, 6, 7);

        // Corner orientations (clockwise 3-cycle)
        rotate4_ori(corner_ori, 0, 1, 4, 5, +1, +1, -1, -1);
        rotate4_ori(corner_ori, 2, 3, 6, 7, +1, +1, -1, -1);

        // Edge orientations (flip)
        std::swap(edge_ori[0], edge_ori[1]);
        std::swap(edge_ori[2], edge_ori[3]);
        std::swap(edge_ori[4], edge_ori[5]);
        std::swap(edge_ori[6], edge_ori[7]);
    }

    // Apply a counterclockwise Up move (U')
    void Up() {
        // Reverse of U: Rotate positions back and orientations counter-clockwise
        rotate4(corner_pos, 0, 3, 2, 1);
        rotate4(corner_pos, 4, 7, 6, 5);

        rotate4(edge_pos, 0, 3, 2, 1);
        rotate4(edge_pos, 4, 7, 6, 5);

        rotate4_ori(corner_ori, 0, 3, 2, 1, -1, -1, +1, +1);
        rotate4_ori(corner_ori, 4, 7, 6, 5, -1, -1, +1, +1);

        std::swap(edge_ori[0], edge_ori[1]);
        std::swap(edge_ori[2], edge_ori[3]);
        std::swap(edge_ori[4], edge_ori[5]);
        std::swap(edge_ori[6], edge_ori[7]);
    }

    // Apply a 180° Up move (U2)
    void U2() {
        U();
        U();
    }

    // Apply a Left move (L)
    void L() {
        rotate4(corner_pos, 0, 4, 3, 7);
        rotate4(corner_pos, 1, 5, 2, 6);
        rotate4(edge_pos, 0, 4, 3, 7);
        rotate4(edge_pos, 1, 5, 2, 6);
        rotate4_ori(corner_ori, 0, 4, 3, 7, +1, +1, -1, -1);
        rotate4_ori(corner_ori, 1, 5, 2, 6, +1, +1, -1, -1);
        std::swap(edge_ori[0], edge_ori[1]);
        std::swap(edge_ori[2], edge_ori[3]);
        std::swap(edge_ori[4], edge_ori[5]);
        std::swap(edge_ori[6], edge_ori[7]);
    }

    // Apply a counterclockwise Left move (L')
    void Lp() {
        rotate4(corner_pos, 0, 7, 3, 4);
        rotate4(corner_pos, 1, 6, 2, 5);
        rotate4(edge_pos, 0, 7, 3, 4);
        rotate4(edge_pos, 1, 6, 2, 5);
        rotate4_ori(corner_ori, 0, 7, 3, 4, -1, -1, +1, +1);
        rotate4_ori(corner_ori, 1, 6, 2, 5, -1, -1, +1, +1);
        std::swap(edge_ori[0], edge_ori[1]);
        std::swap(edge_ori[2], edge_ori[3]);
        std::swap(edge_ori[4], edge_ori[5]);
        std::swap(edge_ori[6], edge_ori[7]);
    }

    // Apply a 180° Left move (L2)
    void L2() {
        L();
        L();
    }

    // Apply a Front move (F)
    void F() {
        rotate4(corner_pos, 0, 1, 2, 3);
        rotate4(corner_pos, 4, 5, 6, 7);
        rotate4(edge_pos, 0, 1, 2, 3);
        rotate4(edge_pos, 4, 5, 6, 7);
        rotate4_ori(corner_ori, 0, 1, 2, 3, +1, +1, -1, -1);
        rotate4_ori(corner_ori, 4, 5, 6, 7, +1, +1, -1, -1);
        std::swap(edge_ori[0], edge_ori[1]);
        std::swap(edge_ori[2], edge_ori[3]);
        std::swap(edge_ori[4], edge_ori[5]);
        std::swap(edge_ori[6], edge_ori[7]);
    }

    // Apply a counterclockwise Front move (F')
    void Fp() {
        rotate4(corner_pos, 0, 3, 2, 1);
        rotate4(corner_pos, 4, 7, 6, 5);
        rotate4(edge_pos, 0, 3, 2, 1);
        rotate4(edge_pos, 4, 7, 6, 5);
        rotate4_ori(corner_ori, 0, 3, 2, 1, -1, -1, +1, +1);
        rotate4_ori(corner_ori, 4, 7, 6, 5, -1, -1, +1, +1);
        std::swap(edge_ori[0], edge_ori[1]);
        std::swap(edge_ori[2], edge_ori[3]);
        std::swap(edge_ori[4], edge_ori[5]);
        std::swap(edge_ori[6], edge_ori[7]);
    }

    // Apply a 180° Front move (F2)
    void F2() {
        F();
        F();
    }

    // Apply a Back move (B)
    void B() {
        rotate4(corner_pos, 1, 5, 2, 6);
        rotate4(corner_pos, 0, 4, 3, 7);
        rotate4(edge_pos, 1, 5, 2, 6);
        rotate4(edge_pos, 0, 4, 3, 7);
        rotate4_ori(corner_ori, 1, 5, 2, 6, +1, +1, -1, -1);
        rotate4_ori(corner_ori, 0, 4, 3, 7, +1, +1, -1, -1);
        std::swap(edge_ori[0], edge_ori[1]);
        std::swap(edge_ori[2], edge_ori[3]);
        std::swap(edge_ori[4], edge_ori[5]);
        std::swap(edge_ori[6], edge_ori[7]);
    }

    // Apply a counterclockwise Back move (B')
    void Bp() {
        rotate4(corner_pos, 1, 6, 2, 5);
        rotate4(corner_pos, 0, 7, 3, 4);
        rotate4(edge_pos, 1, 6, 2, 5);
        rotate4(edge_pos, 0, 7, 3, 4);
        rotate4_ori(corner_ori, 1, 6, 2, 5, -1, -1, +1, +1);
        rotate4_ori(corner_ori, 0, 7, 3, 4, -1, -1, +1, +1);
        std::swap(edge_ori[0], edge_ori[1]);
        std::swap(edge_ori[2], edge_ori[3]);
        std::swap(edge_ori[4], edge_ori[5]);
        std::swap(edge_ori[6], edge_ori[7]);
    }

    // Apply a 180° Back move (B2)
    void B2() {
        B();
        B();
    }

    // Apply a Down move (D)
    void D() {
        // Corner positions (4-cycle)
        rotate4(corner_pos, 4, 5, 6, 7);

        // Edge positions (4-cycle)
        rotate4(edge_pos, 4, 5, 6, 7);

        // Corner orientations (clockwise 3-cycle)
        rotate4_ori(corner_ori, 4, 5, 6, 7, +1, +1, -1, -1);

        // Edge orientations (flip)
        std::swap(edge_ori[4], edge_ori[5]);
        std::swap(edge_ori[6], edge_ori[7]);
    }

    // Apply a counterclockwise Down move (D')
    void Dp() {
        // Reverse of D: Rotate positions back and orientations counter-clockwise
        rotate4(corner_pos, 4, 7, 6, 5);

        // Edge positions (4-cycle)
        rotate4(edge_pos, 4, 7, 6, 5);

        // Corner orientations (counterclockwise 3-cycle)
        rotate4_ori(corner_ori, 4, 7, 6, 5, -1, -1, +1, +1);

        // Edge orientations (flip)
        std::swap(edge_ori[4], edge_ori[5]);
        std::swap(edge_ori[6], edge_ori[7]);
    }

    // Apply a 180° Down move (D2)
    void D2() {
        D();
        D();
    }

};
