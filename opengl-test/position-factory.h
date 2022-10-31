/*****************************************************************************
*
* position-factory.h
*
* By Jari Korkala 10/2022
*
*****************************************************************************/

#ifndef CENGINE_POSITION_FACTORY_H
#define CENGINE_POSITION_FACTORY_H

#include "component-factory.h"

namespace CEngine
{
	class PositionFactory : public ComponentFactory
	{
	public:
		PositionFactory();
		~PositionFactory() override;

		EngineResult::value GetInstance(json& data, std::shared_ptr<Component>& output) override;

	};

}

#endif