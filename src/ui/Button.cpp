#include "ui/Button.h"

//Set Renderable Object Static
Button::Button(const std::vector<Tri>& tris, Shader* shader, const std::string& label)
    : RenderableObjectStatic(tris, shader), label(label) {}

Button::~Button() {}

void Button::setVisible(bool v) {
    visible = v;
}

bool Button::isVisible() const {
    return visible;
}

// void Button::draw(const glm::mat4& parentTransform, const std::vector<LightSource>& lights)
// {
    
// }
