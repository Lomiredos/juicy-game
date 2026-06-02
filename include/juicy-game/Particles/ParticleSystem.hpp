#pragma once
#include <raylib.h>


struct Particle {
	Vector3 position;
	Vector3 velocity;
	Color   colorStart;
	Color   colorEnd;
	float   sizeStart;
	float   sizeEnd;
	float   life;     // temps restant
	float   maxLife;
	bool    active = false;
};


class ParticleSystem {

	static constexpr int MAX_PARTICLES = 1024;

	Particle  m_pool[MAX_PARTICLES];
	Texture2D m_texture = {};

	ParticleSystem() = default;
	ParticleSystem(const ParticleSystem&)            = delete;
	ParticleSystem& operator=(const ParticleSystem&) = delete;

public:

	static ParticleSystem& getInstance() {
		static ParticleSystem instance;
		return instance;
	}

	void init();    // génère la texture billboard (cercle doux)
	void unload();
	void clear();   // désactive toutes les particules

	// Spawn une particule avec contrôle complet
	void spawn(Vector3 _pos, Vector3 _vel,
	           Color _colorStart, Color _colorEnd,
	           float _sizeStart, float _sizeEnd, float _life);

	// Explosion omnidirectionnelle
	void burst(Vector3 _pos, int _count, float _speed,
	           Color _color, float _size, float _life);

	// Trainée continue — appeler chaque frame
	void trail(Vector3 _pos, Vector3 _baseVel,
	           Color _color, float _size, float _life);

	void update(float _dt);
	void render(Camera3D _camera); // doit être appelé entre BeginMode3D et EndMode3D
};
