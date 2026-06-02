#include "juicy-game/Assets/Texture.hpp"


TextureAsset::TextureAsset(const char* _path)
{
    loadFromDisk(_path);
    uploadToGPU();
}

TextureAsset::~TextureAsset()
{
    UnloadTexture(m_texture);
}

void TextureAsset::loadFromDisk(const char* _path)
{
    m_image = LoadImage(_path);
    if (m_image.data == nullptr)
        std::abort();
    m_imageReady = true;
}

void TextureAsset::uploadToGPU()
{
    m_texture    = LoadTextureFromImage(m_image);
    m_width      = (float)m_texture.width;
    m_height     = (float)m_texture.height;
    UnloadImage(m_image);
    m_image      = {};
    m_imageReady = false;
}
