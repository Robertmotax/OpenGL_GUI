#pragma once
#include <unordered_map>
#include <string>
#include <core/Texture.h>

//singleton class to manage globally the entire texture aadequate for the project
class TextureManager {
public:
    static TextureManager& getInstance();

    void loadTextures();
    Texture* getTexture(const std::string& key);

    void cleanup(); // Free memory

private:
    TextureManager() = default;
    std::unordered_map<std::string, Texture*> textureMap;
};
