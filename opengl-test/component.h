/*****************************************************************************
*
* component.h
*
* By Jari Korkala 10/6/2015
*
*****************************************************************************/

#ifndef CENGINE_COMPONENT_H
#define CENGINE_COMPONENT_H

#include <ceng/datatypes.h>

namespace CEngine
{
	class Component
	{
	public:
		Component()
		{
		}

		virtual ~Component()
		{
		}

		virtual void AddComponent(const Ceng::StringUtf8 &name, Component *component)
		{
		}

		virtual void RemoveComponent(const Ceng::StringUtf8 &name)
		{
		}
	};

	

	
}

#endif
