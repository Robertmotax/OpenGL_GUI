#pragma once

#include <functional>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include "core./Tri.h"
#include "core/Shader.h"
#include "core/RenderableObjectStatic.h"
#include "core/RenderableObject.h"

class Button : public RenderableObjectStatic {
    public:
        Button(const std::vector<Tri>& tris, Shader* shader, const std::string& label = "");
        ~Button();

        void setVisible(bool v);
        bool isVisible() const;
        virtual void draw(const glm::mat4& parentTransform, const std::vector<LightSource>& lights);


    private:
        bool visible = true;
        std::string label; // future: render text overlay
};