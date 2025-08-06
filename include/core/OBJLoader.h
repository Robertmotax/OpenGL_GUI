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
#include <algorithm>
#include <filesystem>
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

    static std::string getAvailableFilename(const std::string& basePath, const std::string& baseName, const std::string& extension) {
        namespace fs = std::filesystem; // C++17 required

        std::string filename = basePath + "/" + baseName + extension;
        int index = 1;

        while (fs::exists(filename)) {
            filename = basePath + "/" + baseName + "(" + std::to_string(index) + ")" + extension;
            ++index;
        }

        return filename;
    }

   static void importFromObj(const std::string& filename) {
        std::ifstream inFile(filename);
        if (!inFile.is_open()) {
            std::cerr << "Failed to open file for reading: " << filename << "\n";
            return;
        }

        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> colors;
        std::vector<Tri> currentTris;
        std::string currentObjectName = "Unnamed";

        std::string line;
        while (std::getline(inFile, line)) {
            if (line.empty() || line[0] == '\r') continue;

            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;

            if (prefix == "v") {
                float x, y, z, r = 0.5f, g = 0.5f, b = 0.5f;
                iss >> x >> y >> z;
                if (!(iss >> r >> g >> b)) {
                    r = g = b = 0.5f;  // default gray
                }
                positions.emplace_back(x, y, z);
                colors.emplace_back(r, g, b);

            } else if (prefix == "f") {
                std::vector<int> indices;
                std::string vertex;
                while (iss >> vertex) {
                    std::replace(vertex.begin(), vertex.end(), '/', ' '); // Replace slashes
                    std::istringstream vss(vertex);
                    int posIdx;
                    vss >> posIdx;
                    indices.push_back(posIdx - 1); // OBJ indices start at 1
                }

                if (indices.size() >= 3) {
                    for (size_t i = 1; i + 1 < indices.size(); ++i) {
                        int i0 = indices[0];
                        int i1 = indices[i];
                        int i2 = indices[i + 1];

                        Vertex v0{positions[i0], colors[i0], {0, 0}, {0, 0, 0}};
                        Vertex v1{positions[i1], colors[i1], {0, 0}, {0, 0, 0}};
                        Vertex v2{positions[i2], colors[i2], {0, 0}, {0, 0, 0}};
                        currentTris.emplace_back(v0, v1, v2);
                    }
                }

            } else if (prefix == "o") {
                if (!currentTris.empty()) {
                    auto* obj = new RenderableObject(currentTris, defaultShader, shadowShader);
                    obj->setName(currentObjectName);
                    allObjects.push_back(obj);
                    currentTris.clear();
                }
                iss >> currentObjectName;

            } else if (prefix == "#") {
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

        if (!currentTris.empty()) {
            auto* obj = new RenderableObject(currentTris, defaultShader, shadowShader);
            obj->setName(currentObjectName);
            allObjects.push_back(obj);
        }

        std::cout << "Imported scene OBJ: " << filename << "\n";
    }
};