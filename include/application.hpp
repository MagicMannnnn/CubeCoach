
#include <app_input.hpp>
#include <Shader.h>
#include <visualCube.hpp>
#include <bitwiseCube.hpp>
#include <solver.hpp>
#include <random.hpp>
#include <menu.hpp>



//TODO
/*
* create menu button icon which toggles all menu buttons
* create a settings text file which gets the settings and can update on button click from menu
* can open all these files from within the menu
* create a scramble text file where paste in scramble and click button to scramble using that scramble
*/





class App {
public:
	App() :

	 m_shader(RESOURCES_PATH"piece.vert", RESOURCES_PATH"piece.frag"),
	 m_cube(),
	 m_solvedCube(),
	 m_bitwiseCube(),
	 m_menu()
	{
		m_menu.setPos(UI::TEXTS::CROSS, 20, Settings::SCR_HEIGHT / 2);
		m_menu.setPos(UI::TEXTS::SCRAMBLE, Settings::SCR_WIDTH / 2 - 300, Settings::SCR_HEIGHT / 2 + 200);
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
		} else if (m_menu.getButtonClicked(UI::SOLVE)) {
			m_menu.setButtonClicked(UI::SOLVE, false);
			solve();
		}
		else if (m_menu.getButtonClicked(UI::OPENTXT)) {
			m_menu.setButtonClicked(UI::OPENTXT, false);
			m_solver.openInNotepad();
		}
		else if (m_menu.getButtonClicked(UI::OPENSETTINGS)) {
			m_menu.setButtonClicked(UI::OPENSETTINGS, false);
			Settings::openInNotepad();
		}
		else if (m_menu.getButtonClicked(UI::LOADSETTINGS)) {
			m_menu.setButtonClicked(UI::LOADSETTINGS, false);
			updateSettings();
		}
		else if (m_menu.getButtonClicked(UI::RESETSETTINGS)) {
			m_menu.setButtonClicked(UI::RESETSETTINGS, false);
			Settings::resetSettings();
		}
		else if (m_menu.getButtonClicked(UI::CONTROLS)) {
			m_menu.setButtonClicked(UI::CONTROLS, false);
			Settings::openControlsInNotepad();
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

		VisualCubeMoves();

		if (Global::rightClick) {
			m_cube.resetOrientation();
		}

		m_cube.update(Global::deltaTime);
		m_solvedCube.update(Global::deltaTime);

		

		Global::mouseDeltaX = 0;
		Global::mouseDeltaY = 0;

		if (m_bitwiseCube.isSolved()) {
			m_solving = false;
		}



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
		m_menu.setText(UI::TEXTS::CROSS,  solution);
		m_bitwiseCube.applyMoves(m_solver.getSolution());
		m_cube.applyMoves(m_solver.getSolution());
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
		}





		if (m_scramble.size() > 400) return;

		// Handle U moves
		if (Global::U) { m_cube.U(); m_bitwiseCube.U(); addToScramble("U"); /*m_bitwiseCube.displayHex();*/ }
		if (Global::Up) { m_cube.Up(); m_bitwiseCube.Up(); addToScramble("U'"); /*m_bitwiseCube.displayHex();*/ }
		if (Global::U2) { m_cube.U2(); m_bitwiseCube.U2(); addToScramble("U2"); /*m_bitwiseCube.displayHex();*/ }

		// Handle D moves
		if (Global::D) { m_cube.D(); m_bitwiseCube.D(); addToScramble("D"); m_bitwiseCube.test();/*m_bitwiseCube.displayHex();*/ }
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



		if (Global::zero) {
			ScrambleCube();
		}

		

	}


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

	void ScrambleCube(int length = 20) {

		m_bitwiseCube.reset(true);
		m_cube = m_solvedCube;
		m_scramble = "Scramble: ";

		std::string scramble = "";

		std::array<std::string, 18> moves = {"U", "Up", "U2", "D", "Dp", "D2", "L", "Lp", "L2", "R", "Rp", "R2", "F", "Fp", "F2", "B", "Bp", "B2"};
		std::string prevMove = "Z";
		for (int i = 0; i < length; i++) {
			std::string move = moves[Random::getRangeInt(0, 17)];
			while (redo(move[0], prevMove[0])) {
				move = moves[Random::getRangeInt(0, 17)];
			}
			prevMove = move;
			scramble += move + " ";
			m_bitwiseCube.applyMove(move);
			m_cube.applyMove(move);
		}
		for (char& c : scramble) {
			if (c == 'p') c = '\'';
		}
		std::cout << "\nScramble: " << scramble << "\n";
		m_scramble += scramble + " ";
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
			if (i % 30 == 0) {
				newLine = true;
				
			}
			if (newLine && c == ' ') {
				res += "\n";
				newLine = false;
			}
		}


		m_menu.setText(UI::TEXTS::SCRAMBLE, res);
	}

	std::string getLastMove() {
		if (m_scramble.back() == '2' || m_scramble.back() == '\'') {
			char back = m_scramble.back();
			m_scramble.pop_back();
			char back2 = m_scramble.back();
			m_scramble.pop_back();
			std::string move;
			move += back2;
			move += back;
			return move;
		}
		std::string move;
		char back = m_scramble.back();
		move += back;
		m_scramble.pop_back();
		return move;
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
};