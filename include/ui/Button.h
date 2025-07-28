#pragma once

#include <functional>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include "core./Tri.h"
#include "core/Shader.h"
#include "core/RenderableObjectStatic.h"

class Button : public RenderableObjectStatic {
    public:
        Button(const std::vector<Tri>& tris, Shader* shader, const std::string& label = "");

        ~Button();

        void setCallback(std::function<void()> cb); // setter
        void click();  // invokes the callback
        void setVisible(bool v);
        bool isVisible() const;

    private:
        std::function<void()> onClickCallback;
        bool visible = true;
        std::string label; // future: render text overlay
};