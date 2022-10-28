/*****************************************************************************
*
* entity-registry.cpp
*
* By Jari Korkala 10/2022
*
*****************************************************************************/

#include "entity-registry.h"

using namespace CEngine;

EntityRegistry::EntityRegistry()
{

}

EntityRegistry::~EntityRegistry()
{

}

EngineResult::value EntityRegistry::AddComponentFactory()
{
	return EngineResult::ok;
}

EngineResult::value EntityRegistry::AddEntityType()
{
	return EngineResult::ok;
}