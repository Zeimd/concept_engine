/*****************************************************************************
*
* scene-manager.h
*
* By Jari Korkala 10/2022
*
*****************************************************************************/

#ifndef CENGINE_SCENE_MANAGER_H
#define CENGINE_SCENE_MANAGER_H

#include "manager.h"
#include "engine-result.h"

namespace CEngine
{
	class ShaderManager;
	class MeshManager;
	class MaterialManager;
	class EnvMapManager;

	class SceneManager : public Manager
	{
	protected:

		ShaderManager* shaderManager;
		MeshManager* meshManager;
		EnvMapManager* envMapManager;

	public:
		SceneManager();

		~SceneManager() override;

		EngineResult::value LoadScene(const Ceng::StringUtf8& file, Ceng::StringUtf8& log);

		// Unload current scene.
		// NOTE: Other managers are pruned only after next scene has loaded
		//       unless out of RAM
		void Unload();

		// Render scene
		void Render();
	};
}

#endif