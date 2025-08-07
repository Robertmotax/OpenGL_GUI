#include <glm/glm.hpp>

struct Keyframe {
    float time;                  // Time of this keyframe
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};