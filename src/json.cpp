/*****************************************************************************
*
* json.cpp
*
* By Jari Korkala 10/2022
*
*****************************************************************************/

#include "json.h"

namespace CEngine
{

	void MoveFields(json& dest, json& source)
	{
		for (auto iter = source.begin(); iter != source.end(); ++iter)
		{
			dest[iter.key()] = iter.value();
		}
	}

	const EngineResult::value FindField(json& object, const std::string& name, json::iterator& out)
	{
		out = object.find(name);

		if (out == object.end())
		{
			return EngineResult::fail;
		}

		return EngineResult::ok;
	}

	const EngineResult::value FindField(json& object, const std::string& name, 
		const json::value_t type, json::iterator& out)
	{
		/*
		std::string text = "FindField : ";
		text += name;
		text += '\n';

		Ceng::Log::Print(text);

		text = "Object : \n";
		text += object.dump();
		text += '\n';

		Ceng::Log::Print(text);
		*/

		out = object.find(name);

		if (out == object.end())
		{
			//		Ceng::Log::Print("FindField : not found : " + name);
			return EngineResult::fail;
		}

		if ((*out).type() != type)
		{
			//		Ceng::Log::Print("FindField : type mismatch");
			return EngineResult::not_supported;
		}

		return EngineResult::ok;
	}

}