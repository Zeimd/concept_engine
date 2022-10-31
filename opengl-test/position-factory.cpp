/*****************************************************************************
*
* position-factory.cpp
*
* By Jari Korkala 10/2022
*
*****************************************************************************/

#include "json.h"
#include "position-factory.h"
#include "position-component.h"

using namespace CEngine;

PositionFactory::PositionFactory()
{

}

PositionFactory::~PositionFactory()
{

}

EngineResult::value PositionFactory::GetInstance(json& data, std::shared_ptr<Component>& output)
{
	if (data.is_array() == false)
	{
		return EngineResult::fail;
	}

	if (data.size() != 3)
	{
		return EngineResult::fail;
	}

	for (auto& iter : data)
	{
		if (iter.type() != json::value_t::number_float)
		{
			return EngineResult::fail;
		}
	}

	auto comp = std::make_shared<PositionComponent>(data[0], data[1], data[2]);

	output = comp;

	return EngineResult::ok;
}