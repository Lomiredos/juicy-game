#pragma once

#include "MySystem.hpp"
#include "../Components/TransformComponent.hpp"
#include "../Components/SphereComponent.hpp"
#include "../Components/ModelComponent.hpp"
#include "../Components/PlayerComponent.hpp"
#include "Renderer3D.hpp"
#include "raymath.h"
#include "rlgl.h"

class RenderSystem : public MySystem
{
	static constexpr float FOG_START = 4500.f;
	static constexpr float FOG_END   = 5000.f;

	Shader         m_fogShader;
	Mesh           m_skyboxMesh;
	Model          m_skyboxModel;
	Shader         m_skyboxShader;
	TextureCubemap m_cubeMap;

	int fogColorLoc;
	int fogEndLoc;
	int fogStartLoc;
	int cameraPosLoc;

public:

	Shader getFogShader() const { return m_fogShader; }

	void init() {
		m_fogShader  = LoadShader("assets/Shaders/fog.vs", "assets/Shaders/fog.fs");
		fogColorLoc  = GetShaderLocation(m_fogShader, "fogColor");
		fogEndLoc    = GetShaderLocation(m_fogShader, "fogEnd");
		fogStartLoc  = GetShaderLocation(m_fogShader, "fogStart");
		cameraPosLoc = GetShaderLocation(m_fogShader, "cameraPos");
		initSkybox();
	}

	void render(ee::ecs::World& _world, ee::renderer::Renderer3D& _renderer, bool _showDebug) override
	{
		renderSkybox(_renderer);

		for (auto& id : m_entities)
		{
			auto& t = _world.getComponent<TransformComponent>(id);
			auto& s = _world.getComponent<SphereComponent>(id);

			if (_world.hasComponent<ModelComponent>(id)) {

				rlDisableBackfaceCulling();
				auto& mo  = _world.getComponent<ModelComponent>(id);
				Model& mdl = mo.modelAsset->getModel();

				_renderer.PushMatrix();
				_renderer.Translate(t.position.x, t.position.y, t.position.z);
				Quaternion rq = { t.orientation.x, t.orientation.y, t.orientation.z, t.orientation.w };
				rlMultMatrixf(MatrixToFloat(QuaternionToMatrix(rq)));

				if (_world.hasComponent<PlayerComponent>(id)) {
					_renderer.Rotate(_world.getComponent<PlayerComponent>(id).bankAngle, 0, 0, 1);
				}

				for (int i = 0; i < mdl.materialCount; i++)
					mdl.materials[i].shader = m_fogShader;
				passShaderInfo(_renderer.GetCamera().position);
				DrawModelEx(mdl, { 0, 0, 0 }, { 0, 1, 0 }, mo.rotOffset, mo.modelScale, WHITE);
				_renderer.PopMatrix();

				rlEnableBackfaceCulling();
			}
			else {
				_renderer.PushMatrix();
				_renderer.Translate(t.position.x, t.position.y, t.position.z);
				Quaternion rq = { t.orientation.x, t.orientation.y, t.orientation.z, t.orientation.w };
				Matrix m = QuaternionToMatrix(rq);
				rlMultMatrixf(MatrixToFloat(m));
				_renderer.DrawSphere({0, 0, 0 }, s.radius, RED);
				
				_renderer.PopMatrix();
			}

			if (_showDebug) {
				_renderer.PushMatrix();
				_renderer.Translate(t.position.x, t.position.y, t.position.z);
				Quaternion rq = { t.orientation.x, t.orientation.y, t.orientation.z, t.orientation.w };
				Matrix m = QuaternionToMatrix(rq);
				rlMultMatrixf(MatrixToFloat(m));
				_renderer.DrawSphereWires({ 0, 0, 0 }, s.radius, RED);
				_renderer.PopMatrix();
			}
		}
	}

private:

	void passShaderInfo(Vector3 camPos) {
		Vector4 fogColor = { 0.0f, 0.0f, 0.0f, 0.0f };
		SetShaderValue(m_fogShader, fogColorLoc,  &fogColor,   SHADER_UNIFORM_VEC4);
		SetShaderValue(m_fogShader, fogStartLoc,  &FOG_START,  SHADER_UNIFORM_FLOAT);
		SetShaderValue(m_fogShader, fogEndLoc,    &FOG_END,    SHADER_UNIFORM_FLOAT);
		SetShaderValue(m_fogShader, cameraPosLoc, &camPos,     SHADER_UNIFORM_VEC3);
	}

	void initSkybox() {
		const char* facePaths[6] = {
			"assets/SkyBoxs/Corona/corona_lf.png",
			"assets/SkyBoxs/Corona/corona_rt.png",
			"assets/SkyBoxs/Corona/corona_up.png",
			"assets/SkyBoxs/Corona/corona_dn.png",
			"assets/SkyBoxs/Corona/corona_bk.png",
			"assets/SkyBoxs/Corona/corona_ft.png",
		};

		Image faces[6];
		for (int i = 0; i < 6; i++)
			faces[i] = LoadImage(facePaths[i]);

		int fw = faces[0].width;
		int fh = faces[0].height;
		Image combined = GenImageColor(fw * 6, fh, BLACK); // on cree une image qui fait 6 fois la width de nos image

		for (int i = 0; i < 6; i++) {
			ImageDraw(&combined, faces[i],
				{ 0, 0, (float)fw, (float)fh },
				{ (float)(i * fw), 0, (float)fw, (float)fh },
				WHITE);
			UnloadImage(faces[i]); // on ecris nos image dedant et on les unload
		}

		m_cubeMap = LoadTextureCubemap(combined, CUBEMAP_LAYOUT_LINE_HORIZONTAL); // on load le cube avec notre grande image
		UnloadImage(combined);

		m_skyboxMesh   = GenMeshCube(1.0f, 1.0f, 1.0f);
		m_skyboxModel  = LoadModelFromMesh(m_skyboxMesh);
		m_skyboxShader = LoadShader("assets/Shaders/skybox.vs", "assets/Shaders/skybox.fs");

		for (int i = 0; i < m_skyboxModel.materialCount; i++) {
			m_skyboxModel.materials[i].shader = m_skyboxShader;
			m_skyboxModel.materials[i].maps[MATERIAL_MAP_CUBEMAP].texture = m_cubeMap;
		}

		// Indique au sampler sur quelle unit de texture la cubemap est bindee
		int envMapLoc = GetShaderLocation(m_skyboxShader, "environmentMap");
		int envMapSlot = MATERIAL_MAP_CUBEMAP;
		SetShaderValue(m_skyboxShader, envMapLoc, &envMapSlot, SHADER_UNIFORM_INT);

		printf(m_skyboxShader.id ? "Skybox shader OK id:%d\n" : "ERREUR skybox shader\n", m_skyboxShader.id);
		printf(m_cubeMap.id      ? "Cubemap OK id:%d\n"       : "ERREUR cubemap\n",       m_cubeMap.id);
	}


	void renderSkybox(ee::renderer::Renderer3D& _renderer) {
		Camera cam = _renderer.GetCamera();
		rlDisableBackfaceCulling();
		rlDisableDepthMask();
		DrawModel(m_skyboxModel, cam.position, 1.f, WHITE);
		rlEnableDepthMask();
		rlEnableBackfaceCulling();
	}
};
