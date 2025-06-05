#pragma once

#include <bitwiseCube.hpp>
#include <unordered_map>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <queue>
#include <vector>
#include <array>
#include <string>

class Solver {
public:
	Solver() = default;

	void solve(BitwiseCube cube, std::string scramble) {
		m_solution = cross(cube);
		cube.applyMoves(m_solution);

		
		m_f2lSolution = F2L(cube);
		/*m_f2lSolution = pair(cube, true, false, false, false);
		
		m_solution += m_f2lSolution;
		cube.applyMoves(m_f2lSolution);

		m_f2lSolution = pair(cube, true, true, false, false);
		cube.applyMoves(m_f2lSolution); */
		m_solution += " \n\n" + m_f2lSolution;

		m_OLLSolution = OLL(cube);
		m_solution += " \n " + m_OLLSolution;
		cube.applyMoves(m_OLLSolution);

		m_PLLSolution = PLL(cube);
		m_solution += " \n " + m_PLLSolution;
		cube.applyMoves(m_PLLSolution);

		std::cout << "\nmove count: " << getMoveCount();
		writeToFile(scramble);
		//openInNotepad();

	}

	std::string getSolution() {
		return m_solution;
	}

	std::string getCross() {
		return m_crossSolution;
	}
	std::string getF2L() {
		return m_f2lSolution;
	}
	std::string getOLL() {
		return m_OLLSolution;
	}
	std::string getPLL() {
		return m_OLLSolution;
	}

	int getMoveCount() {
		int counter = 0;
		char lastChar = ' ';
		std::array<char, 8> valid = { '2', 'p', 'U', 'D', 'F', 'B', 'R', 'L' };
		for (char c : m_solution) {
			if (c == ' ') {
				if (contains(valid, lastChar)) {
					counter++;
				}
			}
			lastChar = c;
		}
		if (contains(valid, lastChar)) {
			counter++;
		}
		return counter;
	}


	void openInNotepad(std::string filename = "solution.txt") {
		// Construct the command to open the file in Notepad
		std::string path = RESOURCES_PATH + filename;
		std::string command = "start notepad " + path;

		// Execute the command using system()
		int result = system(command.c_str());

		if (result == 0) {
			std::cout << "Notepad opened successfully!" << std::endl;
		}
		else {
			std::cerr << "Failed to open Notepad." << std::endl;
		}
	}

private:
	void writeToFile(std::string& scramble, std::string filename = "solution.txt") {
		std::ofstream outFile(RESOURCES_PATH + filename);
		if (outFile.is_open()) {
			outFile << scramble + "\n\nSolution: \n" + m_solution;  // Write the content to the file
			outFile.close();
			std::cout << "Content written " << std::endl;
		}
		else {
			std::cerr << "Failed to open file for writing." << std::endl;
		}
	}

	bool contains(std::array<char, 8> arr, char value) {
		for (int i = 0; i < 8; ++i) {
			if (arr[i] == value) {
				return true;  // Character found
			}
		}
		return false;  // Character not found
	}

	std::string cross(BitwiseCube cube) {
		std::string crossSolution;


		

		std::unordered_map<uint64_t, std::string> forwardMap;
		std::unordered_map<uint64_t, std::string> backwardMap;

		BitwiseCube solvedCube;
		solvedCube.reset(cube);

		if (cube.getCrossState() == solvedCube.getCrossState()) return "";

		uint64_t targetState = solvedCube.getCrossState();
		uint64_t initialState = cube.getCrossState();

		std::queue<std::pair<BitwiseCube, std::string>> forwardQueue;
		std::queue<std::pair<BitwiseCube, std::string>> backwardQueue;

		forwardQueue.push({ cube, "" });
		backwardQueue.push({ solvedCube, "" });

		forwardMap[initialState] = "";
		backwardMap[targetState] = "";

		std::array<std::string, 18> moves = { "U", "Up", "U2", "D", "Dp", "D2", "L", "Lp", "L2", "R", "Rp", "R2", "F", "Fp", "F2", "B", "Bp", "B2" };

		int maxDepth = 4;

		for (int depth = 0; depth < maxDepth; depth++) {

			int fQueueSize = forwardQueue.size();
			for (int i = 0; i < fQueueSize; i++) {
				BitwiseCube currentCube = forwardQueue.front().first;
				std::string currentMoveSequence = forwardQueue.front().second;
				forwardQueue.pop();

				for (std::string& move : moves) {
					BitwiseCube tempCube = currentCube;
					applyMove(tempCube, move);
					uint64_t crossState = tempCube.getCrossState();
					if (forwardMap.count(crossState)) { //state already in map
						continue;
					}
					forwardMap[crossState] = currentMoveSequence + " " + move;
					forwardQueue.push({ tempCube, currentMoveSequence + " " + move });

					if (backwardMap.count(crossState)) {
						crossSolution = forwardMap[crossState] + " " + reverseSequence(backwardMap[crossState]);
						return crossSolution;
					}
				}

			}


			int bQueueSize = backwardQueue.size();
			for (int j = 0; j < bQueueSize; j++) {
				BitwiseCube currentCube = backwardQueue.front().first;
				std::string currentMoveSequence = backwardQueue.front().second;
				backwardQueue.pop();

				for (std::string& move : moves) {
					BitwiseCube tempCube = currentCube;
					applyMove(tempCube, move);
					uint64_t crossState = tempCube.getCrossState();
					if (backwardMap.count(crossState)) { //state already in map
						continue;
					}
					backwardMap[crossState] = currentMoveSequence + " " + move;
					backwardQueue.push({ tempCube, currentMoveSequence + " " + move });

					if (forwardMap.count(crossState)) {
						crossSolution = forwardMap[crossState] + " " + reverseSequence(backwardMap[crossState]);
						return crossSolution;
					}
				}

			}
		}

		return "";
		

	}

	std::string F2L(BitwiseCube& cube) {


		std::string fullSolution;
		bool solvedFR = false, solvedFL = false, solvedBL = false, solvedBR = false;


		std::string solution;
		int bestSolutionCount = 100;
		std::string bestSolution = "";
		int bestSolutionIndex = 0;
		for (int i = 0; i < 4; i++) {
			solution = "";
			bestSolutionCount = 100;
			bestSolution = "";
			bestSolutionIndex = 0;

			for (int j = 0; j < 4; j++) {

				//skip already solved ones
				if ((solvedFR && j == 0) || (solvedFL && j == 1) || (solvedBL && j == 2) || (solvedBR && j == 3)) {
					continue;
				}

				bool tempFR = solvedFR || j == 0;
				bool tempFL = solvedFL || j == 1;
				bool tempBL = solvedBL || j == 2;
				bool tempBR = solvedBR || j == 3;

				solution = pair(cube, tempFR, tempFL, tempBL, tempBR);
				if (getMoveCount(solution) < bestSolutionCount) {
					bestSolutionCount = getMoveCount(solution);
					bestSolution = solution;
					bestSolutionIndex = j;
				}

			}
			cube.applyMoves(bestSolution);
			fullSolution += bestSolution + " \n";





			switch (bestSolutionIndex) {
			case 0:
				solvedFR = true;
				break;
			case 1:
				solvedFL = true;
				break;
			case 2:
				solvedBL = true;
				break;
			case 3:
				solvedBR = true;
				break;
			default:
				break;
			}
		}

		//return fullSolution;

		if (bestSolutionCount > 10){
			if (!solvedFR) {
				fullSolution += "R U Rp ";
				solution = pair(cube, false, true, true, true);
				fullSolution += solution + " \n ";
			}
			else if (!solvedFL) {
				fullSolution += "Lp U' L ";
				solution = pair(cube, true, false, true, true);
				fullSolution += solution + " \n ";
			}
			else if (!solvedBL) {
				fullSolution += "L U Lp ";
				solution = pair(cube, true, true, false, true);
				fullSolution += solution + " \n ";
			}
			else if (!solvedBR) {
				fullSolution += "Rp U' R ";
				solution = pair(cube, true, true, true, false);
				fullSolution += solution + " \n ";
			}
		}


		return fullSolution;
	}

	std::string OLL(BitwiseCube cube) {

		if (cube.getOLLSolved()) return " ";

		std::vector<std::string> algs = loadMovesFromFile(RESOURCES_PATH"OLL.txt");
		std::array<std::string, 4> Us = { "", "U", "Up", "U2" };

		for (std::string& alg : algs) {
			for (std::string move : Us) {
				BitwiseCube workingCube = cube;
				workingCube.applyMoves(move + " " + alg);
				if (workingCube.getOLLSolved()) return move + " " + alg;
			}
			
		}

		return " ";

	}

	std::string PLL(BitwiseCube cube) {

		if (cube.isSolved()) return " ";

		std::vector<std::string> algs = loadMovesFromFile(RESOURCES_PATH"PLL.txt");
		algs.push_back(" ");
		std::array<std::string, 4> Us = { "", "U", "Up", "U2" };

		for (std::string& alg : algs) {
			for (std::string moveA : Us) {
				for (std::string move : Us) {
					BitwiseCube workingCube = cube;
					workingCube.applyMoves(move + " " + alg + " " + moveA);
					if (workingCube.isSolved()) return move + " " + alg + " " + moveA;
				}
			}
			

		}

		return " ";

	}

	std::string pair(BitwiseCube cube, bool FR, bool FL, bool BL, bool BR) {

		std::string fpSolution;

		std::unordered_map<uint64_t, std::string> forwardMap;
		std::unordered_map<uint64_t, std::string> backwardMap;

		BitwiseCube solvedCube;
		solvedCube.reset(cube);

		if (cube.getStatePair(FR, FL, BL, BR) == solvedCube.getStatePair(FR, FL, BL, BR)) { return "SOLVED"; }

		uint64_t targetState = solvedCube.getStatePair(FR, FL, BL, BR);
		uint64_t initialState = cube.getStatePair(FR, FL, BL, BR);

		std::queue<std::pair<BitwiseCube, std::string>> forwardQueue;
		std::queue<std::pair<BitwiseCube, std::string>> backwardQueue;

		forwardQueue.push({ cube, "" });
		backwardQueue.push({ solvedCube, "" });

		forwardMap[initialState] = "";
		backwardMap[targetState] = "";

		//
		std::array<std::string, 15> moves = { "U", "Up", "U2", "L", "Lp", "L2", "R", "Rp", "R2", "F", "Fp", "F2", "B", "Bp", "B2"};

		int maxDepth = 4;

		for (int depth = 0; depth < maxDepth; depth++) {

			int fQueueSize = forwardQueue.size();
			for (int i = 0; i < fQueueSize; i++) {
				BitwiseCube currentCube = forwardQueue.front().first;
				std::string currentMoveSequence = forwardQueue.front().second;
				forwardQueue.pop();

				for (std::string& move : moves) {
					BitwiseCube tempCube = currentCube;
					applyMove(tempCube, move);
					uint64_t pairState = tempCube.getStatePair(FR, FL, BL, BR);
					if (forwardMap.count(pairState)) { //state already in map
						continue;
					}
					forwardMap[pairState] = currentMoveSequence + " " + move;
					forwardQueue.push({ tempCube, currentMoveSequence + " " + move });

					if (backwardMap.count(pairState)) {
						fpSolution = forwardMap[pairState] + " " + reverseSequence(backwardMap[pairState]);
						return fpSolution;
					}
				}

			}


			int bQueueSize = backwardQueue.size();
			for (int j = 0; j < bQueueSize; j++) {
				BitwiseCube currentCube = backwardQueue.front().first;
				std::string currentMoveSequence = backwardQueue.front().second;
				backwardQueue.pop();

				for (std::string& move : moves) {
					BitwiseCube tempCube = currentCube;
					applyMove(tempCube, move);
					uint64_t pairState = tempCube.getStatePair(FR, FL, BL, BR);
					if (backwardMap.count(pairState)) { //state already in map
						//std::cout << "ex";
						continue;
					}
					backwardMap[pairState] = currentMoveSequence + " " + move;
					backwardQueue.push({ tempCube, currentMoveSequence + " " + move });

					if (forwardMap.count(pairState)) {
						fpSolution = forwardMap[pairState] + " " + reverseSequence(backwardMap[pairState]);
						return fpSolution;
					}
				}

			}
		}

		return "";
	}



private:
	std::string m_solution = "";
	std::string m_crossSolution = "";
	std::string m_f2lSolution = "";
	std::string m_OLLSolution = "";
	std::string m_PLLSolution = "";

	void applyMove(BitwiseCube& cube, const std::string& move) {
		if (move == "U") cube.U();
		else if (move == "Up") cube.Up();
		else if (move == "U2") cube.U2();
		else if (move == "D") cube.D();
		else if (move == "Dp") cube.Dp();
		else if (move == "D2") cube.D2();
		else if (move == "L") cube.L();
		else if (move == "Lp") cube.Lp();
		else if (move == "L2") cube.L2();
		else if (move == "R") cube.R();
		else if (move == "Rp") cube.Rp();
		else if (move == "R2") cube.R2();
		else if (move == "F") cube.F();
		else if (move == "Fp") cube.Fp();
		else if (move == "F2") cube.F2();
		else if (move == "B") cube.B();
		else if (move == "Bp") cube.Bp();
		else if (move == "B2") cube.B2();
		else if (move == "Y") cube.Y();
		else if (move == "Yp") cube.Yp();
	}

	void applyMoves(BitwiseCube& cube, const std::string& moves) {
		std::string currentMove = "";
		for (char c : moves) {
			if (c == ' ') {
				applyMove(cube, currentMove);
				currentMove = "";
			}
			else {
				currentMove += c;
			}

		}
		applyMove(cube, currentMove);
	}

	int getMoveCount(std::string& moves) {
		if (moves == "SOLVED") { return 1; }
		int count = 0;
		for (char c : moves) {
			if (c == ' ') count++;
		}
		count++;

		if (count == 1) return 100;
		return count;
	}

	std::string invertMove(const std::string& move) {
		if (move.back() == 'p') return move.substr(0, move.size() - 1); // e.g. "Fp" -> "F"
		if (move.back() == '2') return move;                            // "F2" stays "F2"
		return move + "p";                                              // "F" -> "Fp"
	}

	std::string reverseSequence(const std::string& seq) {
		std::istringstream ss(seq);
		std::string move;
		std::vector<std::string> tokens;

		while (ss >> move) {
			tokens.push_back(move);
		}

		std::string result;
		for (auto it = tokens.rbegin(); it != tokens.rend(); ++it) {
			if (!result.empty()) result += " ";
			result += invertMove(*it);
		}

		return result;
	}

	std::vector<std::string> loadMovesFromFile(const std::string& filename) {
		std::vector<std::string> moves;
		std::ifstream file(filename);

		if (!file.is_open()) {
			std::cerr << "Failed to open file: " << filename << std::endl;
			return moves;
		}

		std::string line;
		while (std::getline(file, line)) {
			// Ignore empty lines and lines with just spaces
			if (!line.empty() && line.find_first_not_of(' ') != std::string::npos) {
				moves.push_back(line);
			}
		}

		file.close();
		return moves;
	}


	



	

};
