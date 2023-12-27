/*****************************************************************************
*
* component.h
*
* By Jari Korkala 10/6/2015
*
*****************************************************************************/

#ifndef CENGINE_COMPONENT_H
#define CENGINE_COMPONENT_H

#include <memory>

#include <ceng/datatypes.h>

namespace CEngine
{
	class Component
	{
	protected:
		Component(const Component& component) = delete;
	public:
		Component()
		{
		}

		virtual ~Component()
		{
		}

		virtual void AddComponent(const Ceng::StringUtf8& name, Component* component)
		{

		}

		virtual void RemoveComponent(const Ceng::StringUtf8& name)
		{

		}

		virtual std::shared_ptr<Component> Clone() const = 0;

	};

	

	
}

#endif
