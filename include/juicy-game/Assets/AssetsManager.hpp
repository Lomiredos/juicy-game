#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <atomic>
#include <queue>
#include <vector>
#include <utility>

#include "juicy-game/Assets/Texture.hpp"

class AssetsManager
{
	struct PendingUpload {
		std::string   name;
		TextureAsset* texture;
	};

private:

	std::unordered_map<std::string, std::shared_ptr<TextureAsset>> m_textures;

	std::queue<PendingUpload> m_pendingUpload;
	std::mutex m_pendingMutex;

	std::atomic<int> m_totalScheduled{ 0 };
	std::atomic<int> m_uploadedCount{ 0 };

	std::vector<std::pair<std::string, std::string>> m_loadQueue;

	std::thread m_bgThread;

	AssetsManager() = default;

	AssetsManager(const AssetsManager&)            = delete;
	AssetsManager& operator=(const AssetsManager&) = delete;

public:

	static AssetsManager& getInstance() {
		static AssetsManager instance;
		return instance;
	}

	std::shared_ptr<TextureAsset> getTexture(std::string _name);
	std::shared_ptr<TextureAsset> getTexture(std::string _name, const char* _path);

	void preloadAsync(const std::string& _name, const char* _path);
	void startBackgroundLoad();
	int  uploadPending();
	bool isLoadingComplete() const;

	~AssetsManager() {
		if (m_bgThread.joinable()) m_bgThread.join();
		m_textures.clear();
	}
};
