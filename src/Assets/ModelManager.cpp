#include "juicy-game/Assets/ModelManager.hpp"


std::shared_ptr<ModelAsset> ModelManager::getModel(const char* _name)
{
    for (auto& e : m_models)
        if (strcmp(e.name, _name) == 0)
            return e.asset;
    return nullptr;
}

std::shared_ptr<ModelAsset> ModelManager::getModel(const char* _name, const char* _path)
{
    for (auto& e : m_models)
        if (strcmp(e.name, _name) == 0)
            return e.asset;

    Entry e;
    strncpy(e.name, _name, 127);
    e.name[127] = '\0';
    e.asset = std::shared_ptr<ModelAsset>(new ModelAsset(_path));
    m_models.push_back(e);
    return m_models.back().asset;
}

void ModelManager::preload(const char* _name, const char* _path)
{
    getModel(_name, _path);
}
