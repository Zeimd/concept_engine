/*****************************************************************************
*
* byte-order.cpp
*
* Created By Jari Korkala 26/10/2015
*
* Corrects endianess.
*
*****************************************************************************/

#include "byte-order.h"

namespace CEngine
{
	const Ceng::INT32 CorrectByteOrder(const Ceng::INT32 value, const Ceng::BOOL swapByteOrder)
	{
		if (swapByteOrder == false)
		{
			return value;
		}
		else
		{
			Ceng::UINT8 *byte = (Ceng::UINT8*)&value;

			return byte[3] | (byte[2] << 8) | (byte[1] << 16) | (byte[0] << 24);
		}
	}
}