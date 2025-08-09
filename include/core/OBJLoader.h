#pragma once
#include "RenderableObjectStatic.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <sstream>

class OBJLoader {
public:
    static void load(const std::string &filename) 
    {
        std::ifstream inFile(filename);
        if (!inFile.is_open()) {
            std::cerr << "Failed to open file for reading: " << filename << "\n";
            return;
        }

        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> colors;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texCoords;
        std::vector<Tri> currentTris;
        std::string currentObjectName = "Unnamed";

        // Temporary container for newly created objects, indexed by name
        std::unordered_map<std::string, RenderableObject*> objMap;

        // We will store pending metadata by object name here:
        struct PendingMetadata {
            glm::vec3 position{0.0f};
            glm::vec3 rotation{0.0f};
            glm::vec3 scale{1.0f};
            Texture* texture = nullptr;
            std::vector<Keyframe> keyframes;
            std::string parentName;
            bool hasPosition = false, hasRotation = false, hasScale = false;
        };
        std::unordered_map<std::string, PendingMetadata> metadataMap;

        std::string line;
        while (std::getline(inFile, line)) {
            if (line.empty()) continue;

            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;

            if (prefix == "v") {
                float x, y, z, r = 0.5f, g = 0.5f, b = 0.5f, s = 0.0f, t = 0.0f, p = 0.0f;
                iss >> x >> y >> z;
                if (!(iss >> r >> g >> b)) r = g = b = 0.5f;
                if (!(iss >> s >> t >> p)) s = t = p = 0.0f;
                positions.emplace_back(x, y, z);
                colors.emplace_back(r, g, b);
                normals.emplace_back(s, t, p);
            } else if (prefix == "vt") {
                float u, v;
                iss >> u >> v;
                texCoords.emplace_back(u, v);
            } else if (prefix == "f") {
                std::vector<std::pair<int, int>> vertexPairs;
                std::string vertexDef;

                while (iss >> vertexDef) {
                    int vIdx = -1, vtIdx = -1;

                    size_t slashPos = vertexDef.find('/');
                    if (slashPos == std::string::npos) {
                        vIdx = std::stoi(vertexDef) - 1;
                    } else {
                        std::string vStr = vertexDef.substr(0, slashPos);
                        std::string vtStr = vertexDef.substr(slashPos + 1);

                        vIdx = std::stoi(vStr) - 1;
                        if (!vtStr.empty()) {
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

                for (size_t i = 1; i + 1 < vertexPairs.size(); ++i) {
                    auto [i0v, i0t] = vertexPairs[0];
                    auto [i1v, i1t] = vertexPairs[i];
                    auto [i2v, i2t] = vertexPairs[i + 1];

                    glm::vec2 t0 = (i0t >= 0 && i0t < texCoords.size()) ? texCoords[i0t] : glm::vec2(0.0f, 1.0f);
                    glm::vec2 t1 = (i1t >= 0 && i1t < texCoords.size()) ? texCoords[i1t] : glm::vec2(0.0f, 0.0f);
                    glm::vec2 t2 = (i2t >= 0 && i2t < texCoords.size()) ? texCoords[i2t] : glm::vec2(1.0f, 0.0f);

                    Vertex v0{positions[i0v], colors[i0v], t0, normals[i0v]};
                    Vertex v1{positions[i1v], colors[i1v], t1, normals[i1v]};
                    Vertex v2{positions[i2v], colors[i2v], t2, normals[i2v]};

                    currentTris.emplace_back(v0, v1, v2);
                }
            } else if (prefix == "o") {
                if (!currentTris.empty()) {
                    auto* obj = new RenderableObject(currentTris, defaultShader, shadowShader);
                    obj->setName(currentObjectName);
                    objMap[currentObjectName] = obj;
                    allObjects.push_back(obj);
                    currentTris.clear();
                }
                iss >> currentObjectName;

                // Initialize metadata entry if missing
                if (metadataMap.find(currentObjectName) == metadataMap.end()) {
                    metadataMap[currentObjectName] = PendingMetadata{};
                }

            } else if (prefix == "#META_POSITION") {
                std::string objName;
                glm::vec3 pos;
                iss >> objName >> pos.x >> pos.y >> pos.z;
                metadataMap[objName].position = pos;
                metadataMap[objName].hasPosition = true;

            } else if (prefix == "#META_ROTATION") {
                std::string objName;
                glm::vec3 rot;
                iss >> objName >> rot.x >> rot.y >> rot.z;
                metadataMap[objName].rotation = rot;
                metadataMap[objName].hasRotation = true;

            } else if (prefix == "#META_SCALE") {
                std::string objName;
                glm::vec3 scl;
                iss >> objName >> scl.x >> scl.y >> scl.z;
                metadataMap[objName].scale = scl;
                metadataMap[objName].hasScale = true;

            } else if (prefix == "#META_KEYFRAME") {
                std::string objName;
                Keyframe kf;
                iss >> objName
                    >> kf.time
                    >> kf.position.x >> kf.position.y >> kf.position.z
                    >> kf.rotation.x >> kf.rotation.y >> kf.rotation.z
                    >> kf.scale.x >> kf.scale.y >> kf.scale.z;
                metadataMap[objName].keyframes.push_back(kf);

            } else if (prefix == "#META_PARENT") {
                std::string objName, parentName;
                iss >> objName >> parentName;
                metadataMap[objName].parentName = parentName;
            } else if (prefix == "#META_TEXTURE") {
                std::string texturePath, objName;
                iss >> objName >> texturePath;
                metadataMap[objName].texture = new Texture(texturePath.c_str());
            }   else if (prefix == "#") {
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

        // Add the last object if any tris remain
        if (!currentTris.empty()) {
            auto* obj = new RenderableObject(currentTris, defaultShader, shadowShader);
            obj->setName(currentObjectName);
            objMap[currentObjectName] = obj;
            allObjects.push_back(obj);
        }

        // Now apply metadata to each object by name
        for (auto& [name, obj] : objMap) {
            auto it = metadataMap.find(name);
            if (it != metadataMap.end()) {
                const auto& meta = it->second;
                if (meta.hasPosition) obj->position = meta.position;
                if (meta.hasRotation) obj->rotation = meta.rotation;
                if (meta.hasScale) obj->scale = meta.scale;
                obj->keyframes = meta.keyframes; // overwrite or append if you want

                // Set parent if exists
                if (!meta.parentName.empty()) {
                    auto parentIt = objMap.find(meta.parentName);
                    if (parentIt != objMap.end()) {
                        obj->setParent(parentIt->second);
                    }
                }

                if (meta.texture != nullptr) {
                        obj->setTexture(meta.texture);
                        obj->enableTexture(true);
                }

                // After all transforms are set, update local transform and children
                obj->updateLocalTransformFromComponents();
                obj->updateSelfAndChildren();
            }
        }

        std::cout << "Imported OBJ scene: " << filename << "\n";
    }

    static void save(const std::string &filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file for export: " << filename << std::endl;
            return;
        }

        int vertexOffset = 1;
        int texCoordOffset = 1;

        for (auto *obj : allObjects) {
            RenderableObject* sceneObj = dynamic_cast<RenderableObject*>(obj);
            if (!sceneObj || sceneObj->isUnlit) continue;

            file << "o " << sceneObj->getName() << "\n";

            for (auto& tri : sceneObj->tris) {
                for (const auto& vertex : {tri.v0, tri.v1, tri.v2}) {
                    file << "v " << vertex.position.x << " " << vertex.position.y << " " << vertex.position.z << " "
                            << vertex.color.r << " " << vertex.color.g << " " << vertex.color.b << " "
                            << vertex.normal.s << " " << vertex.normal.t << " " << vertex.normal.p << "\n";

                    file << "vt " << vertex.texCoord.x << " " << vertex.texCoord.y << "\n";
                }
            }

            int triangleCount = static_cast<int>(sceneObj->tris.size());
            for (int i = 0; i < triangleCount; ++i) {
                file << "f ";
                for (int j = 0; j < 3; ++j) {
                    int idx = vertexOffset + i * 3 + j;
                    int vtIdx = texCoordOffset + i * 3 + j;
                    file << idx << "/" << vtIdx << " ";
                }
                file << "\n";
            }

            vertexOffset += triangleCount * 3;
            texCoordOffset += triangleCount * 3;

            // Export metadata
            if (sceneObj->parent)
                file << "#META_PARENT " << sceneObj->getName() << " " << sceneObj->parent->getName() << "\n";

            file << "#META_POSITION " << sceneObj->getName() << " " << sceneObj->position.x << " " << sceneObj->position.y << " " << sceneObj->position.z << "\n";
            file << "#META_ROTATION " << sceneObj->getName() << " " << sceneObj->rotation.x << " " << sceneObj->rotation.y << " " << sceneObj->rotation.z << "\n";
            file << "#META_SCALE " << sceneObj->getName() << " " << sceneObj->scale.x << " " << sceneObj->scale.y << " " << sceneObj->scale.z << "\n";

            if(sceneObj->texture)
            {
                file << "#META_TEXTURE " << sceneObj->getName() << " " << sceneObj->texture->fileLocation << "\n";
            }

            for (const auto &k : sceneObj->keyframes) {
                file << "#META_KEYFRAME " << sceneObj->getName() << " "
                    << k.time << " "
                    << k.position.x << " " << k.position.y << " " << k.position.z << " "
                    << k.rotation.x << " " << k.rotation.y << " " << k.rotation.z << " "
                    << k.scale.x << " " << k.scale.y << " " << k.scale.z << "\n";
            }
        }

        // Export lights as comments
        file << "\n# === Light Sources ===\n";
        for (const auto* light : lights) {
            if (!light || !light->position || !light->color || !light->intensity) continue;
            glm::vec3 pos = *light->position;
            glm::vec3 col = *light->color;
            glm::vec3 inten = *light->intensity;

            file << "# light "
                    << "pos " << pos.x << " " << pos.y << " " << pos.z << " "
                    << "color " << col.r << " " << col.g << " " << col.b << " "
                    << "intensity " << inten.x << " " << inten.y << " " << inten.z << "\n";
        }
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
};
