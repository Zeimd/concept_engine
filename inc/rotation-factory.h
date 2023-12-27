/*****************************************************************************
*
* rotation-factory.h
*
* By Jari Korkala 10/2022
*
*****************************************************************************/

#ifndef CENGINE_ROTATION_FACTORY_H
#define CENGINE_ROTATION_FACTORY_H

#include "component-factory.h"

namespace CEngine
{
	class RotationFactory : public ComponentFactory
	{
	public:
		RotationFactory();
		~RotationFactory() override;

		EngineResult::value GetInstance(json& data, std::shared_ptr<Component>& output) override;
	};
}

#endif