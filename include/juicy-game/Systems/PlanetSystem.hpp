#pragma once
#include "MySystem.hpp"

#include "../Components/TransformComponent.hpp"
#include "../Components/PlayerComponent.hpp"
#include "../Components/SphereComponent.hpp"
#include "../Components/ModelComponent.hpp"
#include "../Components/DangerousComponent.hpp"

#include "juicy-game/Assets/ModelManager.hpp"

#include "ecs/World.hpp"
#include "Renderer3D.hpp"

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <cmath>

class PlanetSystem : public MySystem {

	static constexpr float    CHUNK_SIZE = 3000.f;
	static constexpr int      LOAD_RADIUS = 2;
	static constexpr uint32_t GLOBAL_SEED = 42u;
	static constexpr int      MODEL_COUNT = 50;

	struct LoadedChunk {
		int x, y, z;
		std::vector<ee::ecs::EntityID> entities;
	};

	std::unordered_map<uint64_t, LoadedChunk> m_loadedChunks;
	std::shared_ptr<ModelAsset> m_models[MODEL_COUNT];
	Shader m_fogShader = {};
	bool   m_modelsLoaded = false;

	static uint64_t chunkKey(int x, int y, int z) {
		return (uint64_t)(x + 100000) //+100000 pour mettre en positif les position negative, en gros y'a une limite de map
			| ((uint64_t)(y + 100000) << 21)
			| ((uint64_t)(z + 100000) << 42);
	}

	static float lcgRand(uint32_t& seed) {
		seed = seed * 1664525u + 1013904223u; //askip c'est des nombre magic des math
		return (seed >> 8) / 16777216.f; // on cup les resultat et on le met entre 0 et 1
	}

	static uint32_t chunkSeed(int cx, int cy, int cz) {
		uint32_t s = GLOBAL_SEED;
		s ^= (uint32_t)(cx * 2654435761u); // permet d'avoir des resultat different meme si les chucnk sont proche
		s ^= (uint32_t)(cy * 2246822519u);
		s ^= (uint32_t)(cz * 3266489917u);//nb premier
		return s;
	}

	void loadChunk(int cx, int cy, int cz, ee::ecs::World& world) {
		uint64_t key = chunkKey(cx, cy, cz);
		uint32_t seed = chunkSeed(cx, cy, cz);

		LoadedChunk chunk;
		chunk.x = cx; chunk.y = cy; chunk.z = cz;

		float px = cx * CHUNK_SIZE + lcgRand(seed) * CHUNK_SIZE; // on place au hazard dans le chuck
		float py = cy * CHUNK_SIZE + lcgRand(seed) * CHUNK_SIZE;
		float pz = cz * CHUNK_SIZE + lcgRand(seed) * CHUNK_SIZE;

		float radius = 150.f + lcgRand(seed) * 3050.f; 
		float s = radius / 50.f;

		int modelIdx = (int)(lcgRand(seed) * MODEL_COUNT) % MODEL_COUNT;


		auto eid = world.createEntity();
		world.addComponent(eid, TransformComponent{
			{ px, py, pz },
			ee::math::Quaternion::identity(),
			});

		world.addComponent(eid, SphereComponent{ radius / 5.f});
		world.addComponent(eid, MotionComponent{ {0, 0, 0}, {0, 1, 0}, 0, lcgRand(seed) * 2 });
		world.addComponent(eid, DangerousComponent{});

		ModelComponent mc;
		mc.modelAsset = m_models[modelIdx];
		mc.modelScale = { s, s, s };
		mc.rotOffset = 0.f;
		world.addComponent(eid, mc);

		chunk.entities.push_back(eid);

		m_loadedChunks[key] = chunk;
	}

	void unloadChunk(uint64_t key, ee::ecs::World& world) {
		auto it = m_loadedChunks.find(key);
		if (it == m_loadedChunks.end()) return;
		for (auto eid : it->second.entities)
			world.destroyEntity(eid);
		m_loadedChunks.erase(it);
	}

public:

	void init(ee::renderer::Renderer3D& renderer, Shader fogShader) {
		std::cout << "start: " << GetTime() << std::endl;
		m_fogShader = fogShader;
		char path[64];
		char name[32];
		for (int i = 0; i < MODEL_COUNT; i++) {
			snprintf(path, sizeof(path), "assets/Planets/Planet_%d.glb", i + 1);
			snprintf(name, sizeof(name), "planet_%d", i + 1);
			std::cout << "model: " << GetTime() << std::endl;

			m_models[i] = ModelManager::getInstance().getModel(name, path);
			std::cout << "model end: " << GetTime() << std::endl;
		}
		m_modelsLoaded = true;
		std::cout << "end: " << GetTime() << std::endl;
	}

	void update(ee::ecs::World& world, float _dt) override {

		if (m_entities.empty() || !m_modelsLoaded) return;

		auto& t = world.getComponent<TransformComponent>(*m_entities.begin()); // normalement que le player, on recup le premier en tt cas
		int cx = (int)floorf(t.position.x / CHUNK_SIZE);
		int cy = (int)floorf(t.position.y / CHUNK_SIZE); // on calcule le chuck ou il est
		int cz = (int)floorf(t.position.z / CHUNK_SIZE);

		std::unordered_set<uint64_t> wanted;
		for (int dx = -LOAD_RADIUS; dx <= LOAD_RADIUS; dx++)
			for (int dy = -LOAD_RADIUS; dy <= LOAD_RADIUS; dy++)
				for (int dz = -LOAD_RADIUS; dz <= LOAD_RADIUS; dz++)
					wanted.insert(chunkKey(cx + dx, cy + dy, cz + dz)); // on prend les chuck autour de nous

		for (auto key : wanted)
			if (m_loadedChunks.find(key) == m_loadedChunks.end()) { // si jamais il ne sont pas load
				int lx = (int)(key & 0x1FFFFF) - 100000;
				int ly = (int)((key >> 21) & 0x1FFFFF) - 100000;
				int lz = (int)((key >> 42) & 0x1FFFFF) - 100000;  // on decode la key
				loadChunk(lx, ly, lz, world);
			}

		std::vector<uint64_t> toUnload;
		for (auto& [key, chunk] : m_loadedChunks) // si ils sont load mais pas dnas wanted, c'etais le vieux chucks
			if (wanted.find(key) == wanted.end())
				toUnload.push_back(key);
		for (auto key : toUnload)
			unloadChunk(key, world); // on vire
	}
};
