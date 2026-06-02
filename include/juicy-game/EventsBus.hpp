
#pragma once

#include <unordered_map>
#include <functional>
#include <any>
#include <vector>
#include <typeindex>


class EventBus {

private:

	std::unordered_map<std::type_index, std::vector<std::any>> m_listeners;

	EventBus() {}

public:

	static EventBus& getInstance() {
		static EventBus instance = EventBus();
		return instance;
	}

	template<typename T>
	void publish(T _data) 
	{
		auto it = m_listeners.find(typeid(T));
		if (it != m_listeners.end()) {
			for (auto& cb : it->second) {
				std::any_cast<std::function<void(T)>>(cb)(_data);
			}
		}
	};

	template<typename T>
	void subscribe(std::function<void(T)> _callback)
	{
		m_listeners[std::type_index(typeid(T))].push_back(_callback);
	};

	// Vide toutes les souscriptions — à appeler avant un changement de scène
	void clearAll()
	{
		m_listeners.clear();
	}

};