#pragma once

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "core/RenderableObject.h"
#include "core/Model.h"
#include "Globals.h"
#include "Vertex.h"

class OBJLoader {
public:
    static void exportToObj(const std::string& filename) {
        std::ofstream outFile(filename);
        if (!outFile.is_open()) {
            std::cerr << "Failed to open file for writing: " << filename << "\n";
            return;
        }

        int vertexOffset = 1;
        int texCoordOffset = 1;

        for (auto* obj : allObjects) {
            auto* sceneObj = dynamic_cast<RenderableObject*>(obj);
            if (!sceneObj || sceneObj->isUnlit) continue;

            outFile << "o " << sceneObj->getName() << "\n";
            glm::mat4 modelMatrix = sceneObj->getModelMatrix();

            for (const auto& tri : sceneObj->tris) {
                for (const auto& vertex : {tri.v0, tri.v1, tri.v2}) {
                    glm::vec4 worldPos = modelMatrix * glm::vec4(vertex.position, 1.0f);
                    outFile << "v " << worldPos.x << " " << worldPos.y << " " << worldPos.z << " "
                            << vertex.color.r << " " << vertex.color.g << " " << vertex.color.b << "\n";

                    outFile << "vt " << vertex.texCoord.x << " " << vertex.texCoord.y << "\n";
                }
            }

            int triangleCount = static_cast<int>(sceneObj->tris.size());
            for (int i = 0; i < triangleCount; ++i) {
                outFile << "f ";
                for (int j = 0; j < 3; ++j) {
                    int idx = vertexOffset + i * 3 + j;
                    int vtIdx = texCoordOffset + i * 3 + j;
                    outFile << idx << "/" << vtIdx << " ";
                }
                outFile << "\n";
            }

            vertexOffset += triangleCount * 3;
            texCoordOffset += triangleCount * 3;
        }

        // Export lights as comments
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
        namespace fs = std::filesystem;
        std::string filename = basePath + "/" + baseName + extension;
        int index = 1;

        while (fs::exists(filename)) {
            filename = basePath + "/" + baseName + "(" + std::to_string(index++) + ")" + extension;
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
        std::vector<glm::vec2> texCoords;
        std::vector<Tri> currentTris;
        std::string currentObjectName = "Unnamed";

        std::string line;
        while (std::getline(inFile, line)) {
            if (line.empty()) continue;

            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;

            if (prefix == "v") {
                float x, y, z, r = 0.5f, g = 0.5f, b = 0.5f;
                iss >> x >> y >> z;
                if (!(iss >> r >> g >> b)) r = g = b = 0.5f;
                positions.emplace_back(x, y, z);
                colors.emplace_back(r, g, b);
            } else if (prefix == "vt") {
                float u, v;
                iss >> u >> v;
                texCoords.emplace_back(u, v);
            } else if (prefix == "f") {
                std::vector<std::pair<int, int>> vertexPairs;
                std::string vertexDef;

                while (iss >> vertexDef) {
                    int vIdx = -1, vtIdx = -1;

                    // Check if vertexDef contains '/'
                    size_t slashPos = vertexDef.find('/');
                    if (slashPos == std::string::npos) {
                        // Format: f v1 v2 v3 (only vertex indices)
                        vIdx = std::stoi(vertexDef) - 1;
                    } else {
                        // Format: f v1/vt1 v2/vt2 v3/vt3
                        // Parse vertex and texture indices
                        std::string vStr = vertexDef.substr(0, slashPos);
                        std::string vtStr = vertexDef.substr(slashPos + 1);

                        vIdx = std::stoi(vStr) - 1;
                        if (!vtStr.empty()) {
                            // Sometimes vtStr can be "2/3" if normal index included - ignore normals here
                            size_t secondSlashPos = vtStr.find('/');
                            if (secondSlashPos != std::string::npos) {
                                vtStr = vtStr.substr(0, secondSlashPos);
                            }
                            if (!vtStr.empty()) {
                                vtIdx = std::stoi(vtStr) - 1;
                            }
                        }
                    }
                    vertexPairs.emplace_back(vIdx, vtIdx);
                }

            // Triangulate polygon face if more than 3 vertices
            for (size_t i = 1; i + 1 < vertexPairs.size(); ++i) {
                auto [i0v, i0t] = vertexPairs[0];
                auto [i1v, i1t] = vertexPairs[i];
                auto [i2v, i2t] = vertexPairs[i + 1];

                glm::vec2 t0 = (i0t >= 0 && i0t < texCoords.size()) ? texCoords[i0t] : glm::vec2(0.0f, 1.0f);
                glm::vec2 t1 = (i1t >= 0 && i1t < texCoords.size()) ? texCoords[i1t] : glm::vec2(0.0f, 0.0f);
                glm::vec2 t2 = (i2t >= 0 && i2t < texCoords.size()) ? texCoords[i2t] : glm::vec2(1.0f, 0.0f);

                Vertex v0{positions[i0v], colors[i0v], t0, {0, 0, 0}};
                Vertex v1{positions[i1v], colors[i1v], t1, {0, 0, 0}};
                Vertex v2{positions[i2v], colors[i2v], t2, {0, 0, 0}};

                currentTris.emplace_back(v0, v1, v2);
            }
            
        }  else if (prefix == "o") {
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
                    std::string label;
                    glm::vec3 pos, col, inten;
                    iss >> label >> pos.x >> pos.y >> pos.z;
                    iss >> label >> col.r >> col.g >> col.b;
                    iss >> label >> inten.x >> inten.y >> inten.z;

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

        std::cout << "Imported OBJ scene: " << filename << "\n";
    }
};
