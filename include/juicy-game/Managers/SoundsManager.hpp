#pragma once
#include "raylib.h"
#include <string>
#include <unordered_map>


struct SoundData {
	Sound sound;
	float volume;
	float pitch;

	std::string group;
};

struct MusicData {
	Music music;
	float volume;
	float pitch;
	std::string group;
};



class SoundsManager {

private:


	std::unordered_map<std::string, SoundData> m_sounds;
	std::unordered_map<std::string, MusicData> m_musics;

	//first is Volume second is Pitch
	std::unordered_map<std::string, std::pair<float, float>> m_groupVolumePitch;
	float m_generalVolume = 1.f;
	float m_generalPitch = 1.f;



	SoundsManager() {}

public:

	static SoundsManager& getInstance() {
		static SoundsManager instance = SoundsManager();
		return instance;
	}


	void update() {


		///raylib went us to updateMusicStream Manualy eatch frames
		for (const std::pair<std::string, MusicData>& m : m_musics) {
			if (IsMusicStreamPlaying(m.second.music))
				UpdateMusicStream(m.second.music);

		}
	}

	void PlaySound(std::string _name) {

		if (m_sounds.find(_name) == m_sounds.end()) return;
		
		::PlaySound(m_sounds[_name].sound);
	}

	void PlayMusic(std::string _name) {
		if (m_musics.find(_name) == m_musics.end()) return;
		::PlayMusicStream(m_musics[_name].music);
	}



	//Setters

	//general Setter
	void SetGeneralVolume(float _amout) {
		m_generalVolume = _amout;
		
		for (const std::pair<std::string, SoundData>& sound : m_sounds) {
			ApplySoundVolume(sound.first);
		}

		for (const std::pair<std::string, MusicData>& music : m_musics) {
			ApplyMusicVolume(music.first);
		}

	}
	void SetGeneralPitch(float _amout) {
		m_generalPitch = _amout;

		for (const std::pair<std::string, SoundData>& sound : m_sounds) {
			ApplySoundPitch(sound.first);
		}

		for (const std::pair<std::string, MusicData>& music : m_musics) {
			ApplyMusicPitch(music.first);
		}
	}
	
	//groupe Setter
	void SetGroupVolume(std::string _name, float _volume) {

		if (m_groupVolumePitch.find(_name) == m_groupVolumePitch.end()) return;
		m_groupVolumePitch[_name].first = _volume;

		//Apply Volume to eatch sound of the current groupe
		for (const std::pair<std::string, SoundData>& sound : m_sounds){
			if (sound.second.group != _name) continue;
			ApplySoundVolume(sound.first);
		}

		//same for music
		for (const std::pair<std::string, MusicData>& music : m_musics) {
			if (music.second.group != _name) continue;
			ApplyMusicVolume(music.first );
		}
	}
	void SetGroupPitch(std::string _name, float _pitch) {

		if (m_groupVolumePitch.find(_name) == m_groupVolumePitch.end()) return;
		m_groupVolumePitch[_name].second = _pitch;

		//Apply Volume to eatch sound of the current groupe
		for (const std::pair<std::string, SoundData>& sound : m_sounds) {
			if (sound.second.group != _name) continue;
			ApplySoundPitch(_name);
		}

		//same for music
		for (const std::pair<std::string, MusicData>& music : m_musics) {
			if (music.second.group != _name) continue;
			ApplyMusicPitch(_name);
		}
	}



	//Particular Setter
	/*
	* Warning ! SoundVolume is affect by his groupe Volume and General Volume
	*/
	void SetSoundVolume(std::string _name, float _volume) {
		if (m_sounds.find(_name) == m_sounds.end()) return;
		m_sounds[_name].volume = _volume;
		ApplySoundVolume(_name);
	}
	/*
	* Warning ! MusicVolume is affect by his groupe Volume and General Volume
	*/
	void SetMusicVolume(std::string _name, float _volume) {
		if (m_musics.find(_name) == m_musics.end()) return;
		m_musics[_name].volume = _volume;
		ApplyMusicVolume(_name);
	}
	/*
	* Waring ! SoundPitch is affect by his groupe Pitch and General Pitch
	*/
	void SetSoundPitch(std::string _name, float _pitch) {
		if (m_sounds.find(_name) == m_sounds.end()) return;
		m_sounds[_name].pitch = _pitch;
		ApplySoundPitch(_name);
	}
	/*
	* Waring ! MusicPitch is affect by his groupe Pitch and General Pitch
	*/
	void SetMusicPitch(std::string _name, float _pitch) {
		if (m_musics.find(_name) == m_musics.end()) return;
		m_musics[_name].pitch = _pitch;
		ApplyMusicPitch(_name);
	}


	//Add newThing
	void AddSound(std::string _name, float _volume, float _pitch, std::string _group = "default") {

		if (m_sounds.find(_name) != m_sounds.end()) return;

		SoundData data;
		std::string path = "assets/Sounds/" + _name + ".wav";
		data.sound = LoadSound(path.c_str());

		data.volume = _volume;	
		data.pitch = _pitch;
		data.group = _group;


		if (m_groupVolumePitch.find(_group) == m_groupVolumePitch.end())
			m_groupVolumePitch[_group] = { 1.f, 1.f };

		m_sounds[_name] = data;
	}
	void AddMusic(std::string _name, float _volume, float _pitch, std::string _group = "default") {

		if (m_musics.find(_name) != m_musics.end()) return;
		
		MusicData data;
		std::string path = "assets/Musics/" + _name + ".ogg";
		data.music = LoadMusicStream(path.c_str());

		data.volume = _volume;
		data.pitch = _pitch;
		data.group = _group;

		if (m_groupVolumePitch.find(_group) == m_groupVolumePitch.end())
			m_groupVolumePitch[_group] = { 1.f, 1.f };

		m_musics[_name] = data;
	}
	void AddSong(std::string _name, float _volume, float _pitch) {

		if (m_musics.find(_name) != m_musics.end()) return;

		MusicData data;
		std::string path = "assets/Musics/Songs/" + _name + ".ogg";
		data.music = LoadMusicStream(path.c_str());

		data.volume = _volume;
		data.pitch = _pitch;
		data.group = "music";

		if (m_groupVolumePitch.find("music") == m_groupVolumePitch.end())
			m_groupVolumePitch["music"] = { 1.f, 1.f };

		m_musics[_name] = data;
	}

private:

	void ApplySoundVolume(std::string _name) {
		float actualVolume = m_sounds[_name].volume * m_groupVolumePitch[m_sounds[_name].group].first * m_generalVolume;
		::SetSoundVolume(m_sounds[_name].sound, actualVolume);
	}
	void ApplySoundPitch(std::string _name) {
		float actualPitch = m_sounds[_name].pitch * m_groupVolumePitch[m_sounds[_name].group].second * m_generalPitch;
		::SetSoundPitch(m_sounds[_name].sound, actualPitch);
	}
	
	void ApplyMusicVolume(std::string _name) {
		float actualVolume = m_musics[_name].volume * m_groupVolumePitch[m_musics[_name].group].first * m_generalVolume;
		::SetMusicVolume(m_musics[_name].music, actualVolume);
	}
	void ApplyMusicPitch(std::string _name) {
		float actualPitch = m_musics[_name].pitch * m_groupVolumePitch[m_musics[_name].group].second * m_generalPitch;
		::SetMusicPitch(m_musics[_name].music, actualPitch);
	}

};