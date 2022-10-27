/*****************************************************************************
*
* scene-manager.cpp
*
* By Jari Korkala 10/2022
*
*****************************************************************************/

#include "json.h"

#include "scene-manager.h"
#include "mesh-manager.h"

using namespace CEngine;

SceneManager::SceneManager()
{

}

SceneManager::~SceneManager()
{

}

void SceneManager::Unload()
{

}

EngineResult::value SceneManager::LoadScene(const Ceng::StringUtf8& file, Ceng::StringUtf8& log)
{
	Unload();

	Ceng::StringUtf8 filename;

	Ceng_GetFilename(file, filename);

	Ceng::StringUtf8 readPath;

	Ceng_ApplyRelativePath(assetPath, file, readPath);

	std::ifstream fileReader;

	fileReader.open(readPath.ToCString(), std::ios_base::in);
	if (!fileReader.good())
	{
		log += "Error loading scene file : ";
		log += file;
		log += " : file not found\n";

		return EngineResult::file_not_found;
	}

	if (fileReader.eof())
	{
		return EngineResult::fail;
	}

	try
	{
		sceneFile = json(fileReader);
	}
	catch (std::exception& e)
	{
		log += "Error parsing scene file : ";
		log += file;
		log += " : ";
		log += e.what();
		log += "\n";
		Ceng::Log::Print(log);
		return EngineResult::fail;
	}

	EngineResult::value eresult;

	for (auto iter = sceneFile.begin(); iter != sceneFile.end(); ++iter)
	{
		if (iter.key() == "static_geometry")
		{
			eresult = LoadStaticGeometry(iter.value());		

			if (eresult != EngineResult::ok)
			{
				return eresult;
			}
		}	
	}

	return EngineResult::ok;


}

EngineResult::value SceneManager::LoadStaticGeometry(json& data)
{
	EngineResult::value eresult;

	for (auto iter = data.begin(); iter != data.end(); ++iter)
	{
		if (iter.key() == "mesh")
		{
			eresult = LoadMesh(iter.value(), worldMesh);

			if (eresult != EngineResult::ok)
			{
				return eresult;
			}
		}
		else if (iter.key() == "envProbe")
		{
			eresult = LoadEnvProbes(iter.value());

			if (eresult != EngineResult::ok)
			{
				return eresult;
			}
		}
	}

	return EngineResult::ok;
}

EngineResult::value SceneManager::LoadEnvProbes(json& data)
{
	EngineResult::value eresult;

	for (auto iter = data.begin(); iter != data.end(); ++iter)
	{


	}

	return EngineResult::ok;
}

EngineResult::value SceneManager::LoadMesh(json& data, std::shared_ptr<Mesh>& output)
{
	output = nullptr;

	Ceng::StringUtf8 fileName, meshName;

	for (auto iter = data.begin(); iter != data.end(); ++iter)
	{
		if (iter.key() == "fileName" && iter.value().is_string())
		{
			std::string temp = iter.value();
			fileName = temp;
		}
		else if (iter.key() == "meshName" && iter.value().is_string())
		{
			std::string temp = iter.value();
			meshName = temp;
		}
		else
		{
			return EngineResult::fail;
		}
	}

	EngineResult::value eresult;

	std::shared_ptr<CEngine::Mesh> mesh;

	eresult = meshManager->LoadMesh(fileName, meshName, mesh);

	if (eresult != EngineResult::ok)
	{
		return eresult;
	}

	output = mesh;

	return EngineResult::ok;
}

void SceneManager::Render()
{

}