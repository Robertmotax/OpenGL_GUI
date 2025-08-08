#include <glm/glm.hpp>

struct Keyframe {
    float time;                  // Time of this keyframe
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    Keyframe(float t, glm::vec3 pos, glm::vec3 rot, glm::vec3 scl)
    : time(t), position(pos), rotation(rot), scale(scl) {}
};