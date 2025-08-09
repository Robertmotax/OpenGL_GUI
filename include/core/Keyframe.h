#include <glm/glm.hpp>

struct Keyframe {
    float time;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    Keyframe() 
    : time(0.0f), position(0.0f), rotation(0.0f), scale(1.0f) {}

    Keyframe(float t, glm::vec3 p, glm::vec3 r, glm::vec3 s)
        : time(t), position(p), rotation(r), scale(s) {}
};