#include "ui/Button.h"

//Set Renderable Object Static
Button::Button(const std::vector<Tri>& tris, Shader* shader, const std::string& label)
    : RenderableObjectStatic(tris, shader), label(label) {}

Button::~Button() {}

void Button::setCallback(std::function<void()> cb) {
    onClickCallback = cb;
}

//click on the button and set a proper callback function
void Button::click() {
    if (onClickCallback) onClickCallback();
}

void Button::setVisible(bool v) {
    visible = v;
}

bool Button::isVisible() const {
    return visible;
}