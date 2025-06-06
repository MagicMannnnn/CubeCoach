#pragma once


#include <app_input.hpp>
#include <Shader.h>
#include <visualCube.hpp>
#include <bitwiseCube.hpp>
#include <solver.hpp>
#include <random.hpp>
#include <menu.hpp>
#include <cube_functions.hpp>
#include <algorithm>



//TODO
/*
* create menu button icon which toggles all menu buttons
* create a settings text file which gets the settings and can update on button click from menu
* can open all these files from within the menu
* create a scramble text file where paste in scramble and click button to scramble using that scramble
*/





class Default {
public:
	Default() :

		m_shader(RESOURCES_PATH"shaders/piece.vert", RESOURCES_PATH"shaders/piece.frag"),
		m_cube(),
		m_solvedCube(),
		m_bitwiseCube(),
		m_menu(),
		m_currentMoveRenderer(RESOURCES_PATH"fonts/font1.ttf", 24, RESOURCES_PATH"shaders/text.vert", RESOURCES_PATH"shaders/text.frag")
	{
		m_menu.setPos(UI::TEXTS::CROSS, 20, Settings::SCR_HEIGHT / 2);
		m_menu.setPos(UI::TEXTS::SCRAMBLE, Settings::SCR_WIDTH / 2 - 300, Settings::SCR_HEIGHT / 2 + 350);
	}


	void updateSettings() {
		Settings::updateSettings();
		if (Settings::MOVE_TIME == 0) {
			m_cube.fast = true;
			m_solvedCube.fast = true;
		}
		else {
			m_cube.rotationTime = Settings::MOVE_TIME;
			m_cube.fast = false;
			m_solvedCube.rotationTime = Settings::MOVE_TIME;
			m_solvedCube.fast = false;
		}
		//std::cout << "\n Move Time of CUbe: " << m_cube.rotationTime;

	}

	~Default() {
		m_steppingMoves.clear();
	}

	void run() {

		/*m_bitwiseCube.displayHex();*/
		//m_bitwiseCube.U2();
		///*m_bitwiseCube.displayHex();*/
		//m_bitwiseCube.displayColours();

		updateSettings();

		float lastFrame = static_cast<float>(glfwGetTime());
		float currentFrame;


		int i = 0;
		while (static_cast<float>(glfwGetTime()) - lastFrame < 0) { // change to 1 for testing
			m_bitwiseCube.U();
			m_bitwiseCube.Up();
			m_bitwiseCube.U2();
			m_bitwiseCube.D();
			m_bitwiseCube.Dp();
			m_bitwiseCube.D2();
			m_bitwiseCube.F();
			m_bitwiseCube.Fp();
			m_bitwiseCube.F2();
			m_bitwiseCube.B();
			m_bitwiseCube.Bp();
			m_bitwiseCube.B2();
			m_bitwiseCube.L();
			m_bitwiseCube.Lp();
			m_bitwiseCube.L2();
			m_bitwiseCube.R();
			m_bitwiseCube.Rp();
			m_bitwiseCube.R2();
			i++;
		}

		m_bitwiseCube.reset();

		//std::cout << "moves in 1 seconds: " <<  i * 18 << "  , move time: " << 1000000000.f / (float)(i * 18) << " ns";

		//ScrambleCube();
		//m_bitwiseCube.getCrossState();


		while (!glfwWindowShouldClose(m_window)) {
			currentFrame = static_cast<float>(glfwGetTime());
			Global::deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;


			frameLoop();

			Global::processInput(m_window);

			glClearColor(Settings::BG_COLOUR.x / 255.f, Settings::BG_COLOUR.y / 255.f, Settings::BG_COLOUR.z / 255.f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			m_shader.use();

			m_projection = glm::perspective(glm::radians(m_camera.Zoom), (float)Settings::SCR_WIDTH / (float)Settings::SCR_HEIGHT, 0.1f, 500.0f);

			m_shader.setMat4("view", m_camera.GetViewMatrix());
			m_shader.setMat4("projection", m_projection);

			m_cube.draw(m_shader, m_camera);

			m_menu.draw();

			if (m_stepping) {
				m_currentMoveRenderer.RenderText(m_currentMoveText, Settings::SCR_WIDTH / 2 - 7, 75, 1, glm::vec3(1, 1, 1));
			}

			glfwSwapBuffers(m_window);
			glfwPollEvents();


		}

	}




private:

	void frameLoop() {

		m_menu.update();

		if (m_menu.getButtonClicked(UI::SCRAMBLE_BUTTON)) {
			m_menu.setButtonClicked(UI::SCRAMBLE_BUTTON, false);
			ScrambleCube();
		}
		else if (m_menu.getButtonClicked(UI::SOLVE)) {
			m_menu.setButtonClicked(UI::SOLVE, false);
			solve();
		}
		else if (m_menu.getButtonClicked(UI::RESETSETTINGS)) {
			m_menu.setButtonClicked(UI::RESETSETTINGS, false);
			Settings::resetSettings();
			Settings::MOVE_TIME = 0.1;
			m_menu.setText(UI::TURNSPEED, "10");
			m_menu.setText(UI::SCRAMBLELENGTH, "20");
			m_scrambleLength = 20;
			m_solvedCube.rotationTime = Settings::MOVE_TIME;
			m_cube.rotationTime = Settings::MOVE_TIME;
		}
	


		if (Global::typing) {
			bool typed = m_menu.handleCharacterInput(Global::codepoint);
			Global::typing = false;
			if (typed) {
				Global::processInput(m_window);


				updateMenu();

				return;
			}
		}
		if (Global::backspace) {
			m_menu.backspacePressed();
			Global::processInput(m_window);

			updateMenu();


			return;
		}




		if (Global::leftClick) {
			if (Global::firstLeftClick) {
				m_cube.startDrag();  // Store initial cube state
				Global::firstLeftClick = false;
			}

			m_cube.rotate(100 * Global::mouseDeltaX * Global::deltaTime, glm::vec3(0, 1, 0), true, false);
			m_cube.rotate(-100 * Global::mouseDeltaY * Global::deltaTime, glm::vec3(1, 0, 0), true, false);
		}
		else {
			Global::firstLeftClick = true;  // Reset for next drag
		}

		

		if (Global::rightClick) {
			m_cube.resetOrientation();
		}

		VisualCubeMoves();

		m_cube.update(Global::deltaTime);
		m_solvedCube.update(Global::deltaTime);



		Global::mouseDeltaX = 0;
		Global::mouseDeltaY = 0;

		if (m_stepping) {
			if (Global::LEFT) {

				if (m_stepPointer == 0) return;
				
				m_stepPointer--;

				std::string move = invertMove(m_steppingMoves[m_stepPointer]);
				m_currentMoveText = move;
				if (m_currentMoveText.back() == 'p') {
					m_currentMoveText.pop_back();
					m_currentMoveText += "'";
				}
				m_bitwiseCube.applyMove(move);
				m_cube.applyMove(move);
				
			}
			else if (Global::RIGHT) {

				if (m_stepPointer == m_steppingMoves.size()) {
					m_solving = false;
					m_stepping = false;
					return;
				}

				m_currentMoveText = m_steppingMoves[m_stepPointer];
				if (m_currentMoveText.back() == 'p') {
					m_currentMoveText.pop_back();
					m_currentMoveText += "'";
				}
				m_bitwiseCube.applyMove(m_steppingMoves[m_stepPointer]);
				m_cube.applyMove(m_steppingMoves[m_stepPointer]);
				m_stepPointer++;
			}

			return;
		}


		if (m_bitwiseCube.isSolved()) {
			m_solving = false;
		}

		

	}


	void updateMenu() {
		float tps = m_menu.getTextboxInput(UI::TURNSPEED);
		tps = tps == -1 ? 10 : tps; //if bad input, set to 10
		tps = std::min(tps, 1000.f);
		if (tps == 1000) m_menu.setText(UI::TURNSPEED, "1000");
		Settings::MOVE_TIME = 1.f / tps;
		m_solvedCube.rotationTime = Settings::MOVE_TIME;
		m_cube.rotationTime = Settings::MOVE_TIME;

		int length = m_menu.getTextboxInput(UI::SCRAMBLELENGTH);
		length = length == -1 ? 20 : length;
		length = std::min(length, 100);
		if (length == 100) m_menu.setText(UI::SCRAMBLELENGTH, "100");
		m_scrambleLength = length;
	}

	void solve() {
		double time = glfwGetTime();
		m_solver.solve(m_bitwiseCube, m_scramble);
		std::cout << "\nsolve time: " << glfwGetTime() - time << "s";
		std::cout << "\nCross Solution: " << m_solver.getSolution() << "\n";
		std::string solution = m_solver.getSolution();
		for (char& c : solution) {
			if (c == 'p') c = '\'';
		}
		m_menu.setText(UI::TEXTS::CROSS, solution);

		if (m_menu.getButtonClicked(UI::ARROWS)) {
			m_stepping = true;
			m_stepPointer = 0;
			m_currentMoveText = "";
			createSteppingVector();
		}
		else {
			m_bitwiseCube.applyMoves(m_solver.getSolution());
			m_cube.applyMoves(m_solver.getSolution());
		}

		
	}

	std::string invertMove(const std::string& move) {
		if (move.back() == 'p') return move.substr(0, move.size() - 1); // e.g. "Fp" -> "F"
		if (move.back() == '2') return move;                            // "F2" stays "F2"
		return move + "p";                                              // "F" -> "Fp"
	}


	void createSteppingVector() {
		m_steppingMoves.clear();
		std::array<std::string, 18> moves = { "U", "Up", "U2", "D", "Dp", "D2", "L", "Lp", "L2", "R", "Rp", "R2", "F", "Fp", "F2", "B", "Bp", "B2" };
		std::string currentMove = "";
		for (char c : m_solver.getSolution()) {
			if (c == ' ') {
				auto it = std::find(moves.begin(), moves.end(), currentMove);
				if (it != moves.end()) m_steppingMoves.push_back(currentMove);
				currentMove = "";
			}
			else {
				currentMove += c;
			}

		}
		auto it = std::find(moves.begin(), moves.end(), currentMove);
		if (it != moves.end()) m_steppingMoves.push_back(currentMove);
	}

	


	void VisualCubeMoves() {


		if (Global::solve) {
			solve();
		}

		if (Global::nine) {
			m_bitwiseCube.reset();
			m_cube = m_solvedCube;
			m_scramble = "Scramble: ";
			applyScrambleText();
			m_stepping = false;
		}

		if (Global::zero) {
			ScrambleCube();
			m_stepping = false;
		}



		if (m_stepping) return;

		// Handle U moves
		if (Global::U) { m_cube.U(); m_bitwiseCube.U(); addToScramble("U"); /*m_bitwiseCube.displayHex();*/ }
		if (Global::Up) { m_cube.Up(); m_bitwiseCube.Up(); addToScramble("U'"); /*m_bitwiseCube.displayHex();*/ }
		if (Global::U2) { m_cube.U2(); m_bitwiseCube.U2(); addToScramble("U2"); /*m_bitwiseCube.displayHex();*/ }

		// Handle D moves
		if (Global::D) { m_cube.D(); m_bitwiseCube.D(); addToScramble("D"); /*m_bitwiseCube.displayHex();*/ }
		if (Global::Dp) { m_cube.Dp(); m_bitwiseCube.Dp(); addToScramble("D'"); /*m_bitwiseCube.displayHex();*/ }
		if (Global::D2) { m_cube.D2(); m_bitwiseCube.D2(); addToScramble("D2"); /*m_bitwiseCube.displayHex();*/ }

		// Handle L moves
		if (Global::L) { m_cube.L(); m_bitwiseCube.L(); addToScramble("L"); /*m_bitwiseCube.displayHex();*/ }
		if (Global::Lp) { m_cube.Lp(); m_bitwiseCube.Lp(); addToScramble("L'"); /*m_bitwiseCube.displayHex();*/ }
		if (Global::L2) { m_cube.L2(); m_bitwiseCube.L2(); addToScramble("L2"); /*m_bitwiseCube.displayHex();*/ }

		// Handle R moves
		if (Global::R) { m_cube.R(); m_bitwiseCube.R(); addToScramble("R"); /*m_bitwiseCube.displayHex();*/ }
		if (Global::Rp) { m_cube.Rp(); m_bitwiseCube.Rp(); addToScramble("R'"); /*m_bitwiseCube.displayHex();*/ }
		if (Global::R2) { m_cube.R2(); m_bitwiseCube.R2(); addToScramble("R2"); /*m_bitwiseCube.displayHex();*/ }

		// Handle F moves
		if (Global::F) { m_cube.F(); m_bitwiseCube.F(); addToScramble("F"); /*m_bitwiseCube.displayHex();*/ }
		if (Global::Fp) { m_cube.Fp(); m_bitwiseCube.Fp(); addToScramble("F'"); /*m_bitwiseCube.displayHex();*/ }
		if (Global::F2) { m_cube.F2(); m_bitwiseCube.F2(); addToScramble("F2"); /*m_bitwiseCube.displayHex();*/ }

		// Handle B moves
		if (Global::B) { m_cube.B(); m_bitwiseCube.B(); addToScramble("B"); /*m_bitwiseCube.displayHex();*/ }
		if (Global::Bp) { m_cube.Bp(); m_bitwiseCube.Bp(); addToScramble("B'"); /*m_bitwiseCube.displayHex();*/ }
		if (Global::B2) { m_cube.B2(); m_bitwiseCube.B2(); addToScramble("B2"); /*m_bitwiseCube.displayHex();*/ }

		// Handle M moves
		if (Global::M) { m_cube.M(); m_bitwiseCube.M(); addToScramble("M"); /*m_bitwiseCube.displayHex();*/ }
		if (Global::Mp) { m_cube.Mp(); m_bitwiseCube.Mp(); addToScramble("M'"); /*m_bitwiseCube.displayHex();*/ }
		if (Global::M2) { m_cube.M2(); m_bitwiseCube.M2(); addToScramble("M2"); /*m_bitwiseCube.displayHex();*/ }

		// Handle Rotations
		if (Global::RIGHT) { m_cube.Yp(); /*m_solvedCube.Yp();*/ m_bitwiseCube.Yp(); addToScramble("y'"); /*m_bitwiseCube.displayHex();*/ }
		if (Global::LEFT) { m_cube.Y(); /*m_solvedCube.Y(); */m_bitwiseCube.Y(); addToScramble("y"); /*m_bitwiseCube.displayHex();*/ }
		if (Global::UP) { m_cube.X(); /*m_solvedCube.X(); */m_bitwiseCube.X(); addToScramble("x"); /*m_bitwiseCube.displayHex();*/ }
		if (Global::DOWN) { m_cube.Xp(); /*m_solvedCube.Xp();*/ m_bitwiseCube.Xp(); addToScramble("x'"); /*m_bitwiseCube.displayHex();*/ }





	}


	
	void ScrambleCube() {

		m_scramble = CubeFunctions::scramble(m_cube, m_solvedCube, m_bitwiseCube, m_scrambleLength);
		
		applyScrambleText();


		if (m_autoRotateAfterScramble) {
			m_cube.Y(); /*m_solvedCube.Y();*/ m_bitwiseCube.Y();
			m_cube.Y(); /*m_solvedCube.Y();*/ m_bitwiseCube.Y(); addToScramble("y2");
			m_cube.X(); /*m_solvedCube.X();*/ m_bitwiseCube.X();
			m_cube.X(); /*m_solvedCube.X();*/ m_bitwiseCube.X(); addToScramble("x2");
		}


		m_solving = true;

	}

	void applyScrambleText() {

		int i = 0;
		bool newLine = false;
		std::string res;
		for (char c : m_scramble) {
			res += c;

			i++;
			if (i % 100 == 0) {
				newLine = true;

			}
			if (newLine && c == ' ') {
				res += "\n";
				newLine = false;
			}
		}


		m_menu.setText(UI::TEXTS::SCRAMBLE, res);
	}

	

	void addToScramble(std::string move) {

		if (m_solving) return;


		m_scramble += move + " ";
		//std::string lastMove = getLastMove();




		applyScrambleText();
	}



private:
	Camera& m_camera = Global::camera;
	GLFWwindow*& m_window = Global::window;
	Shader m_shader;
	glm::mat4 m_projection = glm::perspective(glm::radians(m_camera.Zoom), (float)Settings::SCR_WIDTH / (float)Settings::SCR_HEIGHT, 0.1f, 500.0f);
	VisualCube m_cube;
	VisualCube m_solvedCube;
	BitwiseCube m_bitwiseCube;
	UI::Menu m_menu;
	Solver m_solver;
	std::string m_scramble = "Scramble: ";
	bool m_solving = false;
	bool m_autoRotateAfterScramble = true;
	int m_scrambleLength = 20;

	bool m_stepping = false;
	int m_stepPointer = 0;
	std::vector<std::string> m_steppingMoves;
	TextRenderer m_currentMoveRenderer;
	std::string m_currentMoveText;
};