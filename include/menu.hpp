#pragma once

#include <textRenderer.hpp>
#include <array>
#include <vector>
#include <UI_button.hpp>
#include <Shader.h>
#include <memory>
#include <textbox.hpp>

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
		RESETSETTINGS,
		ARROWS,
		SIZE_BUTTONS
	};

	enum TEXTBOXES {
		TURNSPEED = 0,
		SCRAMBLELENGTH,
		TEXTBOX_SIZE
	};
	//do toggle image - maybe just have a rect that you draw instead of button? if pressed render rect else button?
	class Menu {
	public:
		Menu()
			: m_textRenderer(RESOURCES_PATH"fonts/font1.ttf", 12, RESOURCES_PATH"shaders/text.vert", RESOURCES_PATH"shaders/text.frag"),
			m_shader(RESOURCES_PATH"shaders/rect.vert", RESOURCES_PATH"shaders/rect.frag"), m_arrowsRect(glm::vec2(Settings::SCR_WIDTH - 220, Settings::SCR_HEIGHT - 500),
				glm::vec2(80, 80) * 1.5f,
				RESOURCES_PATH"icons/arrowsGrey.png", m_shader, GL_RGBA)

		{
			float baseX = Settings::SCR_WIDTH - 220;  // shifted left for space
			float baseY = Settings::SCR_HEIGHT;

			// Buttons: Gear, Scramble, Solve, Reset
			m_buttons.push_back(std::make_unique<Button>(
				glm::vec2(baseX, baseY - 100),
				glm::vec2(80, 80) * 1.5f,
				RESOURCES_PATH"icons/menu.png", m_shader, GL_RGBA
			));
			m_buttons.push_back(std::make_unique<Button>(
				glm::vec2(baseX, baseY - 200),
				glm::vec2(80, 80) * 1.5f,
				RESOURCES_PATH"icons/scramble2.png", m_shader, GL_RGBA
			));
			m_buttons.push_back(std::make_unique<Button>(
				glm::vec2(baseX, baseY - 300),
				glm::vec2(80, 80) * 1.5f,
				RESOURCES_PATH"icons/solve2.png", m_shader, GL_RGBA
			));
			m_buttons.push_back(std::make_unique<Button>(
				glm::vec2(baseX, baseY - 400),
				glm::vec2(80, 80) * 1.5f,
				RESOURCES_PATH"icons/resetSettings.png", m_shader, GL_RGBA
			));
			m_buttons.push_back(std::make_unique<Button>(
				glm::vec2(baseX, baseY - 500),
				glm::vec2(80, 80) * 1.5f,
				RESOURCES_PATH"icons/arrows.png", m_shader, GL_RGBA
			));

			// TextBoxes: Turn speed, Rotation speed, Cube size
			std::vector<std::string> labels = { "TPS: ", "SCRAMBLE LENGTH: "};

			for (int i = 0; i < TEXTBOX_SIZE; ++i) {
				glm::vec2 pos = glm::vec2(baseX - 50, baseY - (600 + i * 100));  // left side of buttons
				glm::vec2 size = glm::vec2(250, 80);

				auto textbox = std::make_unique<TextBox>(pos, size, m_shader, &m_textRenderer);
				textbox->setLabel(labels[i]);
				textbox->setText(""); // user input starts empty
				m_textboxes.push_back(std::move(textbox));
			}

			m_textboxes[TURNSPEED]->setText("10");
			m_textboxes[SCRAMBLELENGTH]->setText("20");
		}

		void setPos(TEXTS text, int x, int y) {
			m_textPositions[text] = glm::vec2(x, y);
		}

		void setText(TEXTS text, std::string _text) {
			m_texts[text] = _text;
		}

		void setText(TEXTBOXES box, std::string _text) {
			m_textboxes[box]->setText(_text);
		}

		void draw() {
			glDisable(GL_DEPTH_TEST);

			m_buttons[GEAR]->Render();

			// Show textboxes only if gear is open
			if (m_buttons[GEAR]->getClicked()) {
				for (auto& textbox : m_textboxes) {
					textbox->render();
				}
				// Draw buttons
				for (int i = 1; i < SIZE_BUTTONS-1; ++i)
					m_buttons[i]->Render();

				if (m_buttons[ARROWS]->getClicked()) {
					m_arrowsRect.Render();
				}
				else {
					m_buttons[ARROWS]->Render();
				}
			}

			// Draw dynamic UI text
			for (int i = 0; i < TEXTS::SIZE; i++) {
				m_textRenderer.RenderText(m_texts[i], m_textPositions[i].x, m_textPositions[i].y, 1, glm::vec3(1, 1, 1));
			}

			glEnable(GL_DEPTH_TEST);
		}

		void update() {

			m_buttons[GEAR]->update(glm::vec2(Global::mouseLastX, Settings::SCR_HEIGHT - Global::mouseLastY));

			// Update textboxes if gear is clicked
			if (m_buttons[GEAR]->getClicked()) {
				for (auto& textbox : m_textboxes) {
					textbox->update(glm::vec2(Global::mouseLastX, Settings::SCR_HEIGHT - Global::mouseLastY));
				}

				// Update buttons
				for (int i = 1; i < SIZE_BUTTONS; ++i) {
					m_buttons[i]->update(glm::vec2(Global::mouseLastX, Settings::SCR_HEIGHT - Global::mouseLastY));
				}
			}
		}

		bool handleCharacterInput(unsigned int codepoint) {
			if (!m_buttons[GEAR]->getClicked()) return false;

			for (auto& textbox : m_textboxes) {
				if (textbox->getClicked()) {
					textbox->handleInput(codepoint);
					return true;
				}
			}

			return false;
			

		}

		void backspacePressed() {
			if (!m_buttons[GEAR]->getClicked()) return;

			for (auto& textbox : m_textboxes) {
				if (textbox->getClicked()) {
					textbox->backspacePressed();
					break;
				}
			}
		}

		float getTextboxInput(TEXTBOXES box) {
			return m_textboxes[box]->getInput();
		}

		bool getButtonClicked(BUTTONS item) {
			return m_buttons[item]->getClicked();
		}

		void setButtonClicked(BUTTONS item, bool clicked = true) {
			m_buttons[item]->setClicked(clicked);
		}

		std::string getTextBoxValue(TEXTBOXES box) const {
			return m_textboxes[box]->getText();
		}

	private:
		std::array<std::string, TEXTS::SIZE> m_texts;
		std::array<glm::vec2, TEXTS::SIZE> m_textPositions;
		TextRenderer m_textRenderer;
		std::vector<std::unique_ptr<Button>> m_buttons;
		std::vector<std::unique_ptr<TextBox>> m_textboxes;
		Shader m_shader;
		Rect m_arrowsRect;
	};
}
