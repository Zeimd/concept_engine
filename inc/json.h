/*****************************************************************************
*
* json.h
*
* By Jari Korkala 27/5/2016
*
*****************************************************************************/

#ifndef CENGINE_JSON_H
#define CENGINE_JSON_H

#include <json.hpp>

using json = nlohmann::json;

#include "engine-result.h"

namespace CEngine
{
	void MoveFields(json& dest, json& source);

	const EngineResult::value FindField(json& object, const std::string& name, 
		json::iterator& out);

	const EngineResult::value FindField(json& object, const std::string& name, 
		const json::value_t type, json::iterator& out);

}

#endif