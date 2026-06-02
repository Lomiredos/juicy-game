#include "juicy-game/Assets/ModelAsset.hpp"


ModelAsset::ModelAsset(const char* _path)
{
    load(_path);
}

ModelAsset::~ModelAsset()
{
    if (m_loaded)
        UnloadModel(m_model);
}

void ModelAsset::load(const char* _path)
{
    m_model  = LoadModel(_path);
    m_loaded = true;
}
