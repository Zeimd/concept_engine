/*****************************************************************************
*
* rotation-factory.cpp
*
* By Jari Korkala 10/2022
*
*****************************************************************************/

#include "rotation-factory.h"
#include "rotation-component.h"

using namespace CEngine;

RotationFactory::RotationFactory()
{

}

RotationFactory::~RotationFactory()
{

}

EngineResult::value RotationFactory::GetInstance(json& data, std::shared_ptr<Component>& output)
{
	output = nullptr;
	
	if (data.is_object())
	{
		json& rotMode = data["rotMode"];

		if (rotMode.is_string() == false)
		{
			return EngineResult::fail;
		}

		EulerMode::value mode;

		if (rotMode == "QUATERNION")
		{
			json& angles = data["angles"];

			if (angles.is_array() == false)
			{
				return EngineResult::fail;
			}

			if (angles.size() != 4)
			{
				return EngineResult::fail;
			}

			for (int i = 0; i < 4; i++)
			{
				if (angles[i].type() != json::value_t::number_float)
				{
					return EngineResult::fail;
				}
			}

			// Resolve overload ambiguity
			output = std::make_shared<RotationComponent>((Ceng::FLOAT32)angles[0], 
				angles[1], angles[2], angles[3]);
		}
		else
		{
			if (rotMode == "EULER_XYZ")
			{
				mode = EulerMode::xyx;
			}
			else if (rotMode == "EULER_YZX")
			{
				mode = EulerMode::yzx;
			}
			else if (rotMode == "EULER_ZXY")
			{
				mode = EulerMode::zxy;
			}
			else if (rotMode == "EULER_XZY")
			{
				mode = EulerMode::xzy;
			}
			else if (rotMode == "EULER_ZYX")
			{
				mode = EulerMode::zyx;
			}
			else if (rotMode == "EULER_YXZ")
			{
				mode = EulerMode::yxz;
			}
			else
			{
				return EngineResult::fail;
			}

			json& angles = data["angles"];

			if (angles.is_array() == false)
			{
				return EngineResult::fail;
			}

			if (angles.size() != 3)
			{
				return EngineResult::fail;
			}

			for (int i = 0; i < 3; i++)
			{
				if (angles[i].type() != json::value_t::number_float)
				{
					return EngineResult::fail;
				}
			}

			output = std::make_shared<RotationComponent>(mode, 
				angles[0], angles[1], angles[2]);
		}		
	}

	return EngineResult::ok;
}