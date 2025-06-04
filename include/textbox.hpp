#pragma once

#include "UI_button.hpp"
#include "textRenderer.hpp"
#include <GLFW/glfw3.h>
#include <string>
#include <regex>

namespace UI {
    // In TextBox class (TextBox.hpp or equivalent)
    class TextBox : public Rect {
    public:
        TextBox(glm::vec2 pos, glm::vec2 size, Shader& shader, TextRenderer* textRenderer)
            : Rect(pos, size, "", shader), m_textRenderer(textRenderer) {}

        void setLabel(const std::string& label) {
            m_label = label;
        }

        void setText(const std::string& text) {
            m_text = text;
        }

        std::string getText() const {
            return m_text;
        }

        float getInput() {
            std::regex pattern("^\\d*\\.?\\d+$");
            if (std::regex_match(m_text, pattern)) {
                return std::stof(m_text);
            }
            else {
                return -1;
            }
        }

        void render() {
            Render();  // Draw background

            // Draw label
            if (m_textRenderer) {
                m_textRenderer->RenderText(m_label + m_separator + m_text, m_pos.x + 5, m_pos.y + m_size.y - 18, 1.0f, glm::vec3(1, 1, 1));

                // Draw editable text after label
                //float labelWidth = m_label.length() * 7.0f; // crude estimate per char
                //m_textRenderer->RenderText(m_text, m_pos.x + 5 + labelWidth, m_pos.y + m_size.y - 18, 1.0f, glm::vec3(0.8f, 0.8f, 1.0f));
            }
        }

        void update(glm::vec2 mousePos) {
            if (Global::leftClick &&
                mousePos.x >= m_pos.x && mousePos.x <= m_pos.x + m_size.x &&
                mousePos.y >= m_pos.y && mousePos.y <= m_pos.y + m_size.y) {
                Global::leftClick = false;
                m_focused = true;
            }
            else if (Global::leftClick) {
                m_focused = false;
            }
        }

        void handleInput(unsigned int codepoint) {
            if (m_focused) {
                if (codepoint == 8 && !m_text.empty()) { // backspace
                    m_text.pop_back();
                }
                else if (codepoint >= 32 && codepoint < 127) {
                    m_text += static_cast<char>(codepoint);
                }
            }
        }

        void backspacePressed() {
            if (m_text.empty()) return;
            m_text.pop_back();
        }

        bool getClicked() const {
            return m_focused;
        }

        void setClicked(bool clicked = true) {
            m_focused = clicked;
        }

    private:
        std::string m_text;
        std::string m_label;
        std::string m_separator = "\n";
        bool m_focused = false;
        TextRenderer* m_textRenderer;
    };

}
