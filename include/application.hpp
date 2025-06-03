#pragma once


#include <default.hpp>



//TODO
/*
* create menu button icon which toggles all menu buttons
* create a settings text file which gets the settings and can update on button click from menu
* can open all these files from within the menu
* create a scramble text file where paste in scramble and click button to scramble using that scramble
* create a textbox so all the text files are unnessarsary
*/





class App {
	enum GAMESTATE {
		DEFAULT = 0
	};


public:
	App() : m_default()

	{
	}



	
	void run() {

		
		while (!glfwWindowShouldClose(Global::window)) {


			switch (m_state) {
			case DEFAULT:
				m_default.run();
				break;
			default:
				break;
			}

			
		}

	}




private:

private:
	Default m_default;
	GAMESTATE m_state = DEFAULT;
};