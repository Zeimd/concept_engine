/*****************************************************************************
*
* scene-manager.cpp
*
* By Jari Korkala 10/2022
*
*****************************************************************************/

#include "json.h"

#include "scene-manager.h"

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

	json materialFile;

	try
	{
		materialFile = json(fileReader);
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

	return EngineResult::ok;


}

void SceneManager::Render()
{

}