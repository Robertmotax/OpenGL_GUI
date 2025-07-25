#include <singleton/TextureManager.h>

TextureManager& TextureManager::getInstance() {
    static TextureManager instance; //return global instance; thread-safe for C++
    return instance;
}

void TextureManager::loadTextures() {
    textureMap["alaskanMalamut"] = new Texture("textures/alaskan-malamut.jpg");
    textureMap["darkness"]       = new Texture("textures/darkness.jpg");
    textureMap["jupiter"]        = new Texture("textures/jupiter_surface.jpg");
    textureMap["uranus"]         = new Texture("textures/uranus_surface.jpg");
    textureMap["grassland"]      = new Texture("textures/grass-texture.jpg");
}

Texture* TextureManager::getTexture(const std::string& key) {
    auto it = textureMap.find(key);
    return (it != textureMap.end()) ? it->second : nullptr;
}

void TextureManager::cleanup() {
    for (auto& pair : textureMap) {
        delete pair.second;
    }
    textureMap.clear();
}
