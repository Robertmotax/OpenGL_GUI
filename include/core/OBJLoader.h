#pragma once
#include <fstream>
#include <vector>
#include <string>
#include "core/RenderableObject.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "core/Model.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "Vertex.h"
#include "Globals.h"

class OBJLoader {
public:
    static void exportToObj(const std::string& filename) {
        std::ofstream outFile(filename);
        if (!outFile.is_open()) {
            std::cerr << "Failed to open file for writing: " << filename << "\n";
            return;
        }

        int vertexOffset = 1;

        for (auto* obj : allObjects) {
            if (auto* sceneObj = dynamic_cast<RenderableObject*>(obj)) {
                if (!sceneObj->isUnlit) {
                    outFile << "o " << sceneObj->getName() << "\n";
                    glm::mat4 modelMatrix = sceneObj->getModelMatrix();

                    for (const auto& tri : sceneObj->tris) {
                        for (const auto& vertex : {tri.v0, tri.v1, tri.v2}) {
                            glm::vec4 worldPos = modelMatrix * glm::vec4(vertex.position, 1.0f);
                            // Export vertex position + color (r, g, b)
                            outFile << "v " 
                                    << worldPos.x << " " << worldPos.y << " " << worldPos.z << " "
                                    << vertex.color.r << " " << vertex.color.g << " " << vertex.color.b << "\n";
                        }
                    }

                    int triangleCount = sceneObj->tris.size();
                    for (int i = 0; i < triangleCount; ++i) {
                        outFile << "f " << vertexOffset << " " << (vertexOffset + 1) << " " << (vertexOffset + 2) << "\n";
                        vertexOffset += 3;
                    }
                }
            }
        }

        // Write lights as comments (unchanged)
        outFile << "\n# === Light Sources ===\n";
        for (const auto* light : lights) {
            if (!light || !light->position || !light->color || !light->intensity) continue;
            glm::vec3 pos = *light->position;
            glm::vec3 col = *light->color;
            glm::vec3 inten = *light->intensity;

            outFile << "# light "
                    << "pos " << pos.x << " " << pos.y << " " << pos.z << " "
                    << "color " << col.r << " " << col.g << " " << col.b << " "
                    << "intensity " << inten.x << " " << inten.y << " " << inten.z << "\n";
        }

        outFile.close();
        std::cout << "Exported scene with lights to: " << filename << "\n";
    }

    static void importFromObj(const std::string& filename) {
        std::ifstream inFile(filename);
        if (!inFile.is_open()) {
            std::cerr << "Failed to open file for reading: " << filename << "\n";
            return;
        }

        std::vector<glm::vec3> tempVertices;
        std::vector<glm::vec3> tempColors;
        std::string currentObjectName;
        std::vector<Tri> currentTris;

        std::string line;
        while (std::getline(inFile, line)) {
            if (line.empty()) continue;

            std::istringstream iss(line);
            std::string token;
            iss >> token;

            if (token == "v") {
                float x, y, z;
                float r = 0.5f, g = 0.5f, b = 0.5f; // default color gray

                iss >> x >> y >> z;
                if (!(iss >> r >> g >> b)) {
                    // No color specified, fallback to default
                    r = g = b = 0.5f;
                }

                tempVertices.emplace_back(x, y, z);
                tempColors.emplace_back(r, g, b);

            } else if (token == "f") {
                int i1, i2, i3;
                iss >> i1 >> i2 >> i3;

                Vertex v1{ tempVertices[i1 - 1], tempColors[i1 - 1], {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
                Vertex v2{ tempVertices[i2 - 1], tempColors[i2 - 1], {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
                Vertex v3{ tempVertices[i3 - 1], tempColors[i3 - 1], {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };

                currentTris.emplace_back(v1, v2, v3);

            } else if (token == "o") {
                if (!currentTris.empty()) {
                    auto* obj = new RenderableObject(currentTris, defaultShader, shadowShader);
                    obj->setName(currentObjectName);
                    allObjects.push_back(obj);

                    currentTris.clear();
                }
                iss >> currentObjectName;

            } else if (token == "#") {
                std::string commentType;
                iss >> commentType;
                if (commentType == "light") {
                    std::string posLabel, colorLabel, intensityLabel;
                    glm::vec3 pos, col, inten;
                    iss >> posLabel >> pos.x >> pos.y >> pos.z
                        >> colorLabel >> col.r >> col.g >> col.b
                        >> intensityLabel >> inten.x >> inten.y >> inten.z;

                    auto* light = new LightSource(pos, col, inten, defaultShader, shadowShader);
                    RenderableObject* handler = light->lightHandler;
                    allObjects.push_back(handler);
                    light->initShadowCubemap();
                    lights.push_back(light);
                }
            }
        }

        // Final object
        if (!currentTris.empty()) {
            auto* obj = new RenderableObject(currentTris, defaultShader, shadowShader);
            obj->setName(currentObjectName);
            allObjects.push_back(obj);
        }

        std::cout << "Imported scene from: " << filename << "\n";
    }
};