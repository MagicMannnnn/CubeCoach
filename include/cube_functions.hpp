#pragma once

#include <random.hpp>
#include <string>
#include <visualCube.hpp>
#include <bitwiseCube.hpp>





class CubeFunctions {
public:
	CubeFunctions(const Random&) = delete;

	static CubeFunctions& get() {
		static CubeFunctions instance;
		return instance;
	}

	static std::string scramble(VisualCube& visualCube, VisualCube& solvedCube, BitwiseCube& bitwiseCube, int length=20) {
		return get().Iscramble(visualCube, solvedCube, bitwiseCube, length);
	}

	
private:
	CubeFunctions() {  }

	bool redo(char m1, char& m2) {

		if (m1 == m2) return true;

		switch (m1)
		{
		case 'U':
			if (m2 == 'D') return true;
			break;
		case 'D':
			if (m2 == 'U') return true;
			break;

		case 'R':
			if (m2 == 'L') return true;
			break;
		case 'L':
			if (m2 == 'R') return true;
			break;

		case 'F':
			if (m2 == 'B') return true;
			break;
		case 'B':
			if (m2 == 'F') return true;
			break;


		default:
			break;
		}

		return false;
	}

	std::string Iscramble(VisualCube& visualCube, VisualCube& solvedCube, BitwiseCube& bitwiseCube, int length) {
		bitwiseCube.reset(true);
		visualCube = solvedCube;

		std::string scramble = "";

		std::array<std::string, 18> moves = { "U", "Up", "U2", "D", "Dp", "D2", "L", "Lp", "L2", "R", "Rp", "R2", "F", "Fp", "F2", "B", "Bp", "B2" };
		std::string prevMove = "Z";
		for (int i = 0; i < length; i++) {
			std::string move = moves[Random::getRangeInt(0, 17)];
			while (redo(move[0], prevMove[0])) {
				move = moves[Random::getRangeInt(0, 17)];
			}
			prevMove = move;
			scramble += move + " ";
			bitwiseCube.applyMove(move);
			visualCube.applyMove(move);
		}
		for (char& c : scramble) {
			if (c == 'p') c = '\'';
		}

		return "Scramble: " + scramble;


	}


};