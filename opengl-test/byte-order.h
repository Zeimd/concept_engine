/*****************************************************************************
*
* byte-order.h
*
* By Jari Korkala 3/11/2015
*
*****************************************************************************/

#ifndef CENGINE_BYTE_ORDER_H
#define CENGINE_BYTE_ORDER_H

#include <ceng.h>

namespace CEngine
{
	extern const Ceng::INT32 CorrectByteOrder(const Ceng::INT32 value, const Ceng::BOOL swapByteOrder);
}

#endif

