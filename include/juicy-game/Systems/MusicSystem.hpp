#pragma once

#include "MySystem.hpp"
#include <raylib.h>
#include <cstdlib>
#include <cstring>

class MusicSystem : public MySystem {

    static const int MAX_SONGS = 64;

    char m_songPaths [MAX_SONGS][256] = {};
    char m_songTitles[MAX_SONGS][64]  = {};
    int  m_songCount  = 0;
    int  m_currentIdx = -1;

    Music m_music     = {};
    bool  m_loaded    = false;
    Sound m_skipSound = {};

    float m_volume    = 0.f;
    float m_fadeTimer = 0.f;
    static constexpr float FADE_DURATION = 2.f;

    enum class State { FadingIn, Playing, FadingOut } m_state = State::FadingOut;

    Font m_font = {};

public:

    void init()
    {
        m_font      = LoadFontEx("assets/Fonts/space.otf", 24, 0, 250);
        m_skipSound = LoadSound("assets/Sounds/skip.wav");

        FilePathList files = LoadDirectoryFilesEx("assets/Musics/Songs", ".ogg", false);
        for (int i = 0; i < (int)files.count && m_songCount < MAX_SONGS; i++) {
            snprintf(m_songPaths[m_songCount],  256, "%s", files.paths[i]);
            snprintf(m_songTitles[m_songCount], 64,  "%s", GetFileNameWithoutExt(files.paths[i]));
            m_songCount++;
        }
        UnloadDirectoryFiles(files);

        if (m_songCount > 0)
            loadSong(rand() % m_songCount);
    }

    void update(ee::ecs::World&, float _dt) override
    {
        if (!m_loaded || m_songCount == 0) return;

        UpdateMusicStream(m_music);

        if (IsKeyPressed(KEY_O)) {
            PlaySound(m_skipSound);
            startFadeOut();
        }

        switch (m_state)
        {
        case State::Playing:
            if (GetMusicTimePlayed(m_music) >= GetMusicTimeLength(m_music) - FADE_DURATION)
                startFadeOut();
            break;

        case State::FadingOut:
            m_fadeTimer += _dt;
            m_volume = 1.f - (m_fadeTimer / FADE_DURATION);
            m_volume *= 3.f;
            if (m_volume <= 0.f) {
                unloadCurrent();
                loadSong(rand() % m_songCount);
            } else {
                SetMusicVolume(m_music, m_volume);
            }
            break;

        case State::FadingIn:
            m_fadeTimer += _dt;
            m_volume = m_fadeTimer / FADE_DURATION;
            m_volume *= 3.f;
            if (m_volume >= 3.f) {
                m_volume = 3.f;
                m_state  = State::Playing;
            }
            SetMusicVolume(m_music, m_volume);
            break;
        }
    }

    void render(ee::ecs::World& /*_world*/, ee::renderer::Renderer3D& _renderer, bool /*_debug*/) override
    {
        if (!m_loaded || m_currentIdx < 0) return;

        float played = GetMusicTimePlayed(m_music);
        float total  = GetMusicTimeLength(m_music);

        const char* timeStr = TextFormat("%02d:%02d / %02d:%02d",
            (int)played / 60, (int)played % 60,
            (int)total  / 60, (int)total  % 60);

        _renderer.DrawText(m_font, m_songTitles[m_currentIdx], { 10.f, 10.f }, 18.f, 2.f, WHITE);
        _renderer.DrawText(m_font, timeStr,                     { 10.f, 32.f }, 16.f, 2.f, LIGHTGRAY);
    }

private:

    void startFadeOut()
    {
        if (m_state == State::FadingOut) return;
        m_state     = State::FadingOut;
        m_fadeTimer = 0.f;
    }

    void unloadCurrent()
    {
        if (m_loaded) {
            StopMusicStream(m_music);
            UnloadMusicStream(m_music);
            m_loaded = false;
        }
    }

    void loadSong(int idx)
    {
        m_currentIdx = idx;
        m_music      = LoadMusicStream(m_songPaths[idx]);
        m_loaded     = true;
        m_volume     = 0.f;
        m_fadeTimer  = 0.f;
        m_state      = State::FadingIn;
        SetMusicVolume(m_music, 0.f);
        PlayMusicStream(m_music);
    }
};
