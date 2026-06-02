#include "juicy-game/Assets/AssetsManager.hpp"


std::shared_ptr<TextureAsset> AssetsManager::getTexture(std::string _name)
{
	if (m_textures.find(_name) == m_textures.end())
		return nullptr;
	return m_textures[_name];
}

std::shared_ptr<TextureAsset> AssetsManager::getTexture(std::string _name, const char* _path)
{
	if (m_textures.find(_name) != m_textures.end())
		return m_textures[_name];
	std::shared_ptr<TextureAsset> texture = std::shared_ptr<TextureAsset>(new TextureAsset(_path));
	m_textures[_name] = texture;
	return m_textures[_name];
}

void AssetsManager::preloadAsync(const std::string& _name, const char* _path)
{
	if (m_textures.find(_name) != m_textures.end()) return;

	m_textures[_name] = std::shared_ptr<TextureAsset>(new TextureAsset());
	m_loadQueue.push_back({ _name, std::string(_path) });
	++m_totalScheduled;
}

void AssetsManager::startBackgroundLoad()
{
	m_bgThread = std::thread([this]() {
		for (auto& [name, path] : m_loadQueue) {
			TextureAsset* tex = m_textures[name].get();
			tex->loadFromDisk(path.c_str());

			std::lock_guard<std::mutex> lock(m_pendingMutex);
			m_pendingUpload.push({ name, tex });
		}
		m_loadQueue.clear();
	});
}

int AssetsManager::uploadPending()
{
	std::lock_guard<std::mutex> lock(m_pendingMutex);
	if (m_pendingUpload.empty()) return m_totalScheduled - m_uploadedCount;

	PendingUpload item = m_pendingUpload.front();
	m_pendingUpload.pop();
	item.texture->uploadToGPU();
	++m_uploadedCount;
	return m_totalScheduled - m_uploadedCount;
}

bool AssetsManager::isLoadingComplete() const
{
	return m_uploadedCount.load() == m_totalScheduled.load();
}
