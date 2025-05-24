#pragma once

#include <array>
#include <iostream>
#include <bitset>
#include <iomanip>
#include <functional>


typedef std::function<uint16_t()> EdgeGetter;
typedef std::function<uint32_t()> CornerGetter;

class BitwiseCube {
public:
	BitwiseCube() {
		for (int i = 0; i < 6; i++) {
			m_faces[i] = static_cast<uint64_t>(i);
			for (int j = 1; j < 8; j++) {
				m_faces[i] |= (static_cast<uint64_t>(i) << (j * 8));
			}
		}
        m_facesSolved = m_faces;
	}



    void test() {
        

    }



    void reset(bool fully = false) {
        m_faces = m_facesSolved;

        if (fully) {
            for (int i = 0; i < 6; i++) {
                m_faces[i] = static_cast<uint64_t>(i);
                for (int j = 1; j < 8; j++) {
                    m_faces[i] |= (static_cast<uint64_t>(i) << (j * 8));
                }
            }
            m_facesSolved = m_faces;
        }
    }

    std::array<uint64_t, 6> getFacesSolved() {
        return m_facesSolved;
    }

    void reset(BitwiseCube cube) {
        m_facesSolved = cube.getFacesSolved();
        m_faces = m_facesSolved;
    }

    bool isSolved() {
        for (int i = 0; i < 6; i++) {
            if (m_faces[i] != m_facesSolved[i]) return false;
        }
        return true;
    }


    bool isCrossSolved() {

        return getCrossState() == getCrossStateSolved();

        /*
        //bottom side matches
        if ((m_faces[fD] & 0x00FF00FF00FF00FF) != (m_facesSolved[fD] & 0x00FF00FF00FF00FF)) {
            return false;
        }

        if ((m_faces[fL] & 0xFF0000) != (m_facesSolved[fL] & 0xFF0000)) return false;
        if ((m_faces[fB] & 0xFF0000) != (m_facesSolved[fB] & 0xFF0000)) return false;
        if ((m_faces[fR] & 0xFF0000) != (m_facesSolved[fR] & 0xFF0000)) return false;
        if ((m_faces[fD] & 0xFF0000) != (m_facesSolved[fD] & 0xFF0000)) return false;

        return true;
        */
    }

    uint64_t getCrossState() {
        uint16_t solvedEdges[4] = { getDFSolved(), getDRSolved(), getDBSolved(), getDLSolved() };
        uint16_t edges[12] = {
            getUF(), getUR(), getUB(), getUL(),
            getDF(), getDR(), getDB(), getDL(),
            getFR(), getFL(), getBR(), getBL()
        };

        uint64_t state = 0;

        for (int i = 0; i < 4; ++i) {
            uint16_t solved = solvedEdges[i];
            for (int j = 0; j < 12; ++j) {
                if (edges[j] == solved || flipEdge(edges[j]) == solved) {
                    bool flipped = (flipEdge(edges[j]) == solved);
                    // Encode: 4 bits for position (j), 1 bit for flipped, total 5 bits per edge
                    // Shift and OR into result
                    state |= (static_cast<uint64_t>((j & 0xF) | (flipped ? 0x10 : 0)) << (i * 6));
                    break;
                }
            }
        }

        return state;
    }


    uint64_t getCrossStateSolved() {
        // Positions of DF, DR, DB, DL in the standard edge ordering (0-11):
        // UF, UR, UB, UL, DF, DR, DB, DL, FR, FL, BR, BL
        const int positions[4] = { 4, 5, 6, 7 };  // DF, DR, DB, DL
        uint64_t state = 0;

        for (int i = 0; i < 4; ++i) {
            // No flipped bit set (0), only position encoded
            state |= (static_cast<uint64_t>(positions[i] & 0xF) << (i * 6));
        }

        return state;
    }


    bool getOLLSolved() {
        return m_faces[0] == m_facesSolved[0];
    }


    uint64_t getCornerState(CornerGetter* getCornerFuncs, CornerGetter* getCornerSolvedFuncs, int count) {
        uint64_t state = 0;

        for (int i = 0; i < count; ++i) {
            uint32_t solved = getCornerSolvedFuncs[i]();
            for (int j = 0; j < 8; ++j) {  // 8 possible corners
                uint32_t corner = getCornerFuncs[j]();

                for (int rot = 0; rot < 3; ++rot) {
                    if (corner == solved) {
                        state |= ((j & 0x7) | (rot << 3)) << (i * 6);
                        goto next_corner;
                    }
                    solved = rotateCorner(solved);
                }
            }
        next_corner:;
        }

        return state;
    }



    uint64_t getCombinedState(
        EdgeGetter* edgeFuncs, EdgeGetter* edgeSolvedFuncs, int edgeCount,
        CornerGetter* cornerFuncs, CornerGetter* cornerSolvedFuncs, int cornerCount
    ) {
        uint64_t state = 0;
        int shift = 0;
        
        
        // Encode edges
        for (int i = 0; i < edgeCount; ++i) {
            uint16_t solved = edgeSolvedFuncs[i]();
            for (int j = 0; j < 12; ++j) {
                uint16_t edge = edgeFuncs[j]();
                if (edge == solved || flipEdge(edge) == solved) {
                    bool flipped = (flipEdge(edge) == solved);
                    uint64_t encoded = (j & 0xF) | (flipped ? 0x10 : 0);
                    state |= (encoded << shift);
                    shift += 5;
                    break;
                }
            }
        }
        
        for (int i = 0; i < cornerCount; ++i) {
            uint32_t solved = cornerSolvedFuncs[i]();
            ////std::cout << "\nSolved corner:   ";
            //displayHelper(static_cast<uint64_t>(solved));
            bool matched = false;

            for (int j = 0; j < 8; ++j) {
                uint32_t corner = cornerFuncs[j]();
                //std::cout << "\ncorner " << j << ":   ";
                //displayHelper(static_cast<uint64_t>(corner));

                for (int switched = 0; switched < 2; switched++) {
                    for (int rot = 0; rot < 3; ++rot) {
                        if (corner == solved) {
                            uint64_t encoded = (j & 0x7) | ((rot & 0x3) << 3);
                            state |= (encoded << shift);
                            shift += 6;
                            matched = true;
                            break;
                        }
                        solved = rotateCorner(solved);
                    }
                    if (matched) break;
                    solved = swapCornerState(solved);
                }
                if (matched) break;
            }

            if (!matched) {
                std::cerr << "\nWarning: No matching corner found for corner index " << i << "\n";
                // Optionally encode something invalid to catch errors, e.g. 0x3F
                state |= (0x3FULL << shift);
                shift += 5;
            }
        }

        return state;
    }



    uint64_t getStatePair(bool FRSolved, bool FLSolved, bool BLSolved, bool BRSolved) {
        // Bind to the current instance (`this`), not hardcoded at construction
        EdgeGetter edgeFuncs[12] = {
            std::bind(&BitwiseCube::getUF, this),
            std::bind(&BitwiseCube::getUR, this),
            std::bind(&BitwiseCube::getUB, this),
            std::bind(&BitwiseCube::getUL, this),
            std::bind(&BitwiseCube::getDF, this),
            std::bind(&BitwiseCube::getDR, this),
            std::bind(&BitwiseCube::getDB, this),
            std::bind(&BitwiseCube::getDL, this),
            std::bind(&BitwiseCube::getFR, this),
            std::bind(&BitwiseCube::getFL, this),
            std::bind(&BitwiseCube::getBR, this),
            std::bind(&BitwiseCube::getBL, this)
        };

        CornerGetter cornerFuncs[8] = {
            std::bind(&BitwiseCube::getUFL, this),
            std::bind(&BitwiseCube::getUFR, this),
            std::bind(&BitwiseCube::getDFR, this),
            std::bind(&BitwiseCube::getDFL, this),
            std::bind(&BitwiseCube::getUBL, this),
            std::bind(&BitwiseCube::getUBR, this),
            std::bind(&BitwiseCube::getDBL, this),
            std::bind(&BitwiseCube::getDBR, this)
        };



        EdgeGetter FPedgeSolvedFuncs[8];
        int edgeCount = 0;

        CornerGetter FPcornerSolvedFuncs[4];
        int cornerCount = 0;

        FPedgeSolvedFuncs[edgeCount++] = std::bind(&BitwiseCube::getDFSolved, this);
        FPedgeSolvedFuncs[edgeCount++] = std::bind(&BitwiseCube::getDRSolved, this);
        FPedgeSolvedFuncs[edgeCount++] = std::bind(&BitwiseCube::getDBSolved, this);
        FPedgeSolvedFuncs[edgeCount++] = std::bind(&BitwiseCube::getDLSolved, this);
        
 
        if (FRSolved) {
            FPcornerSolvedFuncs[cornerCount++] = std::bind(&BitwiseCube::getDFRSolved, this);
            FPedgeSolvedFuncs[edgeCount++] = std::bind(&BitwiseCube::getFRSolved, this);
        }

        if (FLSolved) {
            FPcornerSolvedFuncs[cornerCount++] = std::bind(&BitwiseCube::getDFLSolved, this);
            FPedgeSolvedFuncs[edgeCount++] = std::bind(&BitwiseCube::getFLSolved, this);
        }
            
        if (BLSolved) {
            FPcornerSolvedFuncs[cornerCount++] = std::bind(&BitwiseCube::getDBLSolved, this);
            FPedgeSolvedFuncs[edgeCount++] = std::bind(&BitwiseCube::getBLSolved, this);
        }
            
        if (BRSolved) {
            FPcornerSolvedFuncs[cornerCount++] = std::bind(&BitwiseCube::getDBRSolved, this);
            FPedgeSolvedFuncs[edgeCount++] = std::bind(&BitwiseCube::getBRSolved, this);
        }
            

        

        return getCombinedState(edgeFuncs, FPedgeSolvedFuncs, edgeCount, cornerFuncs, FPcornerSolvedFuncs, cornerCount);
    }









    void displayHex() {
        std::ios oldState(nullptr);
        oldState.copyfmt(std::cout);

        std::cout << "\n";
        for (int i = 0; i < 6; i++) {
            std::cout << "Face " << i << " (Hex): ";

            // Loop through the 64-bit number and print each byte
            for (int j = 7; j >= 0; j--) {
                // Extract each byte (shift by 8 bits and mask with 0xFF)
                uint8_t byte = (m_faces[i] >> (j * 8)) & 0xFF;
                std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte << " ";
            }

            std::cout << std::endl;
        }

        std::cout.copyfmt(oldState);
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

    


    void U() {
        rotateFaceCw(fU);
        updateAdjacentFaces(fL, fB, fR, fF);
    }
    void Up() {
        rotateFaceCcw(fU);
        updateAdjacentFaces(fL, fF, fR, fB);
    }

    void U2() {
        rotateFaceDouble(fU);
        updateAdjacentFacesDouble(fL, fB, fR, fF);
    }

    void D() {
        rotateFaceCw(fD);
        updateAdjacentFaces(fL, fF, fR, fB, 32, 32, 32, 32);
    }
    void Dp() {
        rotateFaceCcw(fD);
        updateAdjacentFaces(fL, fB, fR, fF, 32, 32 ,32, 32);
    }

    void D2() {
        rotateFaceDouble(fD);
        updateAdjacentFacesDouble(fL, fB, fR, fF, 32, 32, 32, 32);
    }

    void L() {
        rotateFaceCw(fL);
        updateAdjacentFaces(fU, fF, fD, fB, 48, 48, 48, 16);
    }
    //current working on Lp
    void Lp() {
        rotateFaceCcw(fL);
        updateAdjacentFaces(fU, fB, fD, fF, 48, 16, 48, 48);
    }

    void L2() {
        rotateFaceDouble(fL);
        updateAdjacentFacesDouble(fU, fF, fD, fB, 48, 48, 48, 16);
    }

    void R() {
        rotateFaceCw(fR);
        updateAdjacentFaces(fU, fB, fD, fF, 16, 48, 16, 16);
    }
    void Rp() {
        rotateFaceCcw(fR);
        updateAdjacentFaces(fU, fF, fD, fB, 16, 16, 16, 48);
    }

    void R2() {
        rotateFaceDouble(fR);
        updateAdjacentFacesDouble(fU, fB, fD, fF, 16, 48, 16, 16);
    }

    void F() {
        rotateFaceCw(fF);
        updateAdjacentFaces(fU, fR, fD, fL, 32, 48, 0, 16);
    }
    void Fp() {
        rotateFaceCcw(fF);
        updateAdjacentFaces(fU, fL, fD, fR, 32, 16, 0, 48);
    }

    void F2() {
        rotateFaceDouble(fF);
        updateAdjacentFacesDouble(fU, fR, fD, fL, 32, 48, 0, 16);
    }

    //B IS THE PROBLEM
    void B() {
        rotateFaceCw(fB);
        updateAdjacentFaces(fU, fL, fD, fR, 0, 48, 32, 16);
    }
    void Bp() {
        rotateFaceCcw(fB);
        updateAdjacentFaces(fU, fR, fD, fL, 0, 16, 32, 48);
    }

    void B2() {
        rotateFaceDouble(fB);
        updateAdjacentFacesDouble(fU, fL, fD, fR, 0, 48, 32, 16);
    }

    void X() {
        rotateFaceCw(fR);
        rotateFaceCcw(fL);
        uint64_t tempfu = m_faces[fU];
        m_faces[fU] = m_faces[fF];
        m_faces[fF] = m_faces[fD];
        m_faces[fD] = m_faces[fB];
        m_faces[fB] = tempfu;

        rotateFaceCwSolved(fR);
        rotateFaceCcwSolved(fL);
        tempfu = m_facesSolved[fU];
        m_facesSolved[fU] = m_facesSolved[fF];
        m_facesSolved[fF] = m_facesSolved[fD];
        m_facesSolved[fD] = m_facesSolved[fB];
        m_facesSolved[fB] = tempfu;

        rotateFaceDouble(fB);
        rotateFaceDouble(fD);
        rotateFaceDoubleSolved(fB); //only used if solved cube is non standard
        rotateFaceDoubleSolved(fD);
    }

    //do properly later
    void Xp() {
        X();
        X();
        X();
    }

    void Y() {
        rotateFaceCw(fU);
        rotateFaceCcw(fD);
        uint64_t tempfF = m_faces[fF];
        m_faces[fF] = m_faces[fR];
        m_faces[fR] = m_faces[fB];
        m_faces[fB] = m_faces[fL];
        m_faces[fL] = tempfF;

        rotateFaceCwSolved(fR);
        rotateFaceCcwSolved(fL);
        tempfF = m_facesSolved[fF];
        m_facesSolved[fF] = m_facesSolved[fR];
        m_facesSolved[fR] = m_facesSolved[fB];
        m_facesSolved[fB] = m_facesSolved[fL];
        m_facesSolved[fL] = tempfF;

    }

    void Yp() {
        Y();
        Y();
        Y();
    }

    void M() {
        Xp();
        R();
        Lp();
    }

    void Mp() {
        X();
        Rp();
        L();
    }

    void M2() {
        M();
        M();
    }

private:
	enum FACE { fU, fL, fB, fR, fF, fD };
	enum COLOURS { WHITE, ORANGE, BLUE, RED, GREEN, YELLOW };

	std::array<uint64_t, 6> m_faces;
    std::array<uint64_t, 6> m_facesSolved;


    

private:
    void rotateFaceCw(FACE face) {
        uint64_t a = (m_faces[face] >> 16) & 0xFFFFFFFFFFFF;
        uint64_t b = (m_faces[face] << 48) & 0xFFFF000000000000;
        m_faces[face] = a | b;
    }

    void rotateFaceCcw(FACE face) {
        uint64_t a = (m_faces[face] << 16) & 0xFFFFFFFFFFFF0000;
        uint64_t b = (m_faces[face] >> 48) & 0xFFFF;
        m_faces[face] = a | b;
    }

    void rotateFaceDouble(FACE face) {
        uint64_t a = (m_faces[face] << 32) & 0xFFFFFFFF00000000;
        uint64_t b = (m_faces[face] >> 32) & 0xFFFFFFFF;
        m_faces[face] = a | b;
    }

    void rotateFaceCwSolved(FACE face) {
        uint64_t a = (m_facesSolved[face] >> 16) & 0xFFFFFFFFFFFF;
        uint64_t b = (m_facesSolved[face] << 48) & 0xFFFF000000000000;
        m_facesSolved[face] = a | b;
    }

    void rotateFaceCcwSolved(FACE face) {
        uint64_t a = (m_facesSolved[face] << 16) & 0xFFFFFFFFFFFF0000;
        uint64_t b = (m_facesSolved[face] >> 48) & 0xFFFF;
        m_facesSolved[face] = a | b;
    }

    void rotateFaceDoubleSolved(FACE face) {
        uint64_t a = (m_facesSolved[face] << 32) & 0xFFFFFFFF00000000;
        uint64_t b = (m_facesSolved[face] >> 32) & 0xFFFFFFFF;
        m_facesSolved[face] = a | b;
    }

    void updateAdjacentFaces(FACE f1, FACE f2, FACE f3, FACE f4, int offset1 = 0, int offset2 = 0, int offset3 = 0, int offset4 = 0) {
        
        uint64_t newf1 = getNewSide(f1, f4, offset1, offset4);
        uint64_t newf2 = getNewSide(f2, f1, offset2, offset1);
        uint64_t newf3 = getNewSide(f3, f2, offset3, offset2);
        uint64_t newf4 = getNewSide(f4, f3, offset4, offset3);

        m_faces[f1] = newf1;
        m_faces[f2] = newf2;
        m_faces[f3] = newf3;
        m_faces[f4] = newf4;
        
    
    }

    //potential optimiseation of newf3 and newf4 - just remove them? directlyh updsate m_faces
    void updateAdjacentFacesDouble(FACE f1, FACE f2, FACE f3, FACE f4, int offset1 = 0, int offset2 = 0, int offset3 = 0, int offset4 = 0) {

        uint64_t newf1 = getNewSide(f1, f3, offset1, offset3);
        uint64_t newf2 = getNewSide(f2, f4, offset2, offset4);
        uint64_t newf3 = getNewSide(f3, f1, offset3, offset1);
        uint64_t newf4 = getNewSide(f4, f2, offset4, offset2);

        m_faces[f1] = newf1;
        m_faces[f2] = newf2;
        m_faces[f3] = newf3;
        m_faces[f4] = newf4;
    }

    //dont need this keeping just in case
    void flipRunOfThree(uint64_t& side, int offset1, int offset2) {
        std::cout << "\n\nside before: ";
        displayHelper(side);
        if (offset2 == 48) {
            side = (side & 0x000000000000FFFF) | (side >> 40);
            //std::cout << "\n";
            //displayHelper(side);
        }
        else {
            side >>= (40 - offset2);
        }
        
        uint64_t a = side & 0xFF0000;
        uint64_t b = side & 0x00FF00;
        uint64_t c = side & 0x0000FF;
        a >>= 16;
        c <<= 16;
        side = a | b | c;
        if (offset2 == 48) {
            side = (side & 0xFFF) | ((side & 0xFF0000) << 40);
        }
        else {
            side <<= (40 - offset2);
        }
        std::cout << "\nside after: ";
        displayHelper(side);
        
    }//dont even need this but keeping just in case

    uint64_t getNewSide(FACE f1, FACE f2, int offset1, int offset2) {
        uint64_t mask1B;
        uint64_t mask2A;

        switch (offset1)
        {
        case 0:
            mask1B = 0x000000FFFFFFFFFF;
            break;

        case 16:
            mask1B = 0xFFFF000000FFFFFF;
            break;

        case 32:
            mask1B = 0xFFFFFFFF000000FF;
            break;

        case 48:
            mask1B = 0x00FFFFFFFFFF0000;
            break;

        default:
            break;
        }

        switch (offset2)
        {
        case 0:
            mask2A = 0xFFFFFF0000000000;
            break;

        case 16:
            mask2A = 0x0000FFFFFF000000;
            break;

        case 32:
            mask2A = 0x00000000FFFFFF00;
            break;

        case 48:
            mask2A = 0xFF0000000000FFFF;
            break;

        default:
            break;
        }

        uint64_t tempf1 = m_faces[f1] & mask1B;
        uint64_t newthree = m_faces[f2] & mask2A;


        switch (offset1) {
        case 0:
            switch (offset2) {
            case 16:
                newthree <<= 16;
                break;
            case 32:
                newthree <<= 32;
                break;
            case 48:
                newthree = (newthree >> 16) | (newthree << 48);
                break;
            default:
                break;
            }
            break;

        case 16:
            switch (offset2) {
            case 0:
                newthree >>= 16;
                break;
            case 32:
                newthree <<= 16;
                break;
            case 48:
                newthree = (newthree >> 32) | (newthree << 32);
                break;
            default:
                break;
            }
            break;

        case 32:
            switch (offset2) {
            case 0:
                newthree >>= 32;
                break;
            case 16:
                newthree >>= 16;
                break;
            case 48:
                newthree = (newthree >> 48) | (newthree << 16);
                break;
            default:
                break;
            }
            break;

        case 48:
            switch (offset2) {
            case 0:
                newthree = (newthree >> 48) | (newthree << 16);
                break;
            case 16:
                newthree = (newthree >> 32) | (newthree << 32);
                break;
            case 32:
                newthree = (newthree >> 16) | (newthree << 48);
                break;
            default:
                break;
            }
            break;

        default:
            std::cout << "test";
            break;
        
        }

        /*
        std::cout << "\nnewthree: ";
        displayHelper(newthree);
        std::cout << "          tempf1: ";
        displayHelper(tempf1);
        */


        return newthree | tempf1;

    }


    void displayHelper(uint64_t side) {
        std::cout << "\n";

        std::ios oldState(nullptr);
        oldState.copyfmt(std::cout);

        // Loop through the 64-bit number and print each byte
        for (int i = 7; i >= 0; i--) {
            // Extract each byte (shift by 8 bits and mask with 0xFF)
            uint8_t byte = (side >> (i * 8)) & 0xFF;
            std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte << " ";
        }

        std::cout.copyfmt(oldState);
    }

    void displayHelper(uint16_t edge) {
        std::cout << "\n";

        std::ios oldState(nullptr);
        oldState.copyfmt(std::cout);

        // Loop through the 16-bit number and print each byte
        for (int i = 1; i >= 0; i--) {
            // Extract each byte (shift by 8 bits and mask with 0xFF)
            uint8_t byte = (edge >> (i * 8)) & 0xFF;
            std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte << " ";
        }

        std::cout.copyfmt(oldState);
    }


    //for sovler, need to store the state of the 4 cross pieces, not the state of the POSITIONs of the 4 cross pieces

    

    inline uint16_t flipEdge(uint16_t edge) {
        return (edge >> 8) | (edge << 8);
    }
    
    uint16_t getUF() {
        return ((m_faces[0] & 0xFF0000) >> 8) | ((m_faces[4] & 0xFF000000000000) >> 48);
    }

    uint16_t getUR() {
        return ((m_faces[0] & 0xFF00000000) >> 24) | ((m_faces[3] & 0xFF000000000000) >> 48);
    }

    uint16_t getUB() {
        return ((m_faces[0] & 0xFF000000000000) >> 40) | ((m_faces[2] & 0xFF000000000000) >> 48);
    }

    uint16_t getUL() {
        return ((m_faces[0] & 0xFF) << 8) | ((m_faces[1] & 0xFF000000000000) >> 48);
    }

    uint16_t getDF() {
        return ((m_faces[5] & 0xFF000000000000) >> 40) | ((m_faces[4] & 0xFF0000) >> 16);
    }

    uint16_t getDR() {
        return ((m_faces[5] & 0xFF00000000) >> 24) | ((m_faces[3] & 0xFF0000) >> 16);
    }

    uint16_t getDB() {
        return ((m_faces[5] & 0xFF0000) >> 8) | ((m_faces[2] & 0xFF0000) >> 16);
    }

    uint16_t getDL() {
        return ((m_faces[5] & 0xFF) << 8) | ((m_faces[1] & 0xFF0000) >> 16);
    }

    uint16_t getFL() {
        return ((m_faces[4] & 0xFF) << 8) | ((m_faces[1] & 0xFF00000000) >> 32);
    }

    uint16_t getFR() {
        return ((m_faces[4] & 0xFF00000000) >> 24) | (m_faces[3] & 0xFF);
    }

    uint16_t getBR() {
        return ((m_faces[2] & 0xFF) << 8) | ((m_faces[3] & 0xFF00000000) >> 32);
    }

    uint16_t getBL() {
        return ((m_faces[2] & 0xFF00000000) >> 24) | (m_faces[1] & 0xFF);
    }


    uint16_t getUFSolved() {
        return ((m_facesSolved[0] & 0xFF0000) >> 8) | ((m_facesSolved[4] & 0xFF000000000000) >> 48);
    }

    uint16_t getURSolved() {
        return ((m_facesSolved[0] & 0xFF00000000) >> 24) | ((m_facesSolved[3] & 0xFF000000000000) >> 48);
    }

    uint16_t getUBSolved() {
        return ((m_facesSolved[0] & 0xFF000000000000) >> 40) | ((m_facesSolved[2] & 0xFF000000000000) >> 48);
    }

    uint16_t getULSolved() {
        return ((m_facesSolved[0] & 0xFF) << 8) | ((m_facesSolved[1] & 0xFF000000000000) >> 48);
    }

    uint16_t getDFSolved() {
        return ((m_facesSolved[5] & 0xFF000000000000) >> 40) | ((m_facesSolved[4] & 0xFF0000) >> 16);
    }

    uint16_t getDRSolved() {
        return ((m_facesSolved[5] & 0xFF00000000) >> 24) | ((m_facesSolved[3] & 0xFF0000) >> 16);
    }

    uint16_t getDBSolved() {
        return ((m_facesSolved[5] & 0xFF0000) >> 8) | ((m_facesSolved[2] & 0xFF0000) >> 16);
    }

    uint16_t getDLSolved() {
        return ((m_facesSolved[5] & 0xFF) << 8) | ((m_facesSolved[1] & 0xFF0000) >> 16);
    }

    uint16_t getFLSolved() {
        return ((m_facesSolved[4] & 0xFF) << 8) | ((m_facesSolved[1] & 0xFF00000000) >> 32);
    }

    uint16_t getFRSolved() {
        return ((m_facesSolved[4] & 0xFF00000000) >> 24) | (m_facesSolved[3] & 0xFF);
    }

    uint16_t getBRSolved() {
        return ((m_facesSolved[2] & 0xFF) << 8) | ((m_facesSolved[3] & 0xFF00000000) >> 32);
    }

    uint16_t getBLSolved() {
        return ((m_facesSolved[2] & 0xFF00000000) >> 24) | (m_facesSolved[1] & 0xFF);
    }



    inline uint32_t rotateCorner(uint32_t corner) {
       // std::cout << "\n original: ";
       // displayHelper(static_cast<uint64_t>(corner));
        uint64_t a = corner & 0xFF0000;
        uint64_t b = corner & 0x00FF00;
        uint64_t c = corner & 0x0000FF;
        a >>= 8;
        b >>= 8;
        c <<= 16;
        //std::cout << "    rotated: ";
        //displayHelper(static_cast<uint64_t>(a | b | c));
        return a | b | c;
    }

    inline uint32_t swapCornerState(uint32_t corner) {
        // std::cout << "\n original: ";
        // displayHelper(static_cast<uint64_t>(corner));
        uint64_t a = corner & 0xFF0000;
        uint64_t b = corner & 0x00FF00;
        uint64_t c = corner & 0x0000FF;
        a >>= 16;
        c <<= 16;
        //std::cout << "    rotated: ";
        //displayHelper(static_cast<uint64_t>(a | b | c));
        return a | b | c;
    }

    uint32_t getUFL() {
        return ((m_faces[0] & 0xFF00) << 8) | ((m_faces[4] & 0xFF00000000000000) >> 48) | ((m_faces[1] & 0xFF0000000000) >> 40);
    }

    uint32_t getUFR() {
        return ((m_faces[0] & 0xFF000000) >> 8) | ((m_faces[4] & 0xFF0000000000) >> 32) | ((m_faces[3] & 0xFF00000000000000) >> 56);
    }

    uint32_t getDFL() {
        return ((m_faces[5] & 0xFF00000000000000) >> 40) | ((m_faces[4] & 0xFF00)) | ((m_faces[1] & 0xFF000000) >> 24);
    }

    uint32_t getDFR() {
        return ((m_faces[5] & 0xFF0000000000) >> 24) | ((m_faces[4] & 0xFF000000) >> 16) | ((m_faces[3] & 0xFF00) >> 8);
    }

    uint32_t getUBL() {
        return ((m_faces[0] & 0xFF00000000000000) >> 40) | ((m_faces[2] & 0xFF0000000000) >> 32) | ((m_faces[1] & 0xFF00000000000000) >> 56);
    }

    uint32_t getUBR() {
        return ((m_faces[0] & 0xFF0000000000) >> 24) | ((m_faces[2] & 0xFF00000000000000) >> 48) | ((m_faces[3] & 0xFF0000000000) >> 40);
    }

    uint32_t getDBL() {
        return ((m_faces[5] & 0xFF00) << 8) | ((m_faces[2] & 0xFF000000) >> 16) | ((m_faces[1] & 0xFF00) >> 8);
    }

    uint32_t getDBR() {
        return ((m_faces[5] & 0xFF000000) >> 8) | ((m_faces[2] & 0xFF00)) | ((m_faces[3] & 0xFF000000) >> 24);
    }

    uint32_t getUFLSolved() {
        return ((m_facesSolved[0] & 0xFF00) << 8) | ((m_facesSolved[4] & 0xFF00000000000000) >> 48) | ((m_facesSolved[1] & 0xFF0000000000) >> 40);
    }

    uint32_t getUFRSolved() {
        return ((m_facesSolved[0] & 0xFF000000) >> 8) | ((m_facesSolved[4] & 0xFF0000000000) >> 32) | ((m_facesSolved[3] & 0xFF00000000000000) >> 56);
    }

    uint32_t getDFLSolved() {
        return ((m_facesSolved[5] & 0xFF00000000000000) >> 40) | ((m_facesSolved[4] & 0xFF00)) | ((m_facesSolved[1] & 0xFF000000) >> 24);
    }

    uint32_t getDFRSolved() {
        return ((m_facesSolved[5] & 0xFF0000000000) >> 24) | ((m_facesSolved[4] & 0xFF000000) >> 16) | ((m_facesSolved[3] & 0xFF00) >> 8);
    }

    uint32_t getUBLSolved() {
        return ((m_facesSolved[0] & 0xFF00000000000000) >> 40) | ((m_facesSolved[2] & 0xFF0000000000) >> 32) | ((m_facesSolved[1] & 0xFF00000000000000) >> 56);
    }

    uint32_t getUBRSolved() {
        return ((m_facesSolved[0] & 0xFF0000000000) >> 24) | ((m_facesSolved[2] & 0xFF00000000000000) >> 48) | ((m_facesSolved[3] & 0xFF0000000000) >> 40);
    }

    uint32_t getDBLSolved() {
        return ((m_facesSolved[5] & 0xFF00) << 8) | ((m_facesSolved[2] & 0xFF000000) >> 16) | ((m_facesSolved[1] & 0xFF00) >> 8);
    }

    uint32_t getDBRSolved() {
        return ((m_facesSolved[5] & 0xFF000000) >> 8) | ((m_facesSolved[2] & 0xFF00)) | ((m_facesSolved[3] & 0xFF000000) >> 24);
    }



};