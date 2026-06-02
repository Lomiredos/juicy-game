#include "juicy-game/Particles/ParticleSystem.hpp"



#include <raymath.h>
#include <raylib.h>
#include <cmath>


static Color lerpColor(Color a, Color b, float t)
{
	return {
		(unsigned char)((int)a.r + (int)(((int)b.r - (int)a.r) * t)),
		(unsigned char)((int)a.g + (int)(((int)b.g - (int)a.g) * t)),
		(unsigned char)((int)a.b + (int)(((int)b.b - (int)a.b) * t)),
		(unsigned char)((int)a.a + (int)(((int)b.a - (int)a.a) * t)),
	};
}


void ParticleSystem::init()
{
	Image img = GenImageColor(8, 8, WHITE);
	m_texture = LoadTextureFromImage(img);
	UnloadImage(img);
}

void ParticleSystem::unload()
{
	UnloadTexture(m_texture);
	clear();
}

void ParticleSystem::clear()
{
	for (auto& p : m_pool) p.active = false;
}



void ParticleSystem::spawn(Vector3 _pos, Vector3 _vel,
                           Color _colorStart, Color _colorEnd,
                           float _sizeStart, float _sizeEnd, float _life)
{
	for (auto& p : m_pool) {
		if (p.active) continue;
		p = { _pos, _vel, _colorStart, _colorEnd, _sizeStart, _sizeEnd, _life, _life, true };
		return;
	}
	m_pool[0] = { _pos, _vel, _colorStart, _colorEnd, _sizeStart, _sizeEnd, _life, _life, true };
}

void ParticleSystem::burst(Vector3 _pos, int _count, float _speed,
                           Color _color, float _size, float _life)
{
	Color endColor = _color;
	endColor.a = 0;

	for (int i = 0; i < _count; i++) {
		// Échantillonnage uniforme sur la sphère
		float cosP  = GetRandomValue(-100, 100) * 0.01f;
		float sinP  = sqrtf(1.f - cosP * cosP);
		float theta = GetRandomValue(0, 360) * DEG2RAD;
		Vector3 dir = {
			sinP * cosf(theta),
			cosP,
			sinP * sinf(theta)
		};

		float spd  = _speed * (0.7f + GetRandomValue(0, 60) * 0.01f);
		float life = _life  * (0.7f + GetRandomValue(0, 60) * 0.01f);

		Vector3 vel = { dir.x * spd, dir.y * spd, dir.z * spd };

		spawn(_pos, vel, _color, endColor, _size, 0.f, life);
	}
}

void ParticleSystem::trail(Vector3 _pos, Vector3 _baseVel,
                           Color _color, float _size, float _life)
{
	Color endColor = _color;
	endColor.a = 0;

	Vector3 vel = {
		_baseVel.x + GetRandomValue(-10, 10) * 0.1f,
		_baseVel.y + GetRandomValue(-10, 10) * 0.1f,
		_baseVel.z + GetRandomValue(-10, 10) * 0.1f,
	};

	spawn(_pos, vel, _color, endColor, _size, 0.f, _life);
}


void ParticleSystem::update(float _dt)
{
	for (auto& p : m_pool) {
		if (!p.active) continue;

		p.position.x += p.velocity.x * _dt;
		p.position.y += p.velocity.y * _dt;
		p.position.z += p.velocity.z * _dt;

		// Drag
		p.velocity.x *= 0.96f;
		p.velocity.y *= 0.96f;
		p.velocity.z *= 0.96f;

		p.life -= _dt;
		if (p.life <= 0.f) p.active = false;
	}
}

void ParticleSystem::render(Camera3D _camera)
{
	BeginBlendMode(BLEND_ADDITIVE);

	Rectangle src = { 0, 0, (float)m_texture.width, (float)m_texture.height };

	for (auto& p : m_pool) {
		if (!p.active) continue;

		float t    = 1.f - p.life / p.maxLife;
		Color col  = lerpColor(p.colorStart, p.colorEnd, t);
		float size = p.sizeStart + (p.sizeEnd - p.sizeStart) * t;

		// Étirement dans la direction de la vélocité
		float speed = sqrtf(p.velocity.x * p.velocity.x +
		                    p.velocity.y * p.velocity.y +
		                    p.velocity.z * p.velocity.z);

		Vector3 up;
		if (speed > 0.5f) {
			up = { p.velocity.x / speed, p.velocity.y / speed, p.velocity.z / speed };
		} else {
			up = { 0.f, 1.f, 0.f };
		}

		// Évite le billboard dégénéré quand up // direction de vue
		Vector3 look = Vector3Normalize(Vector3Subtract(p.position, _camera.position));
		if (fabsf(Vector3DotProduct(up, look)) > 0.99f) {
			// Choisir l'axe le moins aligné avec look comme pivot
			Vector3 axis = (fabsf(look.x) <= fabsf(look.y) && fabsf(look.x) <= fabsf(look.z))
			             ? Vector3{ 1.f, 0.f, 0.f }
			             : (fabsf(look.y) <= fabsf(look.z))
			             ? Vector3{ 0.f, 1.f, 0.f }
			             : Vector3{ 0.f, 0.f, 1.f };
			up = Vector3Normalize(Vector3CrossProduct(look, axis));
		}

		float stretch = 1.f + speed * 0.003f;
		Vector2 sz    = { size, size * stretch };

		DrawBillboardPro(_camera, m_texture, src, p.position, up, sz, { 0, 0 }, 0.f, col);
	}

	EndBlendMode();
}
