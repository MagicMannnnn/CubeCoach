#pragma once

#include <textRenderer.hpp>
#include <array>
#include <vector>
#include <UI_button.hpp>
#include <Shader.h>
#include <memory>

namespace UI {

	enum TEXTS {
		CROSS = 0,
		F2L,
		OLL,
		PLL,
		SCRAMBLE,
		SIZE
	};

	enum BUTTONS {
		GEAR = 0,
		SCRAMBLE_BUTTON,
		SOLVE,
		OPENTXT,
		OPENSETTINGS,
		LOADSETTINGS,
		RESETSETTINGS,
		CONTROLS,
		SIZE_BUTTONS
	};

	class Menu {
	public:
		Menu()
		: m_textRenderer(RESOURCES_PATH"font1.ttf", 12, RESOURCES_PATH"text.vert", RESOURCES_PATH"text.frag"),
			m_shader(RESOURCES_PATH"rect.vert", RESOURCES_PATH"rect.frag")
		   {
			m_buttons.push_back(new Button(glm::vec2(Settings::SCR_WIDTH - 100 - 20, Settings::SCR_HEIGHT - 100), glm::vec2(80, 80) * 1.5f, RESOURCES_PATH"icons/menu.png", m_shader, GL_RGBA));
			m_buttons.push_back(new Button(glm::vec2(Settings::SCR_WIDTH - 110 - 20, Settings::SCR_HEIGHT - 200), glm::vec2(80, 80) * 1.5f, RESOURCES_PATH"icons/scramble2.png", m_shader, GL_RGBA));
			m_buttons.push_back(new Button(glm::vec2(Settings::SCR_WIDTH - 110 - 20, Settings::SCR_HEIGHT - 300), glm::vec2(80, 80) * 1.5f, RESOURCES_PATH"icons/solve2.png", m_shader, GL_RGBA));
			m_buttons.push_back(new Button(glm::vec2(Settings::SCR_WIDTH - 110 - 20, Settings::SCR_HEIGHT - 400), glm::vec2(80, 80) * 1.5f, RESOURCES_PATH"icons/opentxt2.png", m_shader, GL_RGBA));
			m_buttons.push_back(new Button(glm::vec2(Settings::SCR_WIDTH - 110 - 20, Settings::SCR_HEIGHT - 500), glm::vec2(80, 80) * 1.5f, RESOURCES_PATH"icons/openSettings.png", m_shader, GL_RGBA));
			m_buttons.push_back(new Button(glm::vec2(Settings::SCR_WIDTH - 110 - 20, Settings::SCR_HEIGHT - 600), glm::vec2(80, 80) * 1.5f, RESOURCES_PATH"icons/loadSettings.png", m_shader, GL_RGBA));
			m_buttons.push_back(new Button(glm::vec2(Settings::SCR_WIDTH - 110 - 20, Settings::SCR_HEIGHT - 700), glm::vec2(80, 80) * 1.5f, RESOURCES_PATH"icons/resetSettings.png", m_shader, GL_RGBA));
			m_buttons.push_back(new Button(glm::vec2(Settings::SCR_WIDTH - 110 - 20, Settings::SCR_HEIGHT - 800), glm::vec2(80, 80) * 1.5f, RESOURCES_PATH"icons/controls.png", m_shader, GL_RGBA));

		}
		~Menu() {
			m_buttons.clear();
		}

		void setPos(TEXTS text, int x, int y) {
			m_textPositions[text] = glm::vec2(x, y);
		}

		void setText(TEXTS text, std::string _text) {
			m_texts[text] = _text;
		}

		void draw() {

			glDisable(GL_DEPTH_TEST);

			
			for (int i = 0; i < SIZE_BUTTONS; i++) {
				if (i == GEAR) {
					m_buttons[i]->Render();
				}
				else if (m_buttons[GEAR]->getClicked()){
					m_buttons[i]->Render();
				}
				
			}
			

			for (int i = 0; i < TEXTS::SIZE; i++) {
				m_textRenderer.RenderText(m_texts[i], m_textPositions[i].x, m_textPositions[i].y, 1, glm::vec3(1, 1, 1));
			}

			glEnable(GL_DEPTH_TEST);

			
			
		}


		void update() {

			
			for (int i = 0; i < SIZE_BUTTONS; i++) {
				if (i == GEAR) {
					m_buttons[i]->update(glm::vec2(Global::mouseLastX, Settings::SCR_HEIGHT - Global::mouseLastY));
				}
				else if (m_buttons[GEAR]->getClicked()) {
					m_buttons[i]->update(glm::vec2(Global::mouseLastX, Settings::SCR_HEIGHT - Global::mouseLastY));
				}

			}
			
		}

		bool getButtonClicked(BUTTONS item) {
			return m_buttons[item]->getClicked();
		}

		void setButtonClicked(BUTTONS item, bool clicked = true) {
			m_buttons[item]->setClicked(clicked);
		}

	private:
		std::array<std::string, TEXTS::SIZE> m_texts;
		std::array<glm::vec2, TEXTS::SIZE> m_textPositions;
		TextRenderer m_textRenderer;
		std::vector<Button*> m_buttons;
		Shader m_shader;
	};
}