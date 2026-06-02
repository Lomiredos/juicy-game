#pragma once
#include "MySystem.hpp"


#include "../EventsBus.hpp"
#include "../Events/SoundsEvent.hpp"

#include <unordered_map>
#include <string>
#include <raylib.h>

class SoundsSystem : public MySystem {

	std::unordered_map<std::string, Sound> m_sounds;
	std::unordered_map<std::string, Music> m_musics;

public:

	void init() {

		EventBus::getInstance().subscribe<ee::event::PlayerDeadEvent>(
			[this](ee::event::PlayerDeadEvent _event) -> void {
				StopMusic("moteur");
			});

		EventBus::getInstance().subscribe<ee::event::LaserSoundEvent>(
			[this](ee::event::LaserSoundEvent _event) -> void {
				playSound("laser");
			});
		EventBus::getInstance().subscribe<ee::event::MoteurSoundEvent>(
			[this](ee::event::MoteurSoundEvent _event) -> void {
				playMusic("moteur", _event.volume, _event.pitch);
			});
		EventBus::getInstance().subscribe<ee::event::AlertSoundEvent>(
			[this](ee::event::AlertSoundEvent _event) -> void {
				playSound("alert", 2.f);
			});
		EventBus::getInstance().subscribe<ee::event::PlayerExploseEvent>(
			[this](ee::event::PlayerExploseEvent _event) -> void {
				playSound("explosion", 4.f);
			});
	}

	void update(ee::ecs::World& _world, float _dt) {

		for (auto& pair : m_musics) {
			if (IsMusicStreamPlaying(pair.second)) {
				UpdateMusicStream(pair.second);
			}
		}
	}


private:
	void playSound(std::string _name, float _volume = 1.0f, float _pitch = 1.0f) {

		if (m_sounds.find(_name) == m_sounds.end()) {
			std::string path = "assets/Sounds/" + _name + ".wav";
			m_sounds[_name] = LoadSound(path.c_str());
		}
		SetSoundVolume(m_sounds[_name], _volume);
		SetSoundPitch(m_sounds[_name], _pitch);
		PlaySound(m_sounds[_name]);
	}

	void playMusic(std::string _name, float _volume = 1.0f, float _pitch = 1.0f) {

		if (m_musics.find(_name) == m_musics.end()) {

			std::string path = "assets/Musics/" + _name + ".ogg";
			m_musics[_name] = LoadMusicStream(path.c_str());
		}

		SetMusicVolume(m_musics[_name], _volume);
		SetMusicPitch(m_musics[_name], _pitch);
		if (IsMusicStreamPlaying(m_musics[_name]) == false)
			PlayMusicStream(m_musics[_name]);
	}

	void StopMusic(std::string _name) {
		StopMusicStream(m_musics[_name]);
	}
};