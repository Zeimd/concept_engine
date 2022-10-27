/*****************************************************************************
*
* scene-manager.h
*
* By Jari Korkala 10/2022
*
*****************************************************************************/

#ifndef CENGINE_SCENE_MANAGER_H
#define CENGINE_SCENE_MANAGER_H

#include <memory>

#include "manager.h"
#include "engine-result.h"

namespace CEngine
{
	class ShaderManager;
	class MeshManager;
	class MaterialManager;
	class EnvMapManager;
	class Mesh;

	class SceneManager : public Manager
	{
	protected:

		ShaderManager* shaderManager;
		MeshManager* meshManager;
		EnvMapManager* envMapManager;

		json sceneFile;

		std::shared_ptr<Mesh> worldMesh;

	public:
		SceneManager();

		~SceneManager() override;

		EngineResult::value LoadScene(const Ceng::StringUtf8& file, Ceng::StringUtf8& log);

	protected:

		EngineResult::value LoadStaticGeometry(json& data);

		EngineResult::value LoadEnvProbes(json& data);

		EngineResult::value LoadMesh(json& data, std::shared_ptr<Mesh> &output);

		// Unload current scene.
		// NOTE: Other managers are pruned only after next scene has loaded
		//       unless out of RAM
		void Unload();

		// Render scene
		void Render();
	};
}

#endif